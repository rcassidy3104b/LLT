#include "LLT.h"
#include <sstream>
#include <iomanip>

LLT::LLT()
	: N(0), b(0.0f), elip(false), station(0),
	  y(), c(), a0(), alfa0(), twist()
{
}

LLT::LLT(int N, float b, bool elip, int station,
		 const vector<float>& y,
		 const vector<float>& c,
		 const vector<float>& a0,
		 const vector<float>& alfa0,
		 const vector<float>& twist)
	: N(N), b(b), elip(elip), station(station),
	  y(y), c(c), a0(a0), alfa0(alfa0), twist(twist)
{
	for (int i = 0;i < N;i++) {
		theta_n.push_back(((i + 1) * M_PI) / (N + 1));
		y_t.push_back(-cos(theta_n[i]) * b / 2);
	}

	// interpolate c, a0, alfa0, twist onto the y_t stations
	c_t.resize(N);
	a0_t.resize(N);
	alfa0_t.resize(N);
	twist_t.resize(N);

	// if no input station data, leave interpolated vectors as zeros
	if (y.empty()) return;

	// helper lambda for linear interpolation given input arrays
	auto interp = [&](const vector<float>& src, int idx) -> float {
		// if only one input value, return it
		if (src.size() == 1 || y.size() == 1) return src[0];
		float yt = y_t[idx];
		// if yt is below first y, clamp
		if (yt <= y.front()) return src.front();
		// if yt is above last y, clamp
		if (yt >= y.back()) return src.back();
		// find interval
		for (size_t j = 0; j + 1 < y.size(); ++j) {
			float y0 = y[j];
			float y1 = y[j+1];
			if (yt >= y0 && yt <= y1) {
				float t = (yt - y0) / (y1 - y0);
				return src[j] + t * (src[j+1] - src[j]);
			}
		}
		// fallback	
		return src.back();
	};

	for (int i = 0; i < N; ++i) {
		c_t[i] = interp(c, i);
		a0_t[i] = interp(a0, i);
		alfa0_t[i] = interp(alfa0, i);
		twist_t[i] = interp(twist, i);
	}
	for (int l = 0;l < N;l++) {
		vector<float> row;
		for(int m = 0;m<N;m++){
			row.push_back(((4 * b * sin(m * theta_n[l])) / (alfa0_t[l] * c_t[l])) + (m * sin(m * theta_n[l]) / sin(theta_n[l])));
		}
		Ic.push_back(row);
	}

	// compute and store inverse of Ic right after population
	// if inversion fails (singular), Ic_inv will remain empty
	invertIc();
}

// Compute A = Ic_inv * (alphaIn + twist_t - alfa0_t)
vector<float> LLT::results(float alphaIn)
{
	// ensure sizes are valid
	if (static_cast<int>(twist_t.size()) != N || static_cast<int>(alfa0_t.size()) != N) {
		return vector<float>(); // invalid state
	}

	// ensure inverse is present (attempt to compute if not)
	if (Ic_inv.empty()) {
		if (!invertIc()) return vector<float>(); // singular or invalid
	}

	if (static_cast<int>(Ic_inv.size()) != N) return vector<float>();

	// build RHS: rhs[i] = alphaIn + twist_t[i] - alfa0_t[i]
	vector<float> rhs(N, 0.0f);
	for (int i = 0; i < N; ++i) {
		rhs[i] = alphaIn + twist_t[i] - alfa0_t[i];
	}

	// multiply A = Ic_inv * rhs
	vector<float> A(N, 0.0f);
	for (int i = 0; i < N; ++i) {
		float sum = 0.0f;
		for (int j = 0; j < N; ++j) {
			sum += Ic_inv[i][j] * rhs[j];
		}
		A[i] = sum;
	}

	return A;
}

bool LLT::invertIc()
{
	// clear previous inverse
	Ic_inv.clear();

	// Basic checks
	if (Ic.size() != static_cast<size_t>(N)) return false;
	for (const auto& row : Ic) {
		if (row.size() != static_cast<size_t>(N)) return false;
	}

	if (N == 0) return false;

	// Create augmented matrix [Ic | I]
	vector<vector<float>> aug(N, vector<float>(2 * N, 0.0f));
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			aug[i][j] = Ic[i][j];
		}
		aug[i][N + i] = 1.0f;
	}

	const float EPS = 1e-8f;

	// Gauss-Jordan with partial pivoting
	for (int col = 0; col < N; ++col) {
		// find pivot
		int pivot = col;
		float maxVal = fabs(aug[col][col]);
		for (int r = col + 1; r < N; ++r) {
			float val = fabs(aug[r][col]);
			if (val > maxVal) {
				maxVal = val;
				pivot = r;
			}
		}
		if (maxVal < EPS) {
			// singular matrix
			Ic_inv.clear();
			return false;
		}
		// swap if needed
		if (pivot != col) {
			std::swap(aug[pivot], aug[col]);
		}
		// normalize pivot row
		float pivVal = aug[col][col];
		for (int j = 0; j < 2 * N; ++j) {
			aug[col][j] /= pivVal;
		}
		// eliminate other rows
		for (int r = 0; r < N; ++r) {
			if (r == col) continue;
			float factor = aug[r][col];
			if (fabs(factor) < EPS) continue;
			for (int j = 0; j < 2 * N; ++j) {
				aug[r][j] -= factor * aug[col][j];
			}
		}
	}

	// extract inverse from augmented matrix
	Ic_inv.assign(N, vector<float>(N, 0.0f));
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			Ic_inv[i][j] = aug[i][N + j];
		}
	}
	return true;
}

std::string LLT::to_str() const
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6);
    oss << "N: " << N << "\n";
    oss << "b: " << b << "\n";
    oss << "elip: " << (elip ? "true" : "false") << "\n";
    oss << "station: " << station << "\n";

    auto appendVec = [&](const std::string& name, const std::vector<float>& v) {
        oss << name << ": [";
        for (size_t i = 0; i < v.size(); ++i) {
            if (i) oss << ", ";
            oss << v[i];
        }
        oss << "]\n";
    };

    appendVec("y", y);
    appendVec("c", c);
    appendVec("a0", a0);
    appendVec("alfa0", alfa0);
    appendVec("twist", twist);

    appendVec("theta_n", theta_n);
    appendVec("y_t", y_t);
    appendVec("c_t", c_t);
    appendVec("a0_t", a0_t);
    appendVec("alfa0_t", alfa0_t);
    appendVec("twist_t", twist_t);

    oss << "Ic: [";
    for (size_t i = 0; i < Ic.size(); ++i) {
        if (i) oss << ", ";
        oss << "[";
        for (size_t j = 0; j < Ic[i].size(); ++j) {
            if (j) oss << ", ";
            oss << Ic[i][j];
        }
        oss << "]";
    }
    oss << "]\n";

    // print Ic_inv if present
    if (!Ic_inv.empty()) {
        oss << "Ic_inv: [";
        for (size_t i = 0; i < Ic_inv.size(); ++i) {
            if (i) oss << ", ";
            oss << "[";
            for (size_t j = 0; j < Ic_inv[i].size(); ++j) {
                if (j) oss << ", ";
                oss << Ic_inv[i][j];
            }
            oss << "]";
        }
        oss << "]\n";
    } else {
        oss << "Ic_inv: []\n";
    }

    return oss.str();
}

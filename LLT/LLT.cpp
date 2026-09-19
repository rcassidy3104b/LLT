#include "LLT.h"
#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>

namespace {
	// small helpers copied/adapted from previous main.cpp
	static std::string trim(const std::string& s)
	{
		std::size_t a = s.find_first_not_of(" \t\r\n");
		if (a == std::string::npos) return {};
		std::size_t b = s.find_last_not_of(" \t\r\n");
		return s.substr(a, b - a + 1);
	}

	static std::string firstField(const std::string& line)
	{
		std::istringstream ss(line);
		std::string tok;
		if (!std::getline(ss, tok, ',')) return {};
		return trim(tok);
	}
}

// Default ctor
LLT::LLT()
	: N(0), b(0.0f), elip(false), station(0),
	  y(), c(), a0(), alfa0(), twist(),
	  theta_n(), y_t(), c_t(), a0_t(), alfa0_t(), twist_t(),
	  Ic(), Ic_inv(), aRatio(0.0f)
{
	// nothing to compute
}

// Parameterized ctor — set members and centralise all computation in computeAll()
LLT::LLT(int N_, float b_, bool elip_, int station_,
		 const vector<float>& y_,
		 const vector<float>& c_,
		 const vector<float>& a0_,
		 const vector<float>& alfa0_,
		 const vector<float>& twist_)
	: N(N_), b(b_), elip(elip_), station(station_),
	  y(y_), c(c_), a0(a0_), alfa0(alfa0_), twist(twist_),
	  theta_n(), y_t(), c_t(), a0_t(), alfa0_t(), twist_t(),
	  Ic(), Ic_inv(), aRatio(0.0f)
{
	computeAll();
}

// File-parsing constructor: moves main.cpp parsing logic here
LLT::LLT(const std::string& filename)
	: N(0), b(0.0f), elip(false), station(0),
	  y(), c(), a0(), alfa0(), twist(),
	  theta_n(), y_t(), c_t(), a0_t(), alfa0_t(), twist_t(),
	  Ic(), Ic_inv(), aRatio(0.0f)
{
	std::ifstream inFile(filename);
	if (!inFile) {
		// leave default values; user can check getN/getB etc
		return;
	}

	std::string line;

	// Read header values: b, N, elip, station (each on its own row, first column)
	if (std::getline(inFile, line)) {
		try { b = std::stof(firstField(line)); } catch (...) { b = 0.0f; }
	}
	if (std::getline(inFile, line)) {
		try { N = std::stoi(firstField(line)); } catch (...) { N = 0; }
	}
	if (std::getline(inFile, line)) {
		try { elip = (std::stoi(firstField(line)) != 0); } catch (...) { elip = false; }
	}
	if (std::getline(inFile, line)) {
		try { station = std::stoi(firstField(line)); } catch (...) { station = 0; }
	}

	// reserve station vectors
	y.clear(); c.clear(); a0.clear(); alfa0.clear(); twist.clear();
	y.reserve(std::max(0, station));
	c.reserve(std::max(0, station));
	a0.reserve(std::max(0, station));
	alfa0.reserve(std::max(0, station));
	twist.reserve(std::max(0, station));

	// Read station rows (each row has: y,c,a0,alfa0,twist)
	for (int i = 0; i < station; ++i) {
		if (!std::getline(inFile, line)) {
			// If rows missing, fill remaining with zeros
			y.push_back(0.0f);
			c.push_back(0.0f);
			a0.push_back(0.0f);
			alfa0.push_back(0.0f);
			twist.push_back(0.0f);
			continue;
		}

		std::istringstream ss(line);

		auto parseTok = [&ss]() -> float {
			std::string t;
			if (!std::getline(ss, t, ',')) return 0.0f;
			t = trim(t);
			if (t.empty()) return 0.0f;
			try { return std::stof(t); } catch (...) { return 0.0f; }
		};

		y.push_back(parseTok());
		c.push_back(parseTok());
		a0.push_back(parseTok());
		alfa0.push_back(parseTok());
		twist.push_back(parseTok());
	}

	// now perform calculations
	computeAll();
}

// centralised computation moved here from the old parameterized constructor
void LLT::computeAll()
{
	// clear previous computed containers
	theta_n.clear();
	y_t.clear();
	c_t.clear();
	a0_t.clear();
	alfa0_t.clear();
	twist_t.clear();
	Ic.clear();
	Ic_inv.clear();
	aRatio = 0.0f;

	// basic checks
	if (N <= 0 || b == 0.0f) return;

	// compute theta_n and y_t
	theta_n.reserve(N);
	y_t.reserve(N);
	for (int i = 0; i < N; ++i) {
		theta_n.push_back(((i + 1) * M_PI) / (N + 1));
		y_t.push_back(-cos(theta_n[i]) * b / 2);
	}

	// interpolate c, a0, alfa0, twist onto the y_t stations
	c_t.assign(N, 0.0f);
	a0_t.assign(N, 0.0f);
	alfa0_t.assign(N, 0.0f);
	twist_t.assign(N, 0.0f);

	// if no input station data, leave interpolated vectors as zeros and do not build Ic
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

	// compute wing area S from provided y and c (trapezoidal rule).
	float S = 0.0f;
	if (y.size() >= 2 && c.size() >= 2) {
		for (size_t i = 0; i + 1 < y.size() && i + 1 < c.size(); ++i) {
			float dx = y[i + 1] - y[i];
			S += 0.5f * (c[i] + c[i + 1]) * dx;
		}
		// if y starts at 0 (no negative values), assume input is half-span
		if (y.front() >= 0.0f) {
			S *= 2.0f;
		}
	} else if (y.size() == 1 && c.size() == 1) {
		// single station: approximate area as rectangular wing
		S = c[0] * b;
	} else {
		S = 0.0f;
	}

	const float EPS = 1e-8f;
	if (S > EPS) {
		aRatio = (b * b) / S;
	} else {
		aRatio = 0.0f;
	}

	// build Ic matrix
	Ic.reserve(N);
	for (int l = 0; l < N; ++l) {
		vector<float> row;
		row.reserve(N);
		for (int m = 0; m < N; ++m) {
			// same formula as before
			float val = ((4 * b * sin(m * theta_n[l])) / (alfa0_t[l] * c_t[l])) + (m * sin(m * theta_n[l]) / sin(theta_n[l]));
			row.push_back(val);
		}
		Ic.push_back(std::move(row));
	}

	// compute inverse
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
    oss << "aRatio: " << aRatio << "\n";

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

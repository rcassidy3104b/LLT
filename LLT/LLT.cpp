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
}

void LLT::run()
{
	//
	// N is number of stations
	// M is number of fourier terms
	// N=M!
	
	//[IC](l,m) = ((4*b*sin(m*theta_l))/ alpha_0L(theta_l)*chord(theta_l)) + (m*sin(m*theta_l)/sin(theta_l)
	//[IC] {A} = {alphas} + {t(theta_n)} = {alpha_0L(theta_n)} -> {A} = [IC]^-1 * [alpha + t - alpha_0L]
}

int LLT::getN() const
{
	return N;
}

float LLT::getB() const
{
	return b;
}

bool LLT::isElip() const
{
	return elip;
}

int LLT::getStation() const
{
	return station;
}

const vector<float>& LLT::getY() const
{
	return y;
}

const vector<float>& LLT::getC() const
{
	return c;
}

const vector<float>& LLT::getA0() const
{
	return a0;
}

const vector<float>& LLT::getAlfa0() const
{
	return alfa0;
}

const vector<float>& LLT::getTwist() const
{
	return twist;
}

void LLT::setN(int value)
{
	N = value;
}

void LLT::setB(float value)
{
	b = value;
}

void LLT::setElip(bool value)
{
	elip = value;
}

void LLT::setStation(int value)
{
	station = value;
}

void LLT::setY(const vector<float>& value)
{
	y = value;
}

void LLT::setC(const vector<float>& value)
{
	c = value;
}

void LLT::setA0(const vector<float>& value)
{
	a0 = value;
}

void LLT::setAlfa0(const vector<float>& value)
{
	alfa0 = value;
}

void LLT::setTwist(const vector<float>& value)
{
	twist = value;
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

    return oss.str();
}

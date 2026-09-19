#pragma once
#define _USE_MATH_DEFINES 
#include <cmath>
#include <vector>
#include <string>

using namespace std;

class LLT
{
private:
	int N;
	float b;
	bool elip;
	int station;
	vector<float> y;
	vector<float> c;
	vector<float> a0;
	vector<float> alfa0;
	vector<float> twist;
	
	vector<float> theta_n;
	vector<float> y_t;
	vector<float> c_t;
	vector<float> a0_t;
	vector<float> alfa0_t;
	vector<float> twist_t;

	vector<vector<float>> Ic;
	// new member to hold the inverse of Ic
	vector<vector<float>> Ic_inv;

	// new member: aRatio = b*b / S  (S = wing area computed from y and c)
	float aRatio;
public:
	// default constructor
	LLT();

	// parameterized constructor (signature)
	LLT(int N, float b, bool elip, int station,
		const vector<float>& y,
		const vector<float>& c,
		const vector<float>& a0,
		const vector<float>& alfa0,
		const vector<float>& twist);

	// results now accepts a float alphaIn and returns vector A
	vector<float> results(float alphaIn);

	// new: compute and store inverse of Ic
	// returns true on success, false on failure (e.g. singular matrix)
	bool invertIc();

	// getters (inline)
	int getN() const { return N; }
	float getB() const { return b; }
	bool isElip() const { return elip; }
	int getStation() const { return station; }
	const vector<float>& getY() const { return y; }
	const vector<float>& getC() const { return c; }
	const vector<float>& getA0() const { return a0; }
	const vector<float>& getAlfa0() const { return alfa0; }
	const vector<float>& getTwist() const { return twist; }

	// accessor to aRatio
	float getARatio() const { return aRatio; }

	// optional accessor to the inverted matrix (already inline)
	const vector<vector<float>>& getIcInverse() const { return Ic_inv; }

	// setters (inline)
	void setN(int value) { N = value; }
	void setB(float value) { b = value; }
	void setElip(bool value) { elip = value; }
	void setStation(int value) { station = value; }
	void setY(const vector<float>& value) { y = value; }
	void setC(const vector<float>& value) { c = value; }
	void setA0(const vector<float>& value) { a0 = value; }
	void setAlfa0(const vector<float>& value) { alfa0 = value; }
	void setTwist(const vector<float>& value) { twist = value; }

	std::string to_str() const;
};
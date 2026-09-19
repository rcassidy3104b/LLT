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
	vector<vector<float>> Ic_inv; // new member to hold the inverse of Ic
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

	// existing method
	vector<float> results();

	// new: compute and store inverse of Ic
	// returns true on success, false on failure (e.g. singular matrix)
	bool invertIc();

	// getters
	int getN() const;
	float getB() const;
	bool isElip() const;
	int getStation() const;
	const vector<float>& getY() const;
	const vector<float>& getC() const;
	const vector<float>& getA0() const;
	const vector<float>& getAlfa0() const;
	const vector<float>& getTwist() const;

	// optional accessor to the inverted matrix
	const vector<vector<float>>& getIcInverse() const { return Ic_inv; }

	// setters
	void setN(int value);
	void setB(float value);
	void setElip(bool value);
	void setStation(int value);
	void setY(const vector<float>& value);
	void setC(const vector<float>& value);
	void setA0(const vector<float>& value);
	void setAlfa0(const vector<float>& value);
	void setTwist(const vector<float>& value);

	std::string to_str() const;
};
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
	float S;
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

	// new: uniformly spaced y locations across the span [-b/2, b/2]
	vector<float> y_locations;

	// new: chord values corresponding to y_locations (interpolated from c_t at y_t)
	vector<float> chord_y;

	vector<vector<float>> Ic;
	// new member to hold the inverse of Ic
	vector<vector<float>> Ic_inv;

	// new member: aRatio = b*b / S  (S = wing area computed from y and c)
	float aRatio;

	// centralised computation invoked by constructors and setters
	void computeAll();

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

	// constructor that parses an input CSV similar to previous main logic
	LLT(const std::string& filename);

	// results now accepts a float alphaIn and returns vector A
	vector<float> results(float alphaIn);
	vector<float> coefs(float alphaIn);

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

	// accessor for the new uniform y locations
	const vector<float>& getYLocations() const { return y_locations; }

	// accessor for the chord values at y_locations
	const vector<float>& getCLocations() const { return chord_y; }

	// optional accessor to the inverted matrix (already inline)
	const vector<vector<float>>& getIcInverse() const { return Ic_inv; }

	// getters for computed members
	float getS() const { return S; }
	const vector<float>& getThetaN() const { return theta_n; }
	const vector<float>& getYt() const { return y_t; }
	const vector<float>& getCt() const { return c_t; }
	const vector<float>& getA0t() const { return a0_t; }
	const vector<float>& getAlfa0t() const { return alfa0_t; }
	const vector<float>& getTwistT() const { return twist_t; }
	const vector<vector<float>>& getIc() const { return Ic; }

	// setters (inline) retrigger calculations after change
	void setN(int value) { N = value; computeAll(); }
	void setB(float value) { b = value; computeAll(); }
	void setElip(bool value) { elip = value; computeAll(); }
	void setStation(int value) { station = value; computeAll(); }
	void setY(const vector<float>& value) { y = value; computeAll(); }
	void setC(const vector<float>& value) { c = value; computeAll(); }
	void setA0(const vector<float>& value) { a0 = value; computeAll(); }
	void setAlfa0(const vector<float>& value) { alfa0 = value; computeAll(); }
	void setTwist(const vector<float>& value) { twist = value; computeAll(); }

	std::string to_str() const;

	// write LLT data to CSV file; returns true on success
	bool to_file(const std::string& filename) const;
};
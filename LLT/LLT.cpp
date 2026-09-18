#include "LLT.h"

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
	
}

void LLT::run()
{
	//
	// N is number of stations
	// M is number of fourier terms
	// N=M!
	// for each in N
	//      theta_n = (n*pi)/(N+1)
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

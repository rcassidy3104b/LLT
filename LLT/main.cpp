#include <iostream>
#include "LLT.h"

int main()
{
	// Construct LLT object from file (parsing moved into LLT constructor)
	LLT llt("In.csv");

	std::cout << "LLT object created: N=" << llt.getN()
	          << " b=" << llt.getB()
	          << " elip=" << llt.isElip()
	          << " station=" << llt.getStation() << '\n';

	std::cout << llt.to_str() << std::endl;

	return 0;
}//For elliptical wings in computeAll(), when c_t is calculated, instead of it being based on the vector c, it should instead be c_t[i] = B*sqrt(1-(cos(theta_n[i]))^2) where B is a scaling constant so that the calculated aspect ratio of the wing matches the current aRatio.

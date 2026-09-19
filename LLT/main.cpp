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
}
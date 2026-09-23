#include "LLT.h"
#include <iostream>
#include <string>

int main()
{
	std::cout << "Enter input filename: ";
	std::string filename;
	if (!std::getline(std::cin, filename)) {
		std::cerr << "No filename provided\n";
		return 1;
	}
	if (filename.empty()) {
		std::cerr << "Empty filename\n";
		return 1;
	}

	// Construct LLT object from file (parsing moved into LLT constructor)
	LLT llt(filename);

	std::cout << "LLT object created: N=" << llt.getN()
		<< " b=" << llt.getB()
		<< " elip=" << llt.isElip()
		<< " station=" << llt.getStation() << '\n';

	std::cout << llt.to_str() << std::endl;
	llt.to_file("out.csv");
	vector<float> res = llt.coefs(1.0);
	for (float i : res) {
		std::cout << to_string(i) << std::endl;
	}
	
	return 0;
}
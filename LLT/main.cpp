#include "LLT.h"
#include <iostream>
#include <string>
#include <fstream>

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

	//LLT llt(filename);
	LLT llt("GenWing.csv");
	/*
	std::cout << "LLT object created: N=" << llt.getN()
		<< " b=" << llt.getB()
		<< " elip=" << llt.isElip()
		<< " station=" << llt.getStation() << '\n';

	std::cout << llt.to_str() << std::endl;
	llt.to_file("out.csv");
	vector<float> res = llt.coefs(1.0);
	vector<float> A = llt.results(1.0);
	for (float i : res) {
		std::cout << to_string(i) << std::endl;
	}
	std::cout << std::endl;
	for (float i : A) {
		std::cout << to_string(i) << "    ";
	}
	std::cout << std::endl;
	*/

	/*vector<int> nTest = { 3,4,5,7,10,15,25,30,40,50,100,200 };
	std::ofstream ofs("p4a.csv");
	ofs.precision(10);
	ofs << "N,CL,CDi" << std::endl;
	for (int i : nTest) {
		llt.setN(i);
		ofs << i << "," << llt.results(1.0)[0] << "," << llt.results(1.0)[2] << std::endl;
	}*/

	/*vector<int> nTest = {3,4,5,7,10,15,25,30,40,50,100,200};
	std::ofstream ofs("p4b1.csv");
	ofs.precision(10);
	ofs << "N,A" << std::endl;
	for (int i : nTest) {
		llt.setN(i);
		ofs << i << ",";
		for (float j : llt.results(1.0)) {
			ofs << j << ",";
		}
		ofs << std::endl;
	}
	*/
	/*vector<int> nTest = {3,4,5,7,10,15,25,30,40,50,100,200};
	std::ofstream ofs("p4b2.csv");
	ofs.precision(10);
	for (int i : nTest) {
		llt.setN(i);
		ofs << "N=" << i << std::endl;
		ofs << "y,";
		for (float j : llt.getYt()) {
			ofs << j << ",";
		}
		ofs << std::endl;
		ofs << "c,";
		for (float j : llt.getCt()) {
			ofs << j << ",";
		}
		ofs << std::endl;
	}*/

	
	return 0;
}
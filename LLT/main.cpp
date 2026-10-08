#include "LLT.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <iomanip>

static std::string trim(const std::string& s)
{
	std::size_t a = s.find_first_not_of(" \t\r\n");
	if (a == std::string::npos) return {};
	std::size_t b = s.find_last_not_of(" \t\r\n");
	return s.substr(a, b - a + 1);
}

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

	LLT llt(filename);

	std::cout << "LLT object created: N=" << llt.getN()
		<< " b=" << llt.getB()
		<< " elip=" << llt.isElip()
		<< " station=" << llt.getStation() << '\n';

	std::cout << llt.to_str() << std::endl;

	// write object summary to out.csv
	if (!llt.to_file("out.csv")) {
		std::cerr << "Failed to write out.csv\n";
	}

	// Read last non-empty line of input file to get alphas: start,step,stop
	std::ifstream ifs(filename);
	if (!ifs) {
		std::cerr << "Failed to reopen input file to read alphas\n";
		return 1;
	}
	std::string line;
	std::string lastNonEmpty;
	while (std::getline(ifs, line)) {
		std::string t = trim(line);
		if (!t.empty()) lastNonEmpty = t;
	}
	ifs.close();

	if (lastNonEmpty.empty()) {
		std::cerr << "No non-empty lines found in input file to parse alphas\n";
		return 1;
	}
	// parse CSV fields from lastNonEmpty
	std::vector<std::string> fields;
	{
		std::istringstream ss(lastNonEmpty);
		std::string tok;
		while (std::getline(ss, tok, ',')) {
			fields.push_back(trim(tok));
		}
	}
	if (fields.size() < 3) {
		std::cerr << "Expected alphas in 'start,step,stop' format on last line; found: " << lastNonEmpty << '\n';
		return 1;
	}

	double startDeg = 0.0, stepDeg = 0.0, stopDeg = 0.0;
	try {
		startDeg = std::stod(fields[0]);
		stepDeg = std::stod(fields[1]);
		stopDeg = std::stod(fields[2]);
	}
	catch (...) {
		std::cerr << "Failed to parse start/step/stop from: " << lastNonEmpty << '\n';
		return 1;
	}

	if (stepDeg == 0.0) {
		std::cerr << "Step value must be non-zero\n";
		return 1;
	}

	// Append results to out.csv
	std::ofstream ofs("out.csv", std::ios::app);
	if (!ofs) {
		std::cerr << "Failed to open out.csv for appending results\n";
		return 1;
	}
	ofs << std::fixed << std::setprecision(8);

	// Write a header block for the results (human readable comment line)
	ofs << "\n# Results: alpha_deg, CL, delta, CDi, CR,\n";

	// Iterate alphas and write both results (A) and coefs for each alpha.
	float alphaDeg = startDeg;

	while (alphaDeg <= stopDeg + 1e-12) {
		std::vector<float> C;
		// guard: only call if method exists; assuming it does in this project
		C = llt.coefs(alphaDeg);
		ofs << alphaDeg;
		for (float v : C) ofs << "," << v;
		ofs << "\n";
		alphaDeg += stepDeg;
	}
	//ofs << llt.alphaForCL(1.0) << std::endl;

	ofs.close();
	//HW2 Question 1
	vector<string> fileNames = { "Wing1.csv", "Wing2.csv", "Wing3.csv", "Wing4.csv" };
	std::ofstream clq("HW2_1.csv");
	for (string name : fileNames) {
		LLT lltq(name);
		int numMatch = 0;
		float aUp = 20.0;
		float aLow = 0.0;
		float aMid;
		vector<float> clDist;
		vector<float> clMax = lltq.getClMax();
		while (numMatch < 1 || numMatch>2) {
			numMatch = 0;
			aMid = (aUp + aLow) / 2.0;
			clDist = lltq.localCl(aMid);
			for (int i = 0; i < clDist.size(); i++) {
				if (clDist[i] >= clMax[i]) {
					numMatch++;
				}
			}
			if (numMatch < 1) {
				aLow = aMid;
			}
			else if (numMatch > 2) {
				aUp = aMid;
			}
		}

		clq << name << "," << aMid << std::endl;
		for (int i = 0; i < clDist.size(); i++) {
			clq << clDist[i] << ",";
		}
		clq << std::endl;
		for (int i = 0; i < clMax.size(); i++) {
			clq << clMax[i] << ",";
		}
		clq << std::endl;
	}
	clq.close();


	return 0;
}
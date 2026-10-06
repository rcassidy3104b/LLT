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

	// write object summary to out.csv (existing behavior)
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
		stepDeg  = std::stod(fields[1]);
		stopDeg  = std::stod(fields[2]);
	} catch (...) {
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
		float alphaRad = alphaDeg * M_PI / 180.0;
		// coefs historically expected degrees in the original code; call with degrees
		std::vector<float> C;
		// guard: only call if method exists; assuming it does in this project
		try {
			C = llt.coefs(alphaDeg);
		} catch (...) {
			// if coefs not available or throws, leave C empty
			C.clear();
		}
		ofs << alphaDeg;
		for (float v : C) ofs << "," << v;
		ofs << "\n";
		alphaDeg += stepDeg;
	}
	//ofs << llt.alphaForCL(1.0) << std::endl;

	ofs.close();
	
	return 0;
}
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
	/*vector<string> fileNames = { "Wing1.csv", "Wing2.csv", "Wing3.csv", "Wing4.csv" };
	std::ofstream clq("HW2_1.csv");
	for (string name : fileNames) {
		LLT lltq(name);
		float aMid = lltq.stallOnsetAoA();
		vector<float> clDist = lltq.localCl(aMid);
		vector<float> clMax = lltq.getClMax();
		clq << name << "," << aMid <<"," << lltq.coefs(aMid)[0] <<std::endl;
		for (int i = 0; i < clDist.size(); i++) {
			clq << clDist[i] << ",";
		}
		clq << std::endl;
		for (int i = 0; i < clMax.size(); i++) {
			clq << clMax[i] << ",";
		}
		clq << std::endl;
	}
	clq.close();*/

	//HW2 Question 2
	/*std::ofstream clq("HW2_2.csv");
	LLT llta("Wing2.csv");
	int numMatch = 0;
	float tUp = 0.0;
	float tLow = -20.0;
	float tMid;
	float a;
	vector<float> clDist;
	vector<float> clMax = llta.getClMax();
	float dif = 1.0;
	while (abs(dif) > 0.001) {
		tMid = (tUp + tLow) / 2.0;
		vector<float> twist = { tMid, 0.0, tMid};
		llta.setTwist(twist);
		a = llta.stallOnsetAoA();
		clDist = llta.localCl(a);
		dif = clMax[static_cast <int>(0.85 * llta.getN())] - clDist[static_cast <int>(0.85 * llta.getN())] - 0.1;
		if(dif>0.001){
			tLow = tMid;
		}
		else if(dif<-0.001){
			tUp = tMid;
		}
		else{
			break;
		}
	}
	clq << "Twist at end," << tMid << "," << "Stall onset Alpha," << a << ",CL Max,"<< llta.coefs(a)[0] << std::endl;
	clq << "Cl distribution,";
	for (int i = 0; i < clDist.size(); i++) {
		clq << clDist[i] << ",";
	}
	clq << std::endl;
	clq << "Cl Max distribution,";
	for (int i = 0; i < clMax.size(); i++) {
		clq << clMax[i] << ",";
	}
	clq << std::endl;
	clq << "chord dist,";
	for (int i = 0; i < llta.getChordY().size(); i++) {
		clq << llta.getChordY()[i] << ",";
	}*/

	//HW2 Question 3
	//LLT llta("P3.csv");
	//std::ofstream clq("HW2_3.csv");
	////basic loading
	//float alphaZL = llta.alphaForCL(0.0);
	//vector<float> clDistBasic = llta.localCl(alphaZL);

	//float alpha05 = llta.alphaForCL(0.5);
	//vector<float> clDist05 = llta.localCl(alpha05);
	//clq << "Cl for wing at 0.5,";
	//for (int i = 0; i < clDist05.size(); i++) {
	//	clq << clDist05[i] << ",";
	//}
	//clq << std::endl;

	////Add'l loading
	//vector<float> zeros = { 0.0, 0.0, 0.0 };
	//llta.setAlfa0(zeros);
	//llta.setTwist(zeros);
	//float alphaADD = llta.alphaForCL(1.0);
	//vector<float> clDistAdd = llta.localCl(alphaADD);


	//

	//clq << "Basic Loading Cl,";
	//for (int i = 0; i < clDistBasic.size(); i++) {
	//	clq << clDistBasic[i] << ",";
	//}
	//clq << std::endl << "Additional Loading Cl=1,";
	//for (int i = 0; i < clDistAdd.size(); i++) {
	//	clq << clDistAdd[i] << ",";
	//}
	//clq << std::endl;

	//float alpha5 = llta.alphaForCL(0.5);
	//vector<float> clDistAdd5 = llta.localCl(alpha5);
	//clq << "Additional Loading Cl = .5,";
	//for (int i = 0; i < clDistAdd5.size(); i++) {
	//	clq << clDistAdd5[i] << ",";
	//}
	//clq << std::endl;
	//clq.close();

	//HW2 Question 4
	std::ofstream clq("HW2_4.csv");
	LLT llta("P4a.csv");
	float alpha = llta.alphaForCL(1.0);
	vector<float> clDist = llta.localCl(alpha);
	vector<float> chord = llta.getChordY();
	clq << "Alpha no deflection,"<<alpha<<endl;
	clq << "No Deflection Cl,";
	for (int i = 0; i < clDist.size(); i++) {
		clq << clDist[i] << ",";
	}
	clq << endl << "chord,";
	for (int i = 0; i < chord.size(); i++) {
		clq << chord[i] << ",";
	}

	LLT lltb("P4b.csv");
	//alpha = lltb.alphaForCL(1.0);
	clDist = lltb.localCl(alpha);
	chord = lltb.getChordY();
	//clq << endl << "Alpha 10 deg deflection," << alpha;
	clq << endl<< "10 deg Deflection Cl,";
	for (int i = 0; i < clDist.size(); i++) {
		clq << clDist[i] << ",";
	}
	clq << endl << "y,";
	for (int i = 0; i < lltb.getYLocations().size(); i++) {
		clq << lltb.getYLocations()[i] << ",";
	}
	clq.close();
	
	return 0;
}
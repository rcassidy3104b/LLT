#include "LLT.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

static std::string trim(const std::string& s)// trims a string of whitespace from both ends
{
	std::size_t a = s.find_first_not_of(" \t\r\n");
	if (a == std::string::npos) return {};
	std::size_t b = s.find_last_not_of(" \t\r\n");
	return s.substr(a, b - a + 1);
}

static std::string firstField(const std::string& line)// extracts the first field from a comma-separated line
{
	std::istringstream ss(line);
	std::string tok;
	if (!std::getline(ss, tok, ',')) return {};
	return trim(tok);
}

int main()
{
	std::ifstream inFile("In.csv");// Open the input CSV file
	if (!inFile) {
		std::cerr << "Failed to open In.csv\n";
		return 1;
	}

	std::string line;

	// Read header values: b, N, elip, station (each on its own row, first column)
	if (!std::getline(inFile, line)) { std::cerr << "Missing b\n"; return 1; }
	float b = 0.0f;
	try { b = std::stof(firstField(line)); } catch (...) {}

	if (!std::getline(inFile, line)) { std::cerr << "Missing N\n"; return 1; }
	int N = 0;
	try { N = std::stoi(firstField(line)); } catch (...) {}

	if (!std::getline(inFile, line)) { std::cerr << "Missing elip\n"; return 1; }
	bool elip = false;
	try { elip = (std::stoi(firstField(line)) != 0); } catch (...) {}

	if (!std::getline(inFile, line)) { std::cerr << "Missing station\n"; return 1; }
	int station = 0;
	try { station = std::stoi(firstField(line)); } catch (...) {}

	// Prepare vectors for station rows
	std::vector<float> y; y.reserve(std::max(0, station));
	std::vector<float> c; c.reserve(std::max(0, station));
	std::vector<float> a0; a0.reserve(std::max(0, station));
	std::vector<float> alfa0; alfa0.reserve(std::max(0, station));
	std::vector<float> twist; twist.reserve(std::max(0, station));

	// Read station rows (each row has: y,c,a0,alfa0,twist)
	for (int i = 0; i < station; ++i) {
		if (!std::getline(inFile, line)) {
			// If rows missing, fill remaining with zeros
			y.push_back(0.0f);
			c.push_back(0.0f);
			a0.push_back(0.0f);
			alfa0.push_back(0.0f);
			twist.push_back(0.0f);
			continue;
		}

		std::istringstream ss(line);
		std::string tok;

		auto parseTok = [&ss]() -> float {
			std::string t;
			if (!std::getline(ss, t, ',')) return 0.0f;
			t = trim(t);
			if (t.empty()) return 0.0f;
			try { return std::stof(t); } catch (...) { return 0.0f; }
		};

		y.push_back(parseTok());
		c.push_back(parseTok());
		a0.push_back(parseTok());
		alfa0.push_back(parseTok());
		twist.push_back(parseTok());
	}

	// Construct LLT object (note: LLT constructor signature is LLT(int N, float b, bool elip, int station, ...)
	LLT llt(N, b, elip, station, y, c, a0, alfa0, twist);

	// Optional: indicate success
	std::cout << "LLT object created: N=" << llt.getN()
	          << " b=" << llt.getB()
	          << " elip=" << llt.isElip()
	          << " station=" << llt.getStation() << '\n';
	cout << llt.to_str() << endl;

	return 0;
}
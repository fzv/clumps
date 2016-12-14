#include <iostream>
/*
* cout
* endl
*/
#include <string>
/*
* string
*/
#include <vector>
#include <fstream>
/*
* ifstream
*/
#include <sstream>
/*
* stringstream
*/
#include <iterator>
#include "sdsl/suffix_arrays.hpp"
#include "sdsl/lcp.hpp"

int main(int argc, char* argv[])
{

std::cout << "Welcome to CLUMP" << std::endl;

// parse input

std::string textfile = argv[1];
std::ifstream tFile(textfile);
std::string tline;
std::stringstream T;
if (tFile.is_open()){
	if (tFile.good()){
		while (getline(tFile, tline)){
			if (tline[0] != '>') T << tline;
		}
	}
	tFile.close();
} else {
	std::cout << "Unable to open file." << std::endl;
}
std::string t = T.str();
std::cout << t << std::endl;



return 0;

} //END_MAIN

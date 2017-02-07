#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iterator>
#include <ctime>
#include "sdsl/suffix_arrays.hpp"
#include "sdsl/lcp.hpp"
#include "clumps.h"

int main()
{

/* begin logging */
std::ofstream logfile; 
logfile.open("clumps.log");
logfile << "LOG" << std::endl;

//read clumps.in
std::ifstream clumpsFile("clumps.in");
std::string line;

/* parse text */
std::string textfile = "paper.fas"; //TODO
std::ifstream tFile(textfile);
std::string tline;
std::stringstream tstream;
if (tFile.is_open())
{
	if (tFile.good())
	{
		while (getline(tFile, tline))
		{
			if (tline[0] != '>') tstream << tline;
		}
	}
	tFile.close();
}
else
{
	logfile << "Unable to open file." << std::endl;
}
std::string t = tstream.str();
int n = t.length();

/* TESTING ONLY: print text */
logfile << std::endl << "printing text" << std::endl;
logfile << t << std::endl;
logfile << std::endl << "n = " << n << std::endl;

/* parse input params */
//TODO
int l = 7;
int m = 3;
int ll = l-m+1;
int d = 1;
int k = 2;

/* TESTING ONLY: print some params */
logfile << "l'= " << ll << std::endl;
logfile << "l/2= " << ceil(ll/2) << std::endl;

//run clumps with each batch + add i's to vector
clumps(logfile,t,n,m,ll,d,k);

//reports i's if occur in all vectors

return 0;
}

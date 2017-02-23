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
#include <map>

int main()
{

/* begin logging */
std::ofstream logfile; 
logfile.open("clumps.log");
logfile << "LOG" << std::endl;
std::cout << "CLUMPS" << std::endl;

//read clumps.in
std::ifstream clumpsFile("clumps.in");
std::vector<std::vector<std::string>> lines;
std::string line;
if (clumpsFile.is_open())
{
	if (clumpsFile.good())
	{
		while (getline(clumpsFile, line, '\n'))
		{
			std::vector<std::string> linevec;
			std::stringstream ss(line);
			std::string item;
			while( getline(ss, item, '\t') )
			{
				linevec.push_back(item);
			}
			lines.push_back(linevec);
		}
	}
	clumpsFile.close();
}
else
{
	logfile << "Unable to open clumps.in" << std::endl;
}

/* declare params */
std::string textfile;
std::vector<int> L;
std::vector<int> M;
std::vector<int> D;
std::vector<int> K;
int runs = 0;

for (int item = 0; item != lines.size(); item++)
{
	if (lines[item][0] == "t")
	{
		textfile = lines[item][2];
	}
	else if (lines[item][0] == "#")
	{
		runs++;
	}
	else if (lines[item][0] == "l")
	{
		L.push_back( std::stoi( lines[item][2] ) );
	}
	else if (lines[item][0] == "m")
	{
		M.push_back( std::stoi( lines[item][2] ) );
	}
	else if (lines[item][0] == "d")
	{
		D.push_back( std::stoi( lines[item][2] ) );
	}
	else if (lines[item][0] == "k")
	{
		K.push_back( std::stoi( lines[item][2] ) );
	}
}

/* parse text */
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
	logfile << "Unable to open text file." << std::endl;
}
std::string t = tstream.str();
int n = t.length();

/* TESTING ONLY: print text */
/*
logfile << std::endl << "printing text" << std::endl;
logfile << t << std::endl;
logfile << std::endl << "n = " << n << std::endl;
*/

std::vector<std::vector<std::pair<int,std::string>>> results;
for (int run = 0; run != runs; run++)
{
	//std::cout << "run # " << run << std::endl;
	logfile << "run # " << run << std::endl;
	std::vector<std::pair<int,std::string>> result;
	int l = L[run];
	int m = M[run];
	int ll = l-m+1;
	int d = D[run];
	int k = K[run];

	/* TESTING ONLY: print some params */
	logfile << "l'= " << ll << std::endl;
	logfile << "l/2= " << ceil(ll/2) << std::endl;
	logfile << "m= " << m << std::endl;
	logfile << "d= " << d << std::endl;
	logfile << "k= " << k << std::endl;

	//run clumps with each batch + add i's to vector
	clumps(logfile,t,n,m,ll,d,k,result);
	results.push_back(result);
}



//reports i's if occur in all vectors
std::map<int,int> report;
//std::cout << "run 0" << std::endl;
for(int item = 0; item != results[0].size(); item++)
{
	//std::cout << "i = " << results[0][item].first << std::endl;
	//std::cout << "p = " << results[0][item].second << std::endl;
	report[ results[0][item].first ] = 1;
}

for(int run = 1; run != runs; run++)
{
//std::cout << "run " << run << std::endl;
	for(int item = 0; item != results[run].size(); item++)
	{
		//std::cout << "i = " << results[run][item].first << std::endl;
		//std::cout << "p = " << results[run][item].second << std::endl;
		if( report.count( results[run][item].first ) > 0)
		{
			report[results[run][item].first]++;
		}
		else
		{
			report[results[run][item].first] = 1;
		}
	}
}

//print map

std::map<int,int>::iterator pos;
/*
for (pos = report.begin(); pos != report.end(); ++pos) {
	std::cout << "i: \"" << pos->first << "\" "
		<< "times reported: " << pos->second << std::endl;
}
*/

//report if counter == runs

std::cout << std::endl << "REPORTING CLUMPS" << std::endl;

for (pos = report.begin(); pos != report.end(); ++pos) {
	if (pos->second == runs)
	{
		std::cout << "reporting i = " << pos->first << std::endl;
		std::cout << "with patterns" << std::endl;
		for(int run = 0; run != runs; run++)
		{
			for(int item = 0; item != results[run].size(); item++)
			{
				if(results[run][item].first == pos->first) std::cout << results[run][item].second << std::endl;
			}
		}
	}
}

/* end logging */
logfile.close();
return 0;
}

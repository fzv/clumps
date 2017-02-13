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

std::vector<std::vector<int>> getPValues
/*
* function: 
* time complexity: 
* space complexity:
*/
(
  int& r
, std::vector<std::vector<std::pair<int,std::pair<int,int>>>>& M
)
{
std::vector<std::vector<int>> pvalues;
for (int i = 0; i <= r; i++)
{
	std::vector<int> v;
	if ( ! M[i].empty() )
	{
		v.push_back(M[i][0].second.first);
		if ( M[i].size() > 1 )
		{
			for (std::vector<std::pair<int,std::pair<int,int>>>::iterator iter = M[i].begin() + 1;
				iter != M[i].end();
				iter++)
			{
				if ( (*iter).second.first != (*(iter-1)).second.first )
				{
					v.push_back((*iter).second.first);
				}
			}
		}
	} else {
		v.push_back(-1);
	}
	pvalues.push_back(v);
}
return pvalues;
}

std::string getPattern
/*
* function: given a position tpos in T', and (product of) prime(s) p,
		return degenerate pattern formed by rank at T'[tpos]
		and ranks with same p and parikh[rank] > 0
* time complexity: linear in size of M[i]
* space complexity:
*/
( //PARAMS
  int& i //position in T'
, int& p
, std::vector<int>& tt
, std::vector<std::pair<int,std::pair<int,int>>>& Mi
, std::string& t
, int& m
, std::vector<int>& parikh
, std::ofstream& logfile
) //END_PARAMS
{ //FUNCTION
//FOR TESTING ONLY
logfile << "--------------------------------------------------------------" << std::endl;
logfile << "p = " << p << std::endl;
logfile << "i s.t. tt[i] is current pos = " << i << std::endl;
logfile << "current letter in T' is " << tt[i] << std::endl;

std::string pattern;
std::vector<int> ranks;
ranks.push_back(tt[i]);
for (std::vector<std::pair<int,std::pair<int,int>>>::iterator it = Mi.begin(); it != Mi.end(); it++)
{
	if ( ((*it).second.first == p) && (parikh[(*it).first] != 0) ) ranks.push_back( (*it).first );
}
std::stringstream patterns;
for (std::vector<int>::iterator it = ranks.begin(); it != ranks.end(); it++)
{
	logfile << "rank is " << (*it) << std::endl;
	std::vector<int>::iterator iter;
	iter = std::find( tt.begin() , tt.end() , (*it) );
	int ttpos = std::distance(tt.begin(),iter);
	logfile << "m-gram is " << t.substr(ttpos,m) << std::endl;
	patterns << t.substr(ttpos,m) << ",";
}
pattern = patterns.str();
pattern.pop_back();
logfile << "--------------------------------------------------------------" << std::endl;
return pattern;
} //END_FUNCTION(getPattern)

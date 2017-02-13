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

void constructM
/*
* function: 
* time complexity: 
* space complexity:
*/
(
std::vector<std::vector<std::pair<int,std::pair<int,int>>>>& M
, int& r
, std::vector<int>& tt
, sdsl::csa_bitcompressed<>& sa
, sdsl::lcp_bitcompressed<>& lcp
, int& d
, std::vector<int>& E
, int& m
, std::ofstream& logfile
)
{
for (int i = 0; i <= r; i++)
{
	std::vector<std::pair<int,std::pair<int,int>>> vec;
	M.push_back(vec);
}
for (int i = 0; i <= r; i++){
	for (int j = 0; j <= r; j++){
		if (i != j){
		int p = 1;
		//which suffix does rank i/j represent?
		std::vector<int>::iterator iter_suffi = std::find(tt.begin(), tt.end(), i);
		std::vector<int>::iterator iter_suffj = std::find(tt.begin(), tt.end(), j);
		int suffi = std::distance(tt.begin(),iter_suffi);
		int suffj = std::distance(tt.begin(),iter_suffj);
		//where is suffix i/j in the suffix array?
		sdsl::csa_bitcompressed<>::iterator iter_sai = std::find(sa.begin(), sa.end(), suffi);
		sdsl::csa_bitcompressed<>::iterator iter_saj = std::find(sa.begin(), sa.end(), suffj);
		int sai = std::distance(sa.begin(),iter_sai);
		int saj = std::distance(sa.begin(),iter_saj);
		int match = rmq(&lcp, &sai, &saj, &logfile);
		int pos = match;
		int e = 1;
		p = p * E[pos];
		while ( (pos < m) && (e <= d) ){
			suffi = suffi + match + 1;
			suffj = suffj + match + 1;
			iter_sai = std::find(sa.begin(), sa.end(), suffi);
			iter_saj = std::find(sa.begin(), sa.end(), suffj);
			sai = std::distance(sa.begin(),iter_sai);
			saj = std::distance(sa.begin(),iter_saj);
			match = rmq(&lcp, &sai, &saj, &logfile);
			pos = match + pos + 1;
			if (pos < m){
				e++;
				p = p * E[pos];
			} //END_IF(pos<m)
		} //END_WHILE
		if (e <= d){
			std::pair<int,int> pe = std::make_pair(p,e);
			std::pair<int,std::pair<int,int>> jpe = std::make_pair(j,pe);
			M[i].push_back(jpe);
		} //END_IF(e <= d)
		} //END_IF(i != j)
	}
}
} //END_FUNCTION


int rmq
/*
* function: naive range minimum query TODO implement non naive
* time complexity: linear
* space complexity: n/a
*/
( //PARAMS
sdsl::lcp_bitcompressed<> *lcp
, int *x
, int *y
, std::ofstream *logfile
) //END_PARAMS
{ //FUNCTION
int i;
int j;
if ( (*x) < (*y) ){
	i = (*x);
	j = (*y);
} else {
	i = (*y);
	j = (*x);
}
int min = (*lcp)[(i+1)];
if ( j != (i+1) ){
	for (sdsl::lcp_bitcompressed<>::iterator iter = (*lcp).begin() + i + 1; iter <= (*lcp).begin() + j; iter++){
		if ( (*iter) < min ) min = (*iter);
	} //END_FOR
} //END_IF
return min;
} //END_FUNCTION(rmq)

void sortM
/*
* function: 
* time complexity: linear
* space complexity:
*/
(
std::vector<std::vector<std::pair<int,std::pair<int,int>>>>& M
)
{
for (std::vector<std::vector<std::pair<int,std::pair<int,int>>>>::iterator iter = M.begin(); iter != M.end(); iter++)
{
	std::sort( (*iter).begin() , (*iter).end() , [ ]( const std::pair<int,std::pair<int,int>> left, std::pair<int,std::pair<int,int>> right )
	{
		return ( left.second.first < right.second.first );
	}
	);
}
} //END_FUNCTION

std::vector<int> pvalues
/*
* function: 
* time complexity: 
* space complexity:
*/
( //PARAMS
  std::vector<std::vector<std::pair<int,std::pair<int,int>>>> *M
, int *i // = t'[i]
, std::ofstream *logfile
) //END_PARAMS
{ //FUCNTION
std::vector<int> unique;
unique.push_back( (*M)[(*i)][0].second.second );
if ( (*M)[(*i)].size() > 1 )
{
	for (std::vector<std::pair<int,std::pair<int,int>>>::iterator iter = (*M)[(*i)].begin() + 1; iter != (*M)[(*i)].end(); iter++)
	{
		if ( (*iter).second.second != (*(iter-1)).second.second ) unique.push_back( (*iter).second.second );
	}
}

/////(*logfile) << "printing unique vector iside function" << std::endl;
/////for (int i=0; i<unique.size(); i++) (*logfile) << unique[i] << std::endl;
return unique;
} //END_FUNCTION{pvalues}


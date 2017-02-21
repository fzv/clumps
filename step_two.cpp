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

int rmq(std::vector<int>& table, sdsl::lcp_bitcompressed<>& lcp, int& n, int& i, int& j) {
std::cout << "inside rmq function" << std::endl;
  int lgn = flog2(n);
  if (i > j) {int tmp = j; j = i; i = tmp;}
  i++; //for LCP
std::cout << "checking range [" << i << "," << j << "]" << std::endl;
  if (i == j) return i;
  int k = flog2(j-i+1);
  int a = table[i*lgn + k];
std::cout << "a = " << a << std::endl;
  int b = table[(j - (1<<k) + 1)*lgn + k];
std::cout << "b = " << b << std::endl;
std::cout << "exit rmq function" << std::endl;
  return lcp[a]>lcp[b]?lcp[b]:lcp[a];
}

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
, std::vector<int>& table
, int& n
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
		/////////////////////////////////////////////////int match = rmq(&lcp, &sai, &saj, &logfile);
		std::cout << "i = " << sai << std::endl;
		std::cout << "j = " << saj << std::endl;
		std::cout << "NOW CALLING RMQ FUNCTION before loop" << std::endl; 
		int match = rmq(table, lcp, n, sai, saj);
		std::cout << "lcp = " << match << std::endl << std::endl;
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
			////////////////////////////////////////////////////match = rmq(&lcp, &sai, &saj, &logfile);
			std::cout << "i = " << sai << std::endl;
			std::cout << "j = " << saj << std::endl;
			std::cout << "NOW CALLING RMQ FUNCTION inside loop" << std::endl; 
			match = rmq(table, lcp, n, sai, saj);
			std::cout << "lcp = " << match << std::endl << std::endl;
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




///////////////////////////////////////////////////////int rmq
/*
* function: naive range minimum query TODO implement non naive
* time complexity: linear
* space complexity: n/a
*/
/*
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
*/

//solon's code TODO
int flog2(int v) {
  union { unsigned int u[2]; double d; } t;
  t.u[1] = 0x43300000;
  t.u[0] = v;
  t.d -= 4503599627370496.0;
  return (t.u[1] >> 20) - 0x3FF; //Decimal: 1023  Binary: 1111111111  Octal: 1777  Hexadecimal: 0x3FF
}

void rmq_preprocess(std::vector<int>& table, sdsl::lcp_bitcompressed<>& lcp, int n) {
  int i, j;
  int lgn = flog2(n);
  std::cout << "\n\npre=processing rmq table" << std::endl;
  std::cout << "n = " << n << std::endl;
  std::cout << "log(n) = " << lgn << std::endl;
  // initialize $table$ for the intervals with length $1$
  for (i = 0; i < n; i++)
  {
    table[i*lgn] = i;
    std::cout << "setting table[" << i*lgn << "] to " << i << std::endl;
    std::cout << "pirnitng table" << std::endl;
    for (int i = 0; i != table.size(); i++) std::cout << table[i] << " ";
    std::cout << std::endl;
  }
  // compute values from smaller to bigger intervals
  for (j = 1; 1 << j <= n; j++)
  {
    for (i = 0; i + (1 << j) - 1 < n; i++)
    {
      std::cout << "i = " << i << " j = " << j << std::endl;
      if (lcp[table[i*lgn + j - 1]] < lcp[table[(i + (1 << (j - 1)))*lgn + j - 1]])
      {
	std::cout << lcp[table[i*lgn + j - 1]] << " is less than " << lcp[table[(i + (1 << (j - 1)))*lgn + j - 1]] << std::endl;
        table[i*lgn + j] = table[i*lgn + j - 1];
	std::cout << "setting table[" << i*lgn + j << "] to " << table[i*lgn + j - 1] << std::endl;
        std::cout << "pirnitng table" << std::endl;
        for (int i = 0; i != table.size(); i++) std::cout << table[i] << " ";
        std::cout << std::endl;
      }
      else
      {
	std::cout << lcp[table[i*lgn + j - 1]] << " is more than/equal to " << lcp[table[(i + (1 << (j - 1)))*lgn + j - 1]] << std::endl;
        table[i*lgn + j] = table[(i + (1 << (j - 1)))*lgn + j - 1];
	std::cout << "setting table[" << i*lgn + j << "] to " << table[(i + (1 << (j - 1)))*lgn + j - 1] << std::endl;
        std::cout << "pirnitng table" << std::endl;
        for (int i = 0; i != table.size(); i++) std::cout << table[i] << " ";
        std::cout << std::endl;
      }
    }
  }
  std::cout << "pre-processing done\n\n" << std::endl;
}

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


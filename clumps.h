#ifndef CLUMPS_H
#define CLUMPS_H
/**********************
* HEADERS & INCLUDES
***********************/
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
#include <algorithm>
/*
* find
*/
#include <fstream>
/*
* ifstream
*/
#include <sstream>
/*
* stringstream
*/
#include <iterator>
/*
* 
*/
#include "sdsl/suffix_arrays.hpp"
/*
* 
*/
#include "sdsl/lcp.hpp"
/*
* 
*/
#include <ctime>
/*
* 
*/
#include <map>
/*
* 
*/

/**********************
* FUNCTION DECLARATIONS
***********************/
void clumps
(
std::ofstream& logfile, std::string& t,int& n, int& m, int& ll, int& d, int& k, std::vector<std::pair<int,std::string>>& result
);
int rmq
(
sdsl::lcp_bitcompressed<> *lcp, int *x, int *y, std::ofstream *logfile);
std::vector<int> pvalues(std::vector<std::vector<std::pair<int,std::pair<int,int>>>> *M,
int *i, std::ofstream *logfile
);
std::string getPattern
(
int& i, int& p, std::vector<int>& tt, std::vector<std::pair<int,std::pair<int,int>>>& Mi
, std::string& t, int& m, std::vector<int>& parikh ,std::ofstream& logfile
);
void constructT
(
std::vector<int>& tt , int& r , sdsl::csa_bitcompressed<>& sa, int& n , int& m ,
sdsl::lcp_bitcompressed<>& lcp , int& ll
);
void constructM
(
std::vector<std::vector<std::pair<int,std::pair<int,int>>>>& M , int& r ,
std::vector<int>& tt , sdsl::csa_bitcompressed<>& sa , sdsl::lcp_bitcompressed<>& lcp , int& d ,
std::vector<int>& E, int& m , std::vector<int>& table , int& n , std::ofstream& logfile
);
void sortM(
std::vector<std::vector<std::pair<int,std::pair<int,int>>>>& M
);
std::vector<std::vector<int>> getPValues
(
int& r , std::vector<std::vector<std::pair<int,std::pair<int,int>>>>& M
);

///

int rmq(std::vector<int>& table, sdsl::lcp_bitcompressed<>& lcp, int& n, int& i, int& j);
int flog2(int v);
void rmq_preprocess(std::vector<int>& table, sdsl::lcp_bitcompressed<>& lcp, int n);

#endif


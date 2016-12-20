//usage
// k1457070@nmscde000493:~/clumps$ ./clumps paper.fas -l 6 -m 3 -d 1
/******************************* HEADERS + INCLUDES */
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
#include "sdsl/suffix_arrays.hpp"
#include "sdsl/lcp.hpp"

/******************************* FUNCTION DECLARATIONS */
int rmq(sdsl::lcp_bitcompressed<> *lcp, int *x, int *y);

/******************************* MAIN FUNCTION */

int main(int argc, char* argv[])
{

std::cout << "Welcome to CLUMP" << std::endl;

/* parse input */

std::string textfile = argv[1];
int l = atoi(argv[3]);
int m = atoi(argv[5]);
int d = atoi(argv[7]);
//TODO: if d == 0 ????
std::ifstream tFile(textfile);
std::string tline;
std::stringstream tstream;
if (tFile.is_open()){
	if (tFile.good()){
		while (getline(tFile, tline)){
			if (tline[0] != '>') tstream << tline;
		}
	}
	tFile.close();
} else {
	std::cout << "Unable to open file." << std::endl;
}
std::string t = tstream.str();
int n = t.length();
std::cout << "printing text" << std::endl;
std::cout << t << std::endl;
std::cout << "n = " << n << std::endl;

/* construct suffix array of t */

sdsl::csa_bitcompressed<> sa;
construct_im(sa, t, 1);
std::cout << "printing suffix array" << std::endl;
for (sdsl::csa_bitcompressed<>::iterator iter = sa.begin(); iter != sa.end(); iter++) std::cout << (*iter) << " "; std::cout << std::endl;

/* constuct longest common prefix array of t */

sdsl::lcp_bitcompressed<> lcp;
construct_im(lcp, t, 1);
std::cout << "printing lcp array" << std::endl;
for (sdsl::lcp_bitcompressed<>::iterator iter = lcp.begin(); iter != lcp.end(); iter++) std::cout << (*iter) << " "; std::cout << std::endl;

/* construct t' */

/**********************************************************************************************
std::vector<int> tt(sa.size(), -1);
for (int i=0; i<tt.size(); i++) std::cout << tt[i] << " "; std::cout << std::endl;
int r = -1;
int i = 0;
do{
	if ( sa[i] < (n-m) ){
		int j = i;
		while ( ( sa[j+1] < (n-m) ) && ( lcp[j+1] >= m ) ){
			j++;
		}
		if ( j-i < l/2){
			for (int k=i; k<=j; k++){
				tt[sa[k]] = r+1;
			}
			r++;
		}
		i = j+1;
	} else {
		i++;
	}
} while (i != n);
for (int i=0; i<tt.size(); i++) std::cout << tt[i] << " "; std::cout << std::endl;
*******************************************************************************************/
std::vector<int> tt(n-m+1, -1);
int r = -1;
for (int i=1; i<n+1; i++){
	if (sa[i] < n-m+1){
		if (lcp[i] >= m){
			tt[sa[i]] = r;
		} else {
			r++;
			tt[sa[i]] = r;
		}
	}
}
std::cout << "printing T'" << std::endl;
for (int i=0; i<tt.size(); i++) std::cout << tt[i] << " "; std::cout << std::endl;
std::cout << "highest rank is " << r << std::endl;

/* construct array E */
std::vector<int> PRIME_NUMBERS = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};
std::vector<int>::const_iterator first = PRIME_NUMBERS.begin();
std::vector<int>::const_iterator last = PRIME_NUMBERS.begin() + m;
std::vector<int> E(first,last);
std::cout << "printing array E" << std::endl;
for (int i=0; i<E.size(); i++) std::cout << E[i] << " "; std::cout << std::endl;

/* construction of array M */

std::vector<std::vector<std::pair<int,int>>> M;
for (int i = 0; i != r; i++){
	std::vector<std::pair<int,int>> vec;
	M.push_back(vec);
}








for (int i=0; i!=r+1; i++){
	for (int j=0; j!=r+1; j++){
		if (i < j){
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
		std::cout << "\n\nrank " << i << " represents suffix " << suffi << std::endl; 
		std::cout << "which is in pos " << sai << " of suffix array" << std::endl;
		std::cout << "rank " << j << " represents suffix " << suffj << std::endl; 
		std::cout << "which is in pos " << saj << " of suffix array" << std::endl;
		//
		int match = rmq(&lcp, &sai, &saj);
		std::cout << "match=rmq=lcp= " << match << std::endl;
		int pos = match;
		std::cout << "pos where mismatch " << pos << std::endl;
		int e = 1;
		int boundary = m - (pos+1);
		std::cout << "boundary = " << boundary << std::endl;
		while ( (pos < boundary) && (e <= d) ){
			std::cout << "INSIDE WHILE LOOP" << std::endl;
			p = p * E[pos];
			std::cout << "prime number = " << p << std::endl;
			suffi = suffi + match + 1;
			suffj = suffj + match + 1;
			iter_sai = std::find(sa.begin(), sa.end(), suffi);
			iter_saj = std::find(sa.begin(), sa.end(), suffj);
			sai = std::distance(sa.begin(),iter_sai);
			saj = std::distance(sa.begin(),iter_saj);

			std::cout << "suffix " << suffi << std::endl; 
			std::cout << "which is in pos " << sai << " of suffix array" << std::endl;
			std::cout << "suffix " << suffj << std::endl; 
			std::cout << "which is in pos " << saj << " of suffix array" << std::endl;

			match = rmq(&lcp, &sai, &saj);
			std::cout << "match=rmq=lcp= " << match << std::endl;
			pos = match + pos + 1;
			std::cout << "pos where mismatch " << pos << std::endl;
			boundary = m - (pos+1);
			std::cout << "boundary = " << boundary << std::endl;
			if (pos < boundary) e++;
			//TODO Update p again???
		} //END_WHILE
		if (e <= d){
		std::cout << "INSERTING IN ARRAY M" << std::endl;
		std::pair <int,int> jp = std::make_pair(j,p);
		M[i].push_back(jp);
		}
		} //END_IF(i<j)
	}
}

//print M
/*
for (std::vector<std::vector<std::pair<int,int>>>::iterator it = M.begin(); it != M.end(); it++){
	if ( (*it).size() != 0 ){
	for (std::vector<std::pair<int,int>>::iterator ti = (*it).begin(); ti != (*it).end(); ti++){
		
	}
	}
}
*/
for (int i = 0; i != r; i++){
	if (M[i].size() != 0){
		for (int x = 0; x != M[i].size(); x++){
			std::cout << "i = " << i << " ---" << " j = " << M[i][x].first << " ---" << " p = " << M[i][x].second << std::endl;
		}
	}
}




return 0;

} //END_MAIN

/******************************* FUNCTION DEFINITIONS */
int rmq
/*
* function: naive range minimum query
* time complexity: linear
* space complexity: n/a
*/
( //PARAMS
sdsl::lcp_bitcompressed<> *lcp
, int *x
, int *y
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
if (j != i+1){
for (sdsl::lcp_bitcompressed<>::iterator iter = (*lcp).begin() + i + 2; iter != (*lcp).begin() + j; iter++){
	if ( (*iter) < min ) min = (*iter);
}
}
return min;
} //END_FUNCTION(rmq)

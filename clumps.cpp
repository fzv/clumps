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
int get_lcp(sdsl::lcp_bitcompressed<> *lcp, int *x, int *y);

/******************************* MAIN FUNCTION */

int main(int argc, char* argv[])
{

std::cout << "Welcome to CLUMP" << std::endl;

/* parse input */

std::string textfile = argv[1];
int l = atoi(argv[3]);
//int l = 6;
int m = atoi(argv[5]);
//int m = 3;
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
std::cout << t << std::endl;

/* construct suffix array of t */

sdsl::csa_bitcompressed<> sa;
construct_im(sa, t, 1);
for (sdsl::csa_bitcompressed<>::iterator iter = sa.begin(); iter != sa.end(); iter++) std::cout << (*iter) << " "; std::cout << std::endl;

/* constuct longest common prefix array of t */

sdsl::lcp_bitcompressed<> lcp;
construct_im(lcp, t, 1);
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
//for (int i=0; i<tt.size(); i++) std::cout << tt[i] << " "; std::cout << std::endl;

/* construct array E */
std::vector<int> PRIME_NUMBERS = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};
std::vector<int>::const_iterator first = PRIME_NUMBERS.begin();
std::vector<int>::const_iterator last = PRIME_NUMBERS.begin() + m;
std::vector<int> e(first,last);
//for (int i=0; i<e.size(); i++) std::cout << e[i] << " "; std::cout << std::endl;

/* */
int h = 4;
int j = 2;
int test = get_lcp(&lcp, &h, &j);
std::cout << test << std::endl;




return 0;

} //END_MAIN

/******************************* FUNCTION DEFINITIONS */
int get_lcp(sdsl::lcp_bitcompressed<> *lcp, int *x, int *y)
{
int i;
int j;
if ( (*x) < (*y) ){
	i = (*x);
	j = (*y);
} else {
	i = (*y);
	j = (*x);
}
std::cout << "i = " << i << std::endl;
std::cout << "j = " << j << std::endl;
int min = (*lcp)[(i+1)];
std::cout << "i +1 = " << i+1 << std::endl;
std::cout << "first pos in lcp to check is " << (*lcp)[3] << std::endl;
std::cout << "before loop " << min << std::endl;
for (sdsl::lcp_bitcompressed<>::iterator iter = (*lcp).begin() + i + 2; iter != (*lcp).begin() + j; iter++){
	if ( (*iter) < min ) min = (*iter);
	std::cout << "new min" << min << std::endl;
}
return min;
}

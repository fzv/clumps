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
std::vector<int> pvalues(std::vector<std::vector<std::pair<int,int>>> *M, int *i);

/******************************* MAIN FUNCTION */

int main(int argc, char* argv[])
{

std::cout << "Welcome to CLUMP" << std::endl;

/* parse input */

std::string textfile = argv[1];
int l = atoi(argv[3]);
int m = atoi(argv[5]);
int d = atoi(argv[7]);
int k = atoi(argv[9]);
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
		std::cout << "e = " << e << std::endl;
		//int boundary = m - (pos+1);
		p = p * E[pos];
		std::cout << "prime number = " << p << std::endl;
		//std::cout << "boundary = " << boundary << std::endl;
		while ( (pos < m) && (e <= d) ){
			std::cout << "INSIDE WHILE LOOP" << std::endl;
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
			//boundary = m - (pos+1);
			//std::cout << "boundary = " << boundary << std::endl;
			if (pos < m){
				e++;
				std::cout << "e = " << e << std::endl;
				p = p * E[pos];
				std::cout << "prime number = " << p << std::endl;
			}
		} //END_WHILE
		if (e <= d){
			std::cout << "INSERTING IN ARRAY M the following" << std::endl;
			std::cout << "p = " << p << std::endl;
			std::pair <int,int> jp = std::make_pair(j,p);
			M[i].push_back(jp);
		} else {
			p = -1;
			std::cout << "p = " << p << std::endl;
			std::pair <int,int> jp = std::make_pair(j,p);
			M[i].push_back(jp);
		}
		} //END_IF(i<j)
	}
}

/* sort M */


/* print M */
for (int i = 0; i != r; i++){
	if (M[i].size() != 0){
		for (int x = 0; x != M[i].size(); x++){
			std::cout << "i = " << i << " ---" << " j = " << M[i][x].first << " ---" << " p = " << M[i][x].second << std::endl;
		}
	}
}

/* create string X */
std::cout << "l = " << l << std::endl; 
int ll = l-m+1;
std::cout << "l' = " << ll << std::endl; 
std::vector<int> x(ll, -1);
x.insert( x.end() , tt.begin() , tt.end() );
std::cout << "printing string X" << std::endl;
for (int i=0; i<x.size(); i++) std::cout << x[i] << " "; std::cout << std::endl;

/* initialise parkih vector */
std::vector<int> parikh(r+1, 0);
std::cout << "printing parikh vector" << std::endl;
for (int i=0; i<parikh.size(); i++) std::cout << parikh[i] << std::endl;

/* read X */
for (int i = 0; i < n-m+1; i++){
	std::cout << "step " << i << std::endl;
	if ( tt[i] != -1) parikh[tt[i]]++; 
	if ( ( (i-ll) >= 0 ) && ( tt[i-ll] != -1 ) ) parikh[tt[i-ll]]--;
	std::cout << "printing parikh vector" << std::endl;
	for (int i=0; i<parikh.size(); i++) std::cout << parikh[i] << std::endl;
	if ( parikh[tt[i]] >= k ){
		std::cout << "reporting solid clump!!!" << std::endl;
	} else {
		std::cout << "attempting to merge" << std::endl;
		std::vector<int> unique;
		unique = pvalues(&M, &tt[i]);
		std::cout << "printing unique vector outside function" << std::endl;
		for (int i=0; i<unique.size(); i++) std::cout << unique[i] << std::endl;
		std::vector<int> sum;
		for (int i=0; i<unique.size(); i++) sum.push_back(0);
		//IF SUM[I] PLUS P(TT[I]) > K then report
		for (std::vector<std::vector<std::pair<int,int>>>::iterator a = M.begin(); a != M.end(); a++)
		{
			for (std::vector<std::pair<int,int>>::iterator b = (*a).begin(); b != (*a).end(); b++)
			{
				if ( ( ( (std::distance( M.begin() , a ))==tt[i] ) || ( (*b).first == tt[i] ) ) && ( (*b).second != -1 ) )
				{
					//find index of b.second in unqiue (+ tf in sum) = ind
					//find the val of b.first in parikh = val
					//sum[ind] += val
				{
			}
		}
				
			}
		}
//TODO get rid of -1's in M







return 0;

} //END_MAIN

/******************************* FUNCTION DEFINITIONS */
int rmq
/*
* function: naive range minimum query TODO implemtnt non naive
* time complexity: linear
* space complexity: n/a
*/
( //PARAMS
sdsl::lcp_bitcompressed<> *lcp
, int *x
, int *y
) //END_PARAMS
{ //FUNCTION
std::cout << "INSIDE LCP FUNCTION" << std::endl;
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
std::cout << "first value in range is " << min << std::endl;
std::cout << "i+1 = " << (i+1) << std::endl;
std::cout << "j = " << j << std::endl;
if ( j != (i+1) ){
	std::cout << "j != i+1" << std::endl;
	for (sdsl::lcp_bitcompressed<>::iterator iter = (*lcp).begin() + i + 1; iter <= (*lcp).begin() + j; iter++){
		std::cout << "next value in range is " << (*iter) << std::endl;
		if ( (*iter) < min ) min = (*iter);
	} //END_FOR
} //END_IF
return min;
} //END_FUNCTION(rmq)

std::vector<int> pvalues
/*
* function: 
* time complexity: 
* space complexity:
*/
( //PARAMS
  std::vector<std::vector<std::pair<int,int>>> * M
, int * i
) //END_PARAMS
{ //FUCNTION
std::cout << "INSIDE PVALUES FUNCTION" << std::endl;
std::cout << "i = " << (*i) << std::endl;
std::vector<int> unique;

for (std::vector<std::vector<std::pair<int,int>>>::iterator a = (*M).begin(); a != (*M).end(); a++)
{
	for (std::vector<std::pair<int,int>>::iterator b = (*a).begin(); b != (*a).end(); b++)
	{
		if ( ( ( (std::distance( (*M).begin() , a ))==(*i) ) || ( (*b).first == (*i) ) ) && ( (*b).second != -1 ) ) unique.push_back( (*b).second );
	}
}

/*
unique.push_back((*Mi)[0].second);
for (std::vector<std::pair<int,int>>::iterator iter = (*Mi).begin() + 1; iter != (*Mi).end(); iter++){
	if ( (*iter).second != (*(iter-1)).second ) unique.push_back((*iter).second);
}
*/
std::cout << "printing unique vector iside function" << std::endl;
for (int i=0; i<unique.size(); i++) std::cout << unique[i] << std::endl;
return unique;
} //END_FUNCTION{pvalues}

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

void constructT
/*
* function: 
* time complexity: 
* space complexity:
*/
( //PARAMS
std::vector<int>& tt
, int& r
, sdsl::csa_bitcompressed<>& sa
, int& n
, int& m
, sdsl::lcp_bitcompressed<>& lcp
, int& ll
) //END_PARAMS
{ //FUNCTION
int i = 0;
do{
	if ( sa[i] < (n-m+1) ){ //omitting suffixes of length < m
		int j = i;
		while ( ( sa[j+1] < (n-m+1) ) && ( lcp[j+1] >= m ) ){ //while (next suffix in SA is longer than m) AND (this suffix and next suffix in SA have lcp > m)
			j++; //no. of occ of this suffix in tt + i
		}
		if ( j-i < ceil(ll/2) ){ //if no. of occ < l'/2
			r++;
			for (int k=i; k<=j; k++){ //for each of the identical m-grams
				tt[sa[k]] = r; //set them to r
			}
		}
		i = j+1; //move to the next unique m-gram
	} else { //if len of suffix is less than m
		i++; //skip this suffix
	}
} while (i != n+1); //loop through text
} //END_FUNCTION(constructT)

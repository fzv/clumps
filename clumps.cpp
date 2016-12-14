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

int main(int argc, char* argv[])
{

std::cout << "Welcome to CLUMP" << std::endl;

// parse input

std::string textfile = argv[1];
//int l = argv[2];
int l = 6;
//int m = argv[3];
int m = 3;
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

// construct suffix array of t

sdsl::csa_bitcompressed<> sa;
construct_im(sa, t, 1);

// constuct longest common prefix array of t

sdsl::lcp_bitcompressed<> lcp;
construct_im(lcp, t, 1);

// construct t'
/*
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
*/
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
for (int i=0; i<tt.size(); i++) std::cout << tt[i] << " "; std::cout << std::endl;

return 0;

} //END_MAIN

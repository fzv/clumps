//usage
// k1457070@nmscde000493:~/clumps$ ./clumps paper.fas -l 6 -m 3 -d 1
//k1457070@nmscde000493:~/clumps$ ./clumps paper.fas -l 7 -m 3 -d 1 -k 2

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
int rmq(sdsl::lcp_bitcompressed<> *lcp, int *x, int *y, std::ofstream *logfile);
std::vector<int> pvalues(std::vector<std::vector<std::pair<int,std::pair<int,int>>>> *M, int *i, std::ofstream *logfile);
bool compare(std::pair<int,std::pair<int,int>> *left, std::pair<int,std::pair<int,int>> *right);

/******************************* MAIN FUNCTION */

int main(int argc, char* argv[])
{

std::cout << "Welcome to CLUMP" << std::endl;

/* Begin logging */

std::ofstream logfile; 
logfile.open("clumps.log");
logfile << "LOG" << std::endl;

/* parse input */

std::string textfile = argv[1];
int l = atoi(argv[3]);
int m = atoi(argv[5]);
int ll = l-m+1;
logfile << "l'= " << ll << std::endl;
logfile << "l/2= " << ceil(ll/2) << std::endl;
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
	logfile << "Unable to open file." << std::endl;
}
std::string t = tstream.str();
int n = t.length();
logfile << std::endl << "printing text" << std::endl;
logfile << t << std::endl;
logfile << std::endl << "n = " << n << std::endl;

/* construct suffix array of t */

sdsl::csa_bitcompressed<> sa;
construct_im(sa, t, 1);
logfile << std::endl << "printing suffix array" << std::endl;
for (sdsl::csa_bitcompressed<>::iterator iter = sa.begin(); iter != sa.end(); iter++) logfile << (*iter) << " "; logfile << std::endl;

/* constuct longest common prefix array of t */

sdsl::lcp_bitcompressed<> lcp;
construct_im(lcp, t, 1);
logfile << std::endl << "printing lcp array" << std::endl;
for (sdsl::lcp_bitcompressed<>::iterator iter = lcp.begin(); iter != lcp.end(); iter++) logfile << (*iter) << " "; logfile << std::endl;

/* construct t' without  frequently occuring patterns of length m */
std::vector<int> tt( (n-m+1) , -1);
int r = -1;
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
logfile << std::endl << "printing T'" << std::endl;
for (int i=0; i<tt.size(); i++) logfile << tt[i] << " "; logfile << std::endl;
logfile << std::endl << "highest rank is " << r << std::endl;

/**********************************************************************************************
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
logfile << "printing T'" << std::endl;
for (int i=0; i<tt.size(); i++) logfile << tt[i] << " "; logfile << std::endl;
logfile << "highest rank is " << r << std::endl;
*******************************************************************************************/


/* construct array E */
std::vector<int> PRIME_NUMBERS = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29}; // TODO const int??? // TODO add more numbers 
std::vector<int>::const_iterator first = PRIME_NUMBERS.begin();
std::vector<int>::const_iterator last = PRIME_NUMBERS.begin() + m;
std::vector<int> E(first,last);
logfile << std::endl << "printing array E" << std::endl;
for (int i=0; i<E.size(); i++) logfile << E[i] << " "; logfile << std::endl;



/* construction of array M */
std::vector<std::vector<std::pair<int,std::pair<int,int>>>> M;
for (int i = 0; i <= r; i++)
{
	std::vector<std::pair<int,std::pair<int,int>>> vec;
	M.push_back(vec);
}

for (int i = 0; i <= r; i++){
	for (int j = 0; j <= r; j++){
		/////logfile << "\n\ni = " << i << "    " << "j = " << j << std::endl;
		if (i != j){
		int p = 1;
		//which suffix does rank i/j represent?
		std::vector<int>::iterator iter_suffi = std::find(tt.begin(), tt.end(), i);
		std::vector<int>::iterator iter_suffj = std::find(tt.begin(), tt.end(), j);
		int suffi = std::distance(tt.begin(),iter_suffi);
		/////logfile << "\n\nRANK " << i << " represents suffix " << suffi << std::endl; 
		int suffj = std::distance(tt.begin(),iter_suffj);
		/////logfile << "RANK " << j << " represents suffix " << suffj << std::endl; 
		//where is suffix i/j in the suffix array?
		sdsl::csa_bitcompressed<>::iterator iter_sai = std::find(sa.begin(), sa.end(), suffi);
		sdsl::csa_bitcompressed<>::iterator iter_saj = std::find(sa.begin(), sa.end(), suffj);
		int sai = std::distance(sa.begin(),iter_sai);
		int saj = std::distance(sa.begin(),iter_saj);
		/////logfile << "suff i is in pos " << sai << " of suffix array" << std::endl;
		/////logfile << "suff j is in pos " << saj << " of suffix array" << std::endl;
		//
		int match = rmq(&lcp, &sai, &saj, &logfile);
		/////logfile << "match=rmq=lcp= " << match << std::endl;
		int pos = match;
		/////logfile << "pos where mismatch " << pos << std::endl;
		int e = 1;
		/////logfile << "e = " << e << std::endl;
		//int boundary = m - (pos+1);
		p = p * E[pos];
		/////logfile << "prime number = " << p << std::endl;
		//logfile << "boundary = " << boundary << std::endl;
		while ( (pos < m) && (e <= d) ){
			/////logfile << "INSIDE WHILE LOOP" << std::endl;
			suffi = suffi + match + 1;
			suffj = suffj + match + 1;
			iter_sai = std::find(sa.begin(), sa.end(), suffi);
			iter_saj = std::find(sa.begin(), sa.end(), suffj);
			sai = std::distance(sa.begin(),iter_sai);
			saj = std::distance(sa.begin(),iter_saj);

			/////logfile << "suffix " << suffi << std::endl; 
			/////logfile << "which is in pos " << sai << " of suffix array" << std::endl;
			/////logfile << "suffix " << suffj << std::endl; 
			/////logfile << "which is in pos " << saj << " of suffix array" << std::endl;

			match = rmq(&lcp, &sai, &saj, &logfile);
			/////logfile << "match=rmq=lcp= " << match << std::endl;
			pos = match + pos + 1;
			/////logfile << "pos where mismatch " << pos << std::endl;
			//boundary = m - (pos+1);
			//logfile << "boundary = " << boundary << std::endl;
			if (pos < m){
				e++;
				/////logfile << "e = " << e << std::endl;
				p = p * E[pos];
				/////logfile << "prime number = " << p << std::endl;
			}
		} //END_WHILE
		if (e <= d){
			/////logfile << "INSERTING IN ARRAY M the following" << std::endl;
			/////logfile << "p = " << p << std::endl;
			/////logfile << "e = " << e << std::endl;
			std::pair<int,int> pe = std::make_pair(p,e);
			/////logfile << "pe = (" <<  pe.first << "," << pe.second << ")" << std::endl;
			std::pair<int,std::pair<int,int>> jpe = std::make_pair(j,pe);
			/////logfile << "j = " << jpe.first << std::endl;
			M[i].push_back(jpe);
			/////logfile << "inserted" << std::endl;
		} /* else {
			/////logfile << "INSERTING IN ARRAY M the following" << std::endl;
			p = -1;
			e = -1;
			/////logfile << "p = " << p << std::endl;
			/////logfile << "e = " << e << std::endl;
			std::pair<int,int> pe = std::make_pair(p,e);
			/////logfile << "pe = (" <<  pe.first << "," << pe.second << ")" << std::endl;
			std::pair<int,std::pair<int,int>> jpe = std::make_pair(j,pe);
			/////logfile << "j = " << jpe.first << std::endl;
			M[i].push_back(jpe);
			/////logfile << "inserted" << std::endl;
		} */
		} //END_IF(i != j)
	}
}
/* sort M wrt p*/
for (std::vector<std::vector<std::pair<int,std::pair<int,int>>>>::iterator iter = M.begin(); iter != M.end(); iter++) //each vector in M
{
	std::sort( (*iter).begin() , (*iter).end() , compare );
}

/* make copy of M with pointers to factors = M' */
/*
std::vector<std::vector<std::pair<int,std::pair<int,int>>>> MM;
for (int i = 0; i <= r; i++)
{
	std::vector<std::pair<int,std::pair<int,int>>> vec;
	MM.push_back(vec);
}
*/
/*
std::vector<std::vector<int*>> MM;
for (int i = 0; i <= r; i++)
{
	<std::vector<int*> vec;
	MM.push_back(vec);
}
*/
/*
for (int i = 0; i <= r; i++){ //loop through M
	if (!M[i].empty()){
		for (int x = 0; x != M[i].size(); x++){ //loop through M[i]
			int e = M[i][x].second.second; 
			if (e != 1){ //if p of i,j is not prime number (only divisble by 1 and itself)
				for ( int iter = 0 ; iter < M[i][x].second.first ; iter++ ){
					int p = M[i][iter].second.first;
					if ( ( M[i][x].second.first % p ) == 0 )
					{
						
					}
				}
			}
		}
	} else { //if M[i] is empty
		MM.push_back(-1); //necessary???
	}
}
*/



/* print M */
logfile << "printing array M" << std::endl;
for (int i = 0; i <= r; i++){
	if (M[i].size() != 0){
		for (int x = 0; x != M[i].size(); x++){
			logfile << "i = " << i << " ---" << " j = " << M[i][x].first << " ---" << " p = " << M[i][x].second.first << " ---" << " e = " << M[i][x].second.second << std::endl;
		}
	}
}

/* create string X */
std::vector<int> x(ll, -1);
x.insert( x.end() , tt.begin() , tt.end() );
logfile << "\n\nprinting string X" << std::endl;
for (int i=0; i<x.size(); i++) logfile << x[i] << " "; logfile << std::endl;

/* initialise parkih vector */
std::vector<int> parikh(r+1, 0);
logfile << "printing parikh vector" << std::endl;
for (int i=0; i<parikh.size(); i++) logfile << parikh[i] << std::endl;

/* read X */
/*
for (int i = 0; i < n-m+1; i++){ //read each letter in string X
	logfile << "step " << i << std::endl;
	if ( tt[i] != -1) parikh[tt[i]]++; //if new letter is legit, increase its freq in parikh vec
	if ( ( (i-ll) >= 0 ) && ( tt[i-ll] != -1 ) ) parikh[tt[i-ll]]--; //if old letter was legit & was within text, decrease its freq in parikh vec
	logfile << "printing parikh vector" << std::endl;
	for (int i=0; i<parikh.size(); i++) logfile << parikh[i] << std::endl;
	if ( parikh[tt[i]] >= k ){ //if this pattern occurs >= k times in current window, report solid clump
		logfile << "reporting solid clump!!!" << std::endl;
	} else { //if not, try to merge to reach k occurrences
		if ( ! M[tt[i]].empty() )
		{
		logfile << "attempting to merge" << std::endl;
		std::vector<int> unique; //this will hold all unique p values in M[tt[i]]
		unique = pvalues(&M, &tt[i], &logfile);
		logfile << "printing unique vector outside function" << std::endl;
		for (int i=0; i<unique.size(); i++) logfile << unique[i] << std::endl;
		// initialise sum vector with parikh vector values 
		std::vector<int> sum; //corresponds to unique vector
		for (std::vector<std::pair<int,std::pair<int,int>>>::iterator iter = M[tt[i]].begin(); iter != M[tt[i]].end(); iter++)
		{
		//TODO TODO TODO
		}
		////////////for (int i=0; i<unique.size(); i++) sum.push_back( parikh[tt[i]] );
		//  
		for (std::vector<std::vector<std::pair<int,int>>>::iterator a = M.begin(); a != M.end(); a++)
		{
			for (std::vector<std::pair<int,int>>::iterator b = (*a).begin(); b != (*a).end(); b++)
			{
				if ( ( ( (std::distance( M.begin() , a ))==tt[i] ) || ( (*b).first == tt[i] ) ) && ( (*b).second != -1 ) )
				{
					//find index of b.second in unqiue (+ therefore in sum) = ind
					std::vector<int>::iterator iter = std::find( unique.begin() , unique.end() , ((*b).second) );
					int ind = std::distance( unique.begin() , iter );
					//find the val of b.first in parikh = val
					int val = parikh[(*b).first];
					sum[ind] += val;
				}
			} //END_FOR
		} //END_FOR
		logfile << "printing sum vector" << std::endl;
		for (int i=0; i<sum.size(); i++) logfile << sum[i] << std::endl;
		for (int i = 0; i < unique.size(); i++)
		{
			if (isPrime[i] && sum[i] >= k)
			{
				logfile << "reporting degenerate clump!!!" << std::endl;
			}
			else
			{
				logfile << "todo" << std::endl;
			}
		} //END_FOR
		} //END_IF( M[t'[i]] is not empty )
	} //END_IF
} //END_FOR(each letter in X)
*/




logfile.close();



return 0;

} //END_MAIN

bool compare
/*
* function: 
* time complexity: 
* space complexity:
*/
( //PARAMS
  std::pair<int,std::pair<int,int>> *left
, std::pair<int,std::pair<int,int>> *right
) //END_PARAMS
{ //FUNCTION
return (*left).second.first < (*right).second.first;
} //END_FUNCTION{compare}

/*
http://stackoverflow.com/questions/279854/how-do-i-sort-a-vector-of-pairs-based-on-the-second-element-of-the-pair
std::sort(v.begin(), v.end(), [](const std::pair<int,int> &left, const std::pair<int,int> &right) {
    return left.second < right.second;
});
*/

/******************************* FUNCTION DEFINITIONS */
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
/////(*logfile) << "INSIDE LCP FUNCTION" << std::endl;
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
/////(*logfile) << "first value in range is " << min << std::endl;
/////(*logfile) << "i+1 = " << (i+1) << std::endl;
/////(*logfile) << "j = " << j << std::endl;
if ( j != (i+1) ){
	/////(*logfile) << "j != i+1" << std::endl;
	for (sdsl::lcp_bitcompressed<>::iterator iter = (*lcp).begin() + i + 1; iter <= (*lcp).begin() + j; iter++){
		/////(*logfile) << "next value in range is " << (*iter) << std::endl;
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
  std::vector<std::vector<std::pair<int,std::pair<int,int>>>> *M
, int *i // = t'[i]
, std::ofstream *logfile
) //END_PARAMS
{ //FUCNTION
/////(*logfile) << "INSIDE PVALUES FUNCTION" << std::endl;
/////(*logfile) << "i = " << (*i) << std::endl;
std::vector<int> unique;
/*
for (std::vector<std::vector<std::pair<int,int>>>::iterator a = (*M).begin(); a != (*M).end(); a++)
{
	for (std::vector<std::pair<int,int>>::iterator b = (*a).begin(); b != (*a).end(); b++)
	{
		if ( ( ( (std::distance( (*M).begin() , a ))==(*i) ) || ( (*b).first == (*i) ) ) && ( (*b).second != -1 ) ) unique.push_back( (*b).second );
	}
}
*/
/*
unique.push_back((*Mi)[0].second);
for (std::vector<std::pair<int,int>>::iterator iter = (*Mi).begin() + 1; iter != (*Mi).end(); iter++){
	if ( (*iter).second != (*(iter-1)).second ) unique.push_back((*iter).second);
}
*/
//if ( ! (*M)[*i].empty() )
//{
unique.push_back( (*M)[(*i)][0].second.second );
if ( (*M)[(*i)].size() > 1 )
{
	for (std::vector<std::pair<int,std::pair<int,int>>>::iterator iter = (*M)[(*i)].begin() + 1; iter != (*M)[(*i)].end(); iter++)
	{
		if ( (*iter).second.second != (*(iter-1)).second.second ) unique.push_back( (*iter).second.second );
	}
}
//}
/////(*logfile) << "printing unique vector iside function" << std::endl;
/////for (int i=0; i<unique.size(); i++) (*logfile) << unique[i] << std::endl;
return unique;
} //END_FUNCTION{pvalues}

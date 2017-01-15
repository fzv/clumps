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
void fillSums(std::vector<std::pair<int,std::pair<int,int>>> *Mi, std::map<int,int> *sumsi, std::vector<int> *parikh, std::ofstream *logfile);
void checkSums(int *occ,std::map<int,int> *sumsi,int *k,std::vector<int> *E,std::ofstream *logfile);

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
std::vector<int> PRIME_NUMBERS = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29}; // TODO add more numbers 
std::vector<int>::const_iterator first = PRIME_NUMBERS.begin();
std::vector<int>::const_iterator last = PRIME_NUMBERS.begin() + m;
std::vector<int> E(first,last);
logfile << std::endl << "printing array E" << std::endl;
for (int i=0; i<E.size(); i++) logfile << E[i] << " "; logfile << std::endl;
/* construction of array M */
std::vector<std::vector<std::pair<int,std::pair<int,int>>>> M;
M.reserve(r);
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
			} //END_IF
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
/* sort M wrt p */
/*
for (std::vector<std::vector<std::pair<int,std::pair<int,int>>>>::iterator iter = M.begin(); iter != M.end(); iter++) //each vector in M
{
	std::sort( (*iter).begin() , (*iter).end() , compare );
}
*/
//std::vector<int> sizes(r+1,0);
//int s = 0;
for (std::vector<std::vector<std::pair<int,std::pair<int,int>>>>::iterator iter = M.begin(); iter != M.end(); iter++) //each vector in M
{
	//sizes[s] = (*iter).size();
	//s++;
	std::sort( (*iter).begin() , (*iter).end() , [ ]( const std::pair<int,std::pair<int,int>> left, std::pair<int,std::pair<int,int>> right )
	{
		return ( left.second.first < right.second.first );
	}
	);
}
//logfile << "printing sizes vector" << std::endl;
//for (int i = 0; i < sizes.size(); i++) logfile << " " << sizes[i];
//logfile << std::endl;
/* matrix similar to M to hold unique p values in M, and the sum of their frequencies in current inwodw  */
//std::vector<std::vector<std::pair<int,int>>> sums;
std::vector<std::map<int,int>> sums;
sums.reserve(r+1);
/*
for (int i = 0; i <= r; i++) //each vector in M
{
	int myind = M[i].size() - 1;
	int mysize = M[i][myind].second.first;
	std::vector<std::pair<int,int>> myvector( mysize, std::pair<int,int>(-1,-1) );
	sums.push_back(myvector);
}
*/
for (int i = 0; i <= r; i++) //each vector in M
{
	//std::vector<int> v(sizes[i],-1);
	std::map<int,int> mymap;
	if ( ! M[i].empty() )
	{
		//sums[i][0].first = M[i][0].second.first;
		mymap[ M[i][0].second.first ] = 0;
		if ( M[i].size() > 1 )
		{
			//int j = 1;
			for (std::vector<std::pair<int,std::pair<int,int>>>::iterator iter = M[i].begin() + 1; iter != M[i].end(); iter++)
			{
				if ( (*iter).second.first != (*(iter-1)).second.first )
				{
					//sums[i][j].first = (*iter).second.first;
					//j++;
					mymap[ (*iter).second.first ] = 0;
				}
			}
		}
	} else {
		mymap[-1] = -1;
	}
	sums.push_back(mymap);	
}
logfile << "sums has been initialised" << std::endl;

/* print M */
logfile << "printing array M" << std::endl;
for (int i = 0; i <= r; i++){
	if (M[i].size() != 0){
		for (int x = 0; x != M[i].size(); x++){
			logfile << "i = " << i << " ---" << " j = " << M[i][x].first << " ---" << " p = " << M[i][x].second.first << " ---" << " e = " << M[i][x].second.second << std::endl;
		}
	}
}
/*
for (int i = 0; i <= r; i++) //each vector in P
{
	logfile << "unique values in M[" << i << "] =";
	for (int j = 0; j < sums[i].size(); j++)
	{
		logfile << " " << sums[i][j].first;
	}
	logfile << std::endl;
}
*/
logfile << "printing sums" << std::endl;
for (int i = 0; i <= r; i++) //each map in sums
{
	logfile << "i = " << i << std::endl;
	for (auto const& iter : sums[i])
	{
		logfile << iter.first << " - " << iter.second << std::endl;
	}
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
std::vector<std::vector<std::vector<int*>>> MM;
for (int i = 0; i <= r; i++)
{
	std::vector<std::vector<int*> vec;
	for (int it = 0; it < M[i].size(); it++)
	{
		std::vector<int*> v;
		vec.push_back(v);
	}
	MM.push_back(vec);
}
for (int i = 0; i <= r; i++){ //loop through M
	std::vector<std::vector<int*>> vecvec;
	std::vector<int*> vec;
	int* nulptr = NULL;
	if (!M[i].empty()){ //as long as M[i] contains something
		for (int x = 0; x != M[i].size(); x++){ //loop through M[i]
			int e = M[i][x].second.second; 
			if (e != 1){ //if p of i,j is not prime number (only divisble by 1 and itself)
				for ( int iter = 0 ; iter < x ; iter++ ){ //loop through M[i] again from start to current M[i][x] 
					int p = M[i][iter].second.first;
					if ( ( M[i][x].second.first % p ) == 0 )
					{
						int* ptr = &(M[i][x].first.first); //j
						vec.push_back(ptr);
					}
				}
			} else { //if p of i,j is a prime number
				vec.push_back(nulptr);
			}
			vecvec.push_back(vec);
		}
	} else { //if M[i] is empty
		vec.push_back(nulptr);
		vecvec.push_back(vec);	
	}
	MM[i].push_back(vecvec);
}
int* pointerto2 = &(M[0][0].second.first);
std::cout << pointerto2 << std::endl; //prints address of M[0][0].second.first
std::cout << (*pointerto2) << std::endl; //prints M[0][0].second.first
*/

/* create string X */
/*
std::vector<int> x(ll, -1);
x.insert( x.end() , tt.begin() , tt.end() );
logfile << "\n\nprinting string X" << std::endl;
for (int i=0; i<x.size(); i++) logfile << x[i] << " "; logfile << std::endl;
*/
/* initialise parkih vector */
std::vector<int> parikh(r+1, 0);
logfile << "printing parikh vector" << std::endl;
for (int i=0; i<parikh.size(); i++) logfile << parikh[i] << std::endl;

/* read X */

for (int i = 0; i < n-m+1; i++){ //read each letter in string T'
	logfile << std::endl << "step " << i << std::endl;
	std::string pat = t.substr(i,3);
	logfile << "reading pattern " << pat << std::endl;
	logfile << "represented by rank " << tt[i] << std::endl;
	if ( tt[i] != -1) parikh[tt[i]]++; //if new letter is legit, increase its freq in parikh vec
	if ( ( (i-ll) >= 0 ) && ( tt[i-ll] != -1 ) ) parikh[tt[i-ll]]--; //if old letter was legit & was within text, decrease its freq in parikh vec
	logfile << "printing parikh vector" << std::endl;
	for (int i=0; i<parikh.size(); i++) logfile << parikh[i] << std::endl;
	if ( parikh[tt[i]] >= k ){ //if this pattern occurs >= k times in current window, report solid clump
		logfile << "reporting solid clump with pattern " << pat << std::endl;
	} else { //if not, try to merge to reach k occurrences
		if ( ! M[tt[i]].empty() )
		{
		logfile << "attempting to merge" << std::endl;
		fillSums(&M[tt[i]], &sums[tt[i]], &parikh, &logfile);
		
		logfile << "printing sums in main" << std::endl;
		for (int i = 0; i <= r; i++) //each map in sums
		{
			logfile << "i = " << i << std::endl;
			for (auto const& iter : sums[i])
			{
				logfile << iter.first << " - " << iter.second << std::endl;
			}
		}
		logfile << "calling checkSums()" << std::endl;
		checkSums( &parikh[tt[i]] , &sums[tt[i]] , &k , &E , &logfile);
		logfile << std::endl << "checkSums() completed" << std::endl;
/*
		std::vector<int> sum( P[tt[i]].size() , -1 );
		for (std::vector<std::pair<int,std::pair<int,int>>>::iterator iter = M[tt[i]].begin(); iter != M[tt[i]].end(); iter++)
		{
			//TODO TODO TODO
		}
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
		*/
		} //END_IF( M[t'[i]] is not empty )
	} //END_IF
} //END_FOR(each letter in X)

logfile.close();



return 0;

} //END_MAIN
/******************************* FUNCTION DEFINITIONS */
void checkSums
/*
* function: 
* time complexity: 
* space complexity:
*/
( //PARAMS
  int *occ
, std::map<int,int> *sumsi
, int *k
, std::vector<int> *E
, std::ofstream *logfile
) //END_PARAMS
{ //FUNCTION
// LEVEL 1
for (std::map<int,int>::iterator it = (*sumsi).begin(); it != (*sumsi).end(); it++)
{
	if ( ((*it).second + (*occ)) >= (*k) )
	{
		(*logfile) << "reporting deg pattern made from???" << std::endl;
		(*logfile) << (*it).first << "is the p" << std::endl;
	} else { // LEVEL 2 - LEVEL M
	}
}
} //END_FUNCTION
void fillSums
/*
* function: 
* time complexity: 
* space complexity:
*/
( //PARAMS
  std::vector<std::pair<int,std::pair<int,int>>> *Mi
, std::map<int,int> *sumsi
, std::vector<int> *parikh
, std::ofstream *logfile
) //END_PARAMS
{ //FUNCTION
for (std::vector<std::pair<int,std::pair<int,int>>>::iterator iter = (*Mi).begin();
	iter != (*Mi).end();
	iter++)
{
	//(*logfile) << (*sumsi)[ ( (*iter).second.first ) ] << std::endl;
	(*sumsi)[ (*iter).second.first ] += (*parikh)[ (*iter).first ];
}
} //END_FUNCTION(fillSums)

bool compare
/*
* function: 
* time complexity: 
* space complexity:
*/
( //PARAMS
  std::pair<int,std::pair<int,int>> left
, std::pair<int,std::pair<int,int>> right
) //END_PARAMS
{ //FUNCTION
return ( left.second.first < right.second.first );
} //END_FUNCTION{compare}

/*
http://stackoverflow.com/questions/279854/how-do-i-sort-a-vector-of-pairs-based-on-the-second-element-of-the-pair
std::sort(v.begin(), v.end(), [](const std::pair<int,int> &left, const std::pair<int,int> &right) {
    return left.second < right.second;
});
*/

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

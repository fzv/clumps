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
void checkSums(int *occ,std::map<int,int> *sumsi,int *k,std::vector<int> *E,std::vector<bool> *isPrime,int *tpos,std::vector<int> *tt,std::vector<std::pair<int,std::pair<int,int>>> *Mi,std::string *t,int *m,std::ofstream *logfile);
std::string getPattern(int &tpos,const int &p,std::vector<int>& tt,std::string& t,int& m,std::ofstream &logfile);

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
/* construct array E */
std::vector<int> PRIME_NUMBERS = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29}; // TODO add more numbers 
std::vector<int>::const_iterator first = PRIME_NUMBERS.begin();
std::vector<int>::const_iterator last = PRIME_NUMBERS.begin() + m;
std::vector<int> E(first,last);
logfile << std::endl << "printing array E" << std::endl;
for (int i=0; i<E.size(); i++) logfile << E[i] << " "; logfile << std::endl;
int product = 1;
for (int i=0; i<E.size(); i++) product = product * E[i];
logfile << "product of all m prime numbers in E = " << product << std::endl;
std::vector<bool> isPrime(product+1,false);
for (int i=0; i<E.size(); i++) isPrime[E[i]] = true;
logfile << "printing isPrime" << std::endl;
for (int i=0; i<isPrime.size(); i++) logfile << i << ": " << isPrime[i] << std::endl;
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
			} //END_IF
		} //END_WHILE
		if (e <= d){
			std::pair<int,int> pe = std::make_pair(p,e);
			std::pair<int,std::pair<int,int>> jpe = std::make_pair(j,pe);
			M[i].push_back(jpe);
		}
		} //END_IF(i != j)
	}
}
/* sort M wrt p */
for (std::vector<std::vector<std::pair<int,std::pair<int,int>>>>::iterator iter = M.begin(); iter != M.end(); iter++) //each vector in M
{
	std::sort( (*iter).begin() , (*iter).end() , [ ]( const std::pair<int,std::pair<int,int>> left, std::pair<int,std::pair<int,int>> right )
	{
		return ( left.second.first < right.second.first );
	}
	);
}
/* matrix similar to M to hold unique p values in M, and the sum of their frequencies in current inwodw  */
std::vector<std::map<int,int>> sums;
sums.reserve(r+1);
for (int i = 0; i <= r; i++) //each vector in M
{
	std::map<int,int> mymap;
	if ( ! M[i].empty() )
	{
		mymap[ M[i][0].second.first ] = 0;
		if ( M[i].size() > 1 )
		{
			for (std::vector<std::pair<int,std::pair<int,int>>>::iterator iter = M[i].begin() + 1; iter != M[i].end(); iter++)
			{
				if ( (*iter).second.first != (*(iter-1)).second.first )
				{
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
		const int p = -1;
		//logfile << "pattern is " << getPattern( i , p , tt ,  , logfile ) << std::endl; TODO
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
		checkSums( &parikh[tt[i]] , &sums[tt[i]] , &k , &E , &isPrime , &i , &tt , &M[tt[i]] , &t , &m , &logfile);
		} //END_IF( M[t'[i]] is not empty )
	} //END_IF
} //END_FOR(each letter in X)

logfile.close();



return 0;

} //END_MAIN
/******************************* FUNCTION DEFINITIONS */
std::string getPattern
/*
* function: given a position tpos in T', and (product of) prime(s) p, return degenerate pattern formed by rank at T'[i] and merge-able ranks according to positions represented by p
* time complexity: 
* space complexity:
*/
( //PARAMS
  int& i //position in T'
, const int& p
, std::vector<int>& tt
, std::vector<std::pair<int,std::pair<int,int>>>& Mi
, std::string& t
, int& m
, std::ofstream &logfile
) //END_PARAMS
{ //FUNCTION
logfile << "--------------------------------------------------------------" << std::endl;
std::string pattern = "string";
logfile << "p = " << p << std::endl;
logfile << "i s.t. tt[i] is current pos = " << i << std::endl;
std::vector<int> ranks;
ranks.push_back(tt[i]);
logfile << "current letter in T' is " << tt[i] << std::endl;
for (std::vector<std::pair<int,std::pair<int,int>>>::iterator it = Mi.begin(); it != Mi.end(); it++)
{
	if ( (*it).second.first == p ) ranks.push_back( (*it).first );
}
std::stringstream patterns;
for (std::vector<int>::iterator it = ranks.begin(); it != ranks.end(); it++)
{
	logfile << "rank is " << (*it) << std::endl;
	std::vector<int>::iterator iter;
	iter = std::find( tt.begin() , tt.end() , (*it) );
	int ttpos = std::distance(tt.begin(),iter);
	logfile << "m-gram is " << t.substr(ttpos,m) << std::endl;
	patterns << t.substr(ttpos,m) << ",";
}
pattern = patterns.str();
pattern.pop_back();
logfile << "--------------------------------------------------------------" << std::endl;
return pattern;
} //END_FUNCTION(getPattern)
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
, std::vector<bool> *isPrime
, int *tpos
, std::vector<int> *tt
, std::vector<std::pair<int,std::pair<int,int>>> *Mi
, std::string *t
, int *m
, std::ofstream *logfile
) //END_PARAMS
{ //FUNCTION
(*logfile) << "inside checkSums()" << std::endl;
for (std::map<int,int>::iterator it = (*sumsi).begin(); it != (*sumsi).end(); it++)
{
	(*logfile) << (*it).first << " is the p we are looking at currently............" << std::endl;
	if  ( ( (*isPrime)[ (*it).first ] == true ) )
	{// LEVEL 1
		(*logfile) << "trying LEVEL 1 merge" << std::endl;
		if (( ((*it).second + (*occ)) >= (*k) )){
			(*logfile) << "reporting LEVEL 1 deg pattern made from???" << std::endl;
			(*logfile) << (*it).first << " is the p" << std::endl;
			(*logfile) << "pattern is " << getPattern( (*tpos) , (*it).first , (*tt) , (*Mi) , (*t) , (*m) , (*logfile) ) << std::endl;
		}
	} else { // LEVEL 2 - LEVEL M
		(*logfile) << "trying LEVEL 2...m merge" << std::endl;
		int sum = (*occ) + (*it).second;
		(*logfile) << "sum before adding = " << sum << std::endl; 
		for (std::map<int,int>::iterator it2 = (*sumsi).begin(); it2 != (*sumsi).end(); it2++)
		{
			if ( ( (*it).first!=(*it2).first ) && ( ((*it).first % (*it2).first ))==0 ) sum += (*it2).second;
		}
		(*logfile) << "sum after adding = " << sum << std::endl; 
		if (sum >= (*k))
		{
			(*logfile) << "reporting deg pattern made from???" << std::endl;
			(*logfile) << (*it).first << " is the p" << std::endl;
			(*logfile) << "pattern is " << getPattern( (*tpos) , (*it).first , (*tt) , (*Mi) , (*t) , (*m) , (*logfile) ) << std::endl;
		}
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

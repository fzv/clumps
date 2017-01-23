//usage
// k1457070@nmscde000493:~/clumps$ ./clumps paper.fas -l 6 -m 3 -d 1
//k1457070@nmscde000493:~/clumps$ ./clumps paper.fas -l 7 -m 3 -d 1 -k 2

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
#include "sdsl/suffix_arrays.hpp"
#include "sdsl/lcp.hpp"
#include <ctime>

/**********************
* FUNCTION DECLARATIONS
***********************/
int rmq(sdsl::lcp_bitcompressed<> *lcp, int *x, int *y, std::ofstream *logfile);
std::vector<int> pvalues(std::vector<std::vector<std::pair<int,std::pair<int,int>>>> *M,
int *i, std::ofstream *logfile);
bool compare(std::pair<int,std::pair<int,int>> *left, std::pair<int,std::pair<int,int>> *right);
void fillSums(std::vector<std::pair<int,std::pair<int,int>>> *Mi, std::map<int,int> *sumsi,
std::vector<int> *parikh, std::ofstream *logfile);
void checkSums(int *occ,std::map<int,int> *sumsi,int *k,std::vector<int> *E,
std::vector<bool> *isPrime,int *tpos,std::vector<int> *tt,
std::vector<std::pair<int,std::pair<int,int>>> *Mi,std::string *t,int *m,std::ofstream *logfile);
std::string getPattern(int &tpos,const int &p,std::vector<int>& tt,std::string& t,int& m,
std::ofstream &logfile);
void constructT(std::vector<int>& tt , int& r , sdsl::csa_bitcompressed<>& sa, int& n , int& m ,
sdsl::lcp_bitcompressed<>& lcp , int& ll);
void constructM(std::vector<std::vector<std::pair<int,std::pair<int,int>>>>& M , int& r ,
std::vector<int>& tt , sdsl::csa_bitcompressed<>& sa , sdsl::lcp_bitcompressed<>& lcp , int& d ,
std::vector<int>& E
, int& m , std::ofstream& logfile);
void sortM(std::vector<std::vector<std::pair<int,std::pair<int,int>>>>& M);

/**********************
* MAIN FUNCTION
***********************/

int main(int argc, char* argv[])
{

/* print time */
const time_t ctt = time(0);
std::cout << asctime(localtime(&ctt));

/* Begin logging */

std::ofstream logfile; 
logfile.open("clumps.log");
logfile << "LOG" << std::endl;

/* parse text */
std::string textfile = argv[1];
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

/* TESTING ONLY: print text */
logfile << std::endl << "printing text" << std::endl;
logfile << t << std::endl;
logfile << std::endl << "n = " << n << std::endl;

/* parse input params */
int l = atoi(argv[3]);
int m = atoi(argv[5]);
int ll = l-m+1;
int d = atoi(argv[7]);
int k = atoi(argv[9]);

/* TESTING ONLY: print some params */
logfile << "l'= " << ll << std::endl;
logfile << "l/2= " << ceil(ll/2) << std::endl;

/* construct suffix array of t */
sdsl::csa_bitcompressed<> sa;
construct_im(sa, t, 1);

/* TESTING ONLY: print suffix array */
logfile << std::endl << "printing suffix array" << std::endl;
for (sdsl::csa_bitcompressed<>::iterator iter = sa.begin(); iter != sa.end(); iter++)
{
logfile << (*iter) << " ";
}
 logfile << std::endl;

/* constuct longest common prefix array of t */
sdsl::lcp_bitcompressed<> lcp;
construct_im(lcp, t, 1);

/* TESTING ONLY: print LCP array */
logfile << std::endl << "printing lcp array" << std::endl;
for (sdsl::lcp_bitcompressed<>::iterator iter = lcp.begin(); iter != lcp.end(); iter++)
{
logfile << (*iter) << " ";
}
logfile << std::endl;

/* construct T' without frequently occuring patterns of length m */
std::vector<int> tt( (n-m+1) , -1);
int r = -1;
constructT(tt , r , sa , n , m , lcp , ll);

/* construct array E */
std::vector<int> PRIME_NUMBERS = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29}; // TODO add more numbers 
std::vector<int>::const_iterator first = PRIME_NUMBERS.begin();
std::vector<int>::const_iterator last = PRIME_NUMBERS.begin() + m;
std::vector<int> E(first,last);

/* FOR TESTING ONLY: print array E */
logfile << std::endl << "printing array E" << std::endl;
for (int i=0; i<E.size(); i++)
{
logfile << E[i] << " "; logfile << std::endl;
}

/* compute largest possible p value (pmax) */
int product = 1;
for (int i=0; i<E.size(); i++)
{
product = product * E[i];
}

/* FOR TESTING ONLY: print pmax */
logfile << "product of all m prime numbers in E = " << product << std::endl;

/* construct isPrime */
std::vector<bool> isPrime( product+1 , false );
for (int i=0; i<E.size(); i++) isPrime[E[i]] = true;

/* FOR TESTING ONLY: print isPrime */
logfile << "printing isPrime" << std::endl;
for (int i=0; i<isPrime.size(); i++)
{
logfile << i << ": " << isPrime[i] << std::endl;
}

/* construction of array M */
std::vector<std::vector<std::pair<int,std::pair<int,int>>>> M;
M.reserve(r);
constructM(M , r , tt , sa , lcp , d , E , m , logfile);

/* sort M wrt p */
sortM(M);

/* matrix similar to M to hold unique p values in M, and the sum of their frequencies in current window  */
/*
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
			for (std::vector<std::pair<int,std::pair<int,int>>>::iterator iter = M[i].begin() + 1;
				iter != M[i].end();
				iter++)
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
}*/

std::vector<std::vector<int>> pvalues;
for (int i = 0; i <= r; i++)
{
	std::vector<int> v;
	if ( ! M[i].empty() )
	{
		v.push_back(M[i][0].second.first);
		if ( M[i].size() > 1 )
		{
			for (std::vector<std::pair<int,std::pair<int,int>>>::iterator iter = M[i].begin() + 1;
				iter != M[i].end();
				iter++)
			{
				if ( (*iter).second.first != (*(iter-1)).second.first )
				{
					v.push_back((*iter).second.first);
				}
			}
		}
	} else {
		v.push_back(-1);
	}
	pvalues.push_back(v);
}

logfile << "printing pvalues" << std::endl;
for (int i = 0; i <= r; i++)
{
	logfile << "pvalues[" << i << "] ----- "; 
	for (std::vector<int>::iterator it = pvalues[i].begin(); it != pvalues[i].end(); it++)
	{
		logfile << (*it) << " ";
	}
	logfile << std::endl;
}

/* FOR TESTING ONLY: print M */
logfile << "printing array M" << std::endl;
for (int i = 0; i <= r; i++){
	if (M[i].size() != 0){
		for (int x = 0; x != M[i].size(); x++){
			logfile
			<< "i = " << i << " ---"
			<< " j = " << M[i][x].first << " ---"
			<< " p = " << M[i][x].second.first << " ---"
			<< " e = " << M[i][x].second.second
			<< std::endl;
		}
	}
}

/* initialise parkih vector */
std::vector<int> parikh(r+1, 0);

/* FOR TESTING ONLY: print parikh vector */
logfile << "printing parikh vector" << std::endl;
for (int i=0; i<parikh.size(); i++)
{
logfile << parikh[i] << std::endl;
}

/* read T' */
for (int i = 0; i < n-m+1; i++){

	/* m-gram */
	std::string pattern = t.substr(i,3);

	/* FOR TESTING ONLY:  print details */
	logfile << std::endl << "step " << i << std::endl;
	logfile << "reading pattern " << pattern << std::endl;
	logfile << "represented by rank " << tt[i] << std::endl;

	/* shift window */
	if ( ( (i-ll) >= 0 ) && ( tt[i-ll] != -1 ) ) parikh[tt[i-ll]]--;
	if ( tt[i] != -1)
	{
	parikh[tt[i]]++;

	int occ = parikh[tt[i]];
	logfile << "occ is " << occ << std::endl;

	/* FOR TESTING ONLY: print parikh vector*/
	logfile << "printing parikh vector" << std::endl;
	for (int i=0; i<parikh.size(); i++) logfile << parikh[i] << std::endl;

	/* try to report something */
	if ( occ >= k ){
		logfile << "reporting solid clump with pattern " << pattern << std::endl;

	} else { 
		if ( ! M[tt[i]].empty() )
		{ //TODO WHAT IF NEXT IS END???
		logfile << "attempting to merge" << std::endl;
		std::vector<int> P = pvalues[tt[i]]; //TODO not efficient ^ copying
		logfile << "p.back = " << P.back() << std::endl;
		std::vector<int> SUM( P.back() , 0); //size = largest p in row of M
		std::vector<std::pair<int,std::pair<int,int>>>::iterator current = M[tt[i]].begin();
		std::vector<std::pair<int,std::pair<int,int>>>::iterator null =  M[tt[i]].end();
		bool reported = false;
		while ( current != null ) //(M[tt[i]].end())==NULL
		{
			int current_j = (*current).first;
			logfile << "current j = " << current_j << std::endl;
			int current_p = (*current).second.first;
			logfile << "current p = " << current_p << std::endl;
			int current_e = (*current).second.second;
			logfile << "current e = " << current_e << std::endl;
			std::vector<std::pair<int,std::pair<int,int>>>::iterator next = current + 1;
			int next_p;
			int next_e;
			if (next!=null){
			next_p = (*next).second.first;
			logfile << "next p = " << next_p << std::endl;
			next_e = (*next).second.second;
			logfile << "next e = " << next_e << std::endl;
			} else {
			next_p = -10;
			next_e = -10;
			}
			SUM[current_p] += parikh[current_j];
/***
			if (current_e==1)
			{
			logfile << "current e is 1" << std::endl;
			logfile << "trying to reach k with " << (SUM[current_p]+occ) << std::endl;
				if ( (SUM[current_p]+occ) >= k ){
					std::cout << "TODO REPORT DEG CLUMP" << std::endl;
					if ( current_e != next_e )
					{
						logfile << "setting pointer to null" << std::endl;
						current = null;
					}
				}
			}
			else
			{
				logfile << "e > 1" << std::endl;
				int sum = SUM[current_p];
				for ( std::vector<int>::iterator p = P.begin(); p != P.end(); p++ )
				{
					if ( (current_p % (*p)) == 0 )
					{
						sum += SUM[(*p)];
					}
				}
				if ( (sum+occ) >= k )
				{
					std::cout << "TODO REPORT DEG CLUMP" << std::endl;
					if ( current_e != next_e )
					{
						current = null;
					}
				}
			}
***/
			if ((SUM[current_p]+occ) >= k)
			{
				logfile << "TODO REPORT DEG CLUMP" << std::endl;
				reported = true;
				/*
				if ( current_e != next_e )
				{
					current = null;
				}
				*/
			} else if ( (current_p != next_p) && (current_e > 1) ){
				int sum = SUM[current_p];
				for ( std::vector<int>::iterator p = P.begin(); p != P.end(); p++ )
				{
					if ( current_p % (*p) == 0 )
					{
						sum += SUM[(*p)];
					}
				}
				logfile << "sum = " << sum << std::endl;
				if ( sum >= k )
				{
					logfile << "TODO REPORT DEG CLUMP" << std::endl;
					reported = true;
					/*
					if ( current_e != next_e )
					{
						current = null;
					}
					*/
				}
			}

			//if (current != null) current++; // loop through row of M
			if ( (current_e != next_e) && (reported==true))
			{
				current = null;
			} else {
				current++; //if next is null, current will become null anyway
			}
		} //END_WHILE

	/*
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
		checkSums( &parikh[tt[i]] , &sums[tt[i]] , &k , &E , &isPrime ,
			&i , &tt , &M[tt[i]] , &t , &m , &logfile);
	*/
		
		} //END_IF( M[t'[i]] is not empty )
	} //END_IF( parikh[tt[i]] >= k )
	} //END_IF(( tt[i] != -1))
} //END_FOR(each letter in X)

/* end program */
logfile.close();
const time_t cttt = time(0);
std::cout << asctime(localtime(&cttt));
return 0;

} //END_MAIN
/***************************
* FUNCTION DECLARATIONS
****************************/
std::string getPattern
/*
* function: given a position tpos in T', and (product of) prime(s) p,
		return degenerate pattern formed by rank at T'[i] and
		merge-able ranks according to positions represented by p
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
			(*logfile) << "pattern is "
			<< getPattern( (*tpos) , (*it).first , (*tt) , (*Mi) , (*t) , (*m) , (*logfile) )
			<< std::endl;
		}
	} else { // LEVEL 2 - LEVEL M
		(*logfile) << "trying LEVEL 2...m merge" << std::endl;
		int sum = (*occ) + (*it).second;
		(*logfile) << "sum before adding = " << sum << std::endl; 
		//for (std::map<int,int>::iterator it2 = (*sumsi).begin(); it2 != (*sumsi).end(); it2++)
		for (std::map<int,int>::iterator it2 = (*sumsi).begin(); it2 != it; it2++)
		{
			if ( ( (*it).first!=(*it2).first ) && ( ((*it).first % (*it2).first ))==0 ) sum += (*it2).second;
		}
		(*logfile) << "sum after adding = " << sum << std::endl; 
		if (sum >= (*k))
		{
			(*logfile) << "reporting deg pattern made from???" << std::endl;
			(*logfile) << (*it).first << " is the p" << std::endl;
			(*logfile) << "pattern is "
			<< getPattern( (*tpos) , (*it).first , (*tt) , (*Mi) , (*t) , (*m) , (*logfile) )
			<< std::endl;
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

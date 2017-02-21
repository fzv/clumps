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

void clumps(std::ofstream& logfile, std::string& t,int& n, int& m, int& ll, int& d, int& k, std::vector<std::pair<int,std::string>>& result)
{

/* print time */
const time_t START = time(0);
std::cout << asctime(localtime(&START));

//
std::cout << "Constructing Suffix Array" << std::endl;

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


//
std::cout << "Constructing Longest Common Prefix Array" << std::endl;

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

std::cout << "\n\nn = " << n << std::endl;
std::cout << "size of lcp array = " << lcp.size() << std::endl;


/* construct T' without frequently occuring patterns of length m */


//
std::cout << "Constructing T'" << std::endl;

std::vector<int> tt( (n-m+1) , -1);
int r = -1;
constructT(tt , r , sa , n , m , lcp , ll);

/* construct array E */


//
std::cout << "Constructing Array E" << std::endl;

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

/* prepare for RMQ */


//
std::cout << "Preparing for RMQ" << std::endl;

int lgn = flog2(n+1);
std::vector<int> table( ( (n+1)*lgn) , -1 );

std::cout << "pirnitng table" << std::endl;
for (int i = 0; i != table.size(); i++) std::cout << table[i] << " ";
std::cout << std::endl;

rmq_preprocess(table, lcp, (n+1) );

std::cout << "pirnitng table" << std::endl;
for (int i = 0; i != table.size(); i++) std::cout << table[i] << " ";
std::cout << std::endl;

/* construction of array M */


//
std::cout << "Constructing Array M" << std::endl;

std::vector<std::vector<std::pair<int,std::pair<int,int>>>> M;
M.reserve(r);
constructM(M , r , tt , sa , lcp , d , E , m , table , n , logfile);

/* sort M wrt p */
sortM(M);

/* get unique p values in each array in M*/
std::vector<std::vector<int>> pvalues = getPValues(r , M);

/* FOR TESTING ONLY: print pValues */
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
for (int i = 0; i <= r; i++)
{
	if (M[i].size() != 0)
	{
		for (int x = 0; x != M[i].size(); x++)
		{
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

/* prepare to report */
std::pair<int,std::string> window_result;


//
std::cout << "Reading T'" << std::endl;

/* read T' */
for (int i = 0; i < n-m+1; i++)
{
	//TODO: i is pos for computer scientist, not for biologist!
	/* m-gram */
	std::string pattern = t.substr(i,m);

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
		if ( occ >= k )
		{
			logfile << "reporting solid clump with pattern " << pattern << std::endl;
			//ivec.push_back(i); pvec.push_back(pattern);
			window_result = std::make_pair(i,pattern);
			result.push_back(window_result);
		}
		else
		{ 
			if ( ! M[tt[i]].empty() )
			{
				logfile << "attempting to merge" << std::endl;
				std::vector<int> P = pvalues[tt[i]]; //TODO not efficient ^ copying
				logfile << "p.back = " << P.back() << std::endl;
				std::vector<int> SUM( P.back()+1 , 0);
				logfile << "printing sum vector after initialisation" << std::endl;
				for (int i = 0; i != SUM.size(); i++) logfile << SUM[i] << std::endl; 
				std::vector<std::pair<int,std::pair<int,int>>>::iterator current = M[tt[i]].begin();
				std::vector<std::pair<int,std::pair<int,int>>>::iterator null =  M[tt[i]].end();
				bool reported = false;
				while ( current != null )
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
					if (next!=null)
					{
						next_p = (*next).second.first;
						logfile << "next p = " << next_p << std::endl;
						next_e = (*next).second.second;
						logfile << "next e = " << next_e << std::endl;
					}
					else
					{
						next_p = -10;
						next_e = -10;
					}
					SUM[current_p] += parikh[current_j];
					logfile << "printing sum vector after updating inside while loop" << std::endl;
					for (int i = 0; i != SUM.size(); i++) logfile << SUM[i] << std::endl; 
					if (current_e==1)
					{
						logfile << "(SUM[current_p]+occ) = " << (SUM[current_p]+occ) << std::endl;
						if ((SUM[current_p]+occ) >= k)
						{
							logfile << "pattern reported "
							<< getPattern(i, current_p, tt, M[tt[i]], t, m, parikh, logfile)
							<< std::endl;
							//ivec.push_back(i); pvec.push_back(getPattern(i, current_p, tt, M[tt[i]], t, m, parikh, logfile));
							window_result = std::make_pair(i,getPattern(i, current_p, tt, M[tt[i]], t, m, parikh, logfile));
							result.push_back(window_result);
							reported = true;
						} //END_IF
					}
					else if ( (current_p != next_p) && (current_e > 1) )
					{
						logfile << "e is more than 1" << std::endl;
						int sum = SUM[current_p];
						logfile << "before loop sum is " << sum << std::endl;
						for ( std::vector<int>::iterator p = P.begin(); p != P.end(); p++ )
						{
							if ( current_p % (*p) == 0 )
							{
								logfile << "adding " << SUM[(*p)] << " to sum" << std::endl;
								sum += SUM[(*p)];
							} //END_IF
						} //END_FOR
						logfile << "after loop sum is " << sum << std::endl;
						if ( sum >= k )
						{
							logfile << "pattern reported "
							<< getPattern(i, current_p, tt, M[tt[i]], t, m, parikh, logfile)
							<< std::endl;
							//ivec.push_back(i); pvec.push_back(getPattern(i, current_p, tt, M[tt[i]], t, m, parikh, logfile));
							window_result = std::make_pair(i,getPattern(i, current_p, tt, M[tt[i]], t, m, parikh, logfile));
							result.push_back(window_result);
							reported = true;
						} //END_IF( sum >= k )
					} //END_ELSEIF( (current_p != next_p) && (current_e > 1) )
					if ( (current_e != next_e) && (reported==true))
					{
						logfile << "setting current to null" << std::endl;
						current = null;
					} else {
						logfile << "incrementing current pointer" << std::endl;
						current++;
					} //END_IF( (current_e != next_e) && (reported==true))
				} //END_WHILE( current != null )
			} //END_IF( M[t'[i]] is not empty )
		} //END_IF( parikh[tt[i]] >= k )
		} //END_IF(( tt[i] != -1))
} //END_FOR(each letter in X)


/* end program */
const time_t END = time(0);
std::cout << asctime(localtime(&END));
} //END_CLUMPS

/*
http://stackoverflow.com/questions/279854/how-do-i-sort-a-vector-of-pairs-based-on-the-second-element-of-the-pair
std::sort(v.begin(), v.end(), [](const std::pair<int,int> &left, const std::pair<int,int> &right) {
    return left.second < right.second;
});
*/






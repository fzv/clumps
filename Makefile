all:
	g++ -std=c++11 -O3 -msse4.2 -fomit-frame-pointer -funroll-loops -DNDEBUG -I ~/include -L ~/lib clumps.cpp -o clumps -lsdsl -ldivsufsort -ldivsufso
rt64

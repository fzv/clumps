all:
	g++ -std=c++11 -O3 -msse4.2 -fomit-frame-pointer -funroll-loops -DNDEBUG -I ~/include -L ~/lib run_clumps.cpp clumps.cpp pre.cpp step_one.cpp step_two.cpp step_three.cpp clumps.h -o clumps -lsdsl -ldivsufsort -ldivsufsort64

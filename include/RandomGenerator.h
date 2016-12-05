#pragma once

#include <time.h>
#include <string>

// A simple random number generator that is used as a function object. 
// Returns a new random value every time operator() is called.
class RandomGenerator
{
public:
	unsigned long int operator()();
	unsigned long int operator()(unsigned int val);
	
	//generates a name from a set of predefined names
	std::string name();
private:
	static unsigned long int randIndex;
	
	static const int NAME_COUNT = 20;
	std::string names[NAME_COUNT] = { "Brian", "Jack", "Tom", "Tim", "Wally", "Murphy", "Maggie", "Ivar", 
			"Abe", "Albert", "Alfie", "Baldric", "Barabara", "Charlie", "Dave", "Ellie", "Gordon",
			"Greg" ,"Carly Alfredo", "Hari Puttir" };
};
// A simple random number generator that is primarily used as a function object. 
// Returns a new random value every time operator() is called.
// Created by Torsten Gustafsson aug 2016.
// Some additional functionality have been added as needed.

#ifndef RANDOMGENERATOR
#define RANDOMGENERATOR

#include <random>
#include <time.h>
#include <climits>

class RandomGenerator
{
public:

	RandomGenerator() : rng{rd()} {}

	//Returns a random positive integer value 
	unsigned long int operator()()
	{
		std::uniform_int_distribution<int> uni(0, INT_MAX);
		return uni(rng);
	}

	//Returns a random integer value between 0 and val
	unsigned long int operator()(unsigned int val)
	{
		std::uniform_int_distribution<int> uni(0, val);
		return uni(rng);
	}

	//Returns a random floating point value in the normal distribution specified by val and range.
	//For example, 'distribution(0.5, 0.1)' returns a random value between 0.4 and 0.6, and 'distribution(0.5)' 
	//returns a random value between 0.25 and 0.75 (+-50%).
	float distribution(float val, float range = 0)
	{
		range = range < 0.0 ? -range : range; //convert negative numbers
		range = range > val ? val : range; //range may not be larger than val
		const unsigned vals = 100000; // generate this many values in the distribution range
		std::uniform_int_distribution<int> uni( static_cast<int>( val * vals - (range == 0 ? val / 2 * vals : range * vals) ), 
										   static_cast<int>( val * vals + (range == 0 ? val / 2 * vals : range * vals) )
		);
		return uni(rng) / static_cast<float>(vals);
	}

	//Generates a simple unique id
	static unsigned long int unique();

private:
	std::random_device rd;
	std::mt19937 rng;
	static unsigned long int unique_id;
};

#endif
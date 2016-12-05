#include "RandomGenerator.h"

unsigned long int RandomGenerator::randIndex = 0;
/*
RandomGenerator::RandomGenerator()
{
	//Generate new random seed for every instance to hide patterns
	randIndex = randIndex + time(NULL);
}
*/

unsigned long int RandomGenerator::operator()()
{
	randIndex = (randIndex * 500041 + time(NULL)) % 542081;
	return randIndex;
}

unsigned long int RandomGenerator::operator()(unsigned int val)
{
	if(val <= 0) return 0;
	randIndex = (randIndex * 500041 + time(NULL)) % 542081;
	return randIndex % val;
}

std::string RandomGenerator::name()
{
	return names[(*this)() % NAME_COUNT];
}
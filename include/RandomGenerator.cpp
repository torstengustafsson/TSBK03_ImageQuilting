#include "RandomGenerator.h"

unsigned long int RandomGenerator::unique_id = 0;

unsigned long int RandomGenerator::unique()
{
	return unique_id++;
}
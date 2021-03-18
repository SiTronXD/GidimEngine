#include "Log.h"

void Log::Error(std::string errorMessage)
{
	std::cout << "--ERROR [ " << errorMessage << " ]" << std::endl;
}
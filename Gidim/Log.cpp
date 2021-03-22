#include "Log.h"
#include <Windows.h>

void Log::print(std::string message)
{
	std::cout << "-- " << message << std::endl;
}

void Log::error(std::string errorMessage)
{
	//std::cout << "--ERROR [ " << errorMessage << " ]" << std::endl;

	MessageBox(
		NULL, errorMessage.c_str(), "ERROR", MB_OK
	);
}

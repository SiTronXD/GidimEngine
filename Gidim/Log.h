#pragma once

#include <iostream>
#include <string>

class Log
{
public:
	static void print(std::string message);
	static void warning(std::string warningMessage);
	static void error(std::string errorMessage);
};
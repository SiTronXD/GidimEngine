#pragma once

#include <iostream>

class Log
{
public:
	static void print(std::string message);
	static void error(std::string errorMessage);
};
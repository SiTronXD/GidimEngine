#pragma once

#include <iostream>
#include <string>
#include <d3d11.h>

class Log
{
public:
	static void print(std::string message);
	static void warning(std::string warningMessage);
	static void error(std::string errorMessage);
	static void resultFailed(std::string errorMessage, HRESULT& result);
};
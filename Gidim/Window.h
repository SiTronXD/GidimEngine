#pragma once

#include <Windows.h>

#include "Input.h"
#include "Log.h"

class Window
{
private:
	Input input;

	HWND m_hwnd;

	std::string windowTitle;

	bool running;

	int windowWidth;
	int windowHeight;

	void handleFrame();

public:
	Window(int windowWidth, int windowHeight, std::string windowTitle);
	~Window();

	bool init();
	bool update();
	bool isRunning();
	bool release();

	// Events
	void onCreate();
	void onDestroy();

	LRESULT CALLBACK messageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};
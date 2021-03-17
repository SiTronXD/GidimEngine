#pragma once

#include <Windows.h>

class Window
{
protected:
	HWND m_hwnd;

	bool is_run;

public:
	Window();
	~Window();

	bool init();
	bool broadcast();
	bool isRun();
	bool release();

	// Events
	virtual void onCreate() = 0;
	virtual void onUpdate() = 0;
	virtual void onDestroy();
};
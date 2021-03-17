#include "Window.h"

Window* window = nullptr;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		window->onCreate();

		break;
	}

	case WM_DESTROY:
	{
		window->onDestroy();

		PostQuitMessage(0);

		break;
	}

	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return NULL;
}

Window::Window()
{
}

Window::~Window()
{
}

bool Window::init()
{
	WNDCLASSEX wc;
	wc.cbClsExtra = NULL;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = NULL;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = NULL;
	wc.lpszClassName = "MyWindowClass";
	wc.lpszMenuName = "";
	wc.style = NULL;
	wc.lpfnWndProc = &WndProc;

	// Class registration failed
	if (!RegisterClassEx(&wc))
		return false;

	if (!window)
		window = this;

	// Create handle
	this->m_hwnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW, 
		"MyWindowClass", "Gidim", 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
		1280, 720, 
		NULL, NULL, NULL, NULL
	);

	// Handle creation failed
	if (!this->m_hwnd)
		return false;

	// Display window
	ShowWindow(this->m_hwnd, SW_SHOW);
	UpdateWindow(this->m_hwnd);

	this->is_run = true;

	return true;
}

bool Window::broadcast()
{
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	window->onUpdate();

	Sleep(0);

	return true;
}

bool Window::isRun()
{
	return is_run;
}

bool Window::release()
{
	if (this->m_hwnd)
	{
		if (DestroyWindow(this->m_hwnd))
		{
			return false;
		}
	}

	return true;
}

void Window::onDestroy()
{
	this->is_run = false;
}
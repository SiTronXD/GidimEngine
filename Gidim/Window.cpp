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
		return window->messageHandler(hwnd, msg, wparam, lparam);
	}

	return NULL;
}

void Window::handleFrame()
{
	// Exit if the escape button is pressed
	if (Input::isKeyDown(KEYS::ESCAPE))
		this->running = false;
}

Window::Window(int windowWidth, int windowHeight, std::string windowTitle)
	: handle(nullptr), running(false), windowWidth(windowWidth), windowHeight(windowHeight),
	windowTitle(windowTitle)
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
	{
		Log::Error("Window class registration failed.");

		return false;
	}

	if (!window)
		window = this;

	DEVMODE dmScreenSettings;
	DWORD displayStyle = WS_OVERLAPPEDWINDOW;
	int windowPosX = 0;
	int windowPosY = 0;
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	bool fullscreen = false;

	// Check for fullscreen (will be refactored for easier use later)
	if (fullscreen)
	{
		displayStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;

		this->windowWidth = screenWidth;
		this->windowHeight = screenHeight;

		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long) this->windowWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long) this->windowHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Apply settings to fullscreen
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		windowPosX = (screenWidth - windowWidth) / 2;
		windowPosY = (screenHeight - windowHeight) / 2;
	}


	// Create handle
	this->handle = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW, "MyWindowClass", windowTitle.c_str(), 
		displayStyle,
		windowPosX, windowPosY,
		windowWidth, windowHeight, 
		NULL, NULL, NULL, NULL
	);

	// Handle creation failed
	if (!this->handle)
	{
		Log::Error("Handle creation failed.");

		return false;
	}

	// Display window
	ShowWindow(this->handle, SW_SHOW);
	SetForegroundWindow(this->handle);
	SetFocus(this->handle);
	UpdateWindow(this->handle);

	this->running = true;

	return true;
}

bool Window::update()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
		{
			this->running = false;
		}
	}


	if (this->running)
		this->handleFrame();


	//Sleep(0);

	return true;
}

bool Window::isRunning()
{
	return this->running;
}

bool Window::release()
{
	// Handle
	if (this->handle)
		DestroyWindow(this->handle);

	return true;
}

void Window::onCreate()
{
}

void Window::onDestroy()
{
	this->running = false;
}

// Handle messages from window, inside this class
LRESULT Window::messageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
		// Key was pressed
		case WM_KEYDOWN:
		{
			this->input.setKeyDown((unsigned int) wparam);

			break;
		}

		// Key was released
		case WM_KEYUP:
		{
			this->input.setKeyDown((unsigned int) wparam);

			break;
		}

		default:
		{
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
	}

	return NULL;
}

const HWND& Window::getHandle() const
{
	return this->handle;
}

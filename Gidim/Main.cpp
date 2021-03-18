#include "Window.h"

int main()
{
	Window window(1280, 720, "Gidim");

	if (window.init())
	{
		while (window.isRunning())
		{
			window.update();
		}
	}

	window.release();

	return 0;
}
#include "AppWindow.h"

int main()
{
	AppWindow app;
	if (app.init())
	{
		while (app.isRun())
		{
			app.broadcast();
		}
	}

	app.release();

	return 0;
}
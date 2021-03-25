#include <crtdbg.h>
#include "WaterRendering.h"

int main()
{
	// Set flags for tracking memory leaks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Water rendering example
	WaterRendering wr;
	wr.run();

	return 0;
}
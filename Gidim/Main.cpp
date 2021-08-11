#include <crtdbg.h>
#include "Examples/WaterRendering.h"
#include "Examples/BoidsExample.h"

int main()
{
	// Set flags for tracking memory leaks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Water rendering example
	/*WaterRendering wr;
	wr.run();*/

	// Boids AI logic and rendering
	BoidsExample be;
	be.run();

	return 0;
}
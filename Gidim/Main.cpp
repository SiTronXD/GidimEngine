#include "Window.h"
#include "Renderer.h"
#include "Mesh.h"

int main()
{
	// Set flags for tracking memory leaks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Create window
	Window window(1280, 720, "Gidim");
	if (!window.init())
	{
		Log::Error("Could not initialize window.");
	}

	Renderer renderer(window);
	Mesh mesh(renderer);

	// Main game loop
	while (window.isRunning())
	{
		// Handle windows events
		window.update();

		// Prepare for rendering frame
		renderer.beginFrame();

		// Clear background
		float clearColor[]{ 0.1f, 0.1f, 0.1f, 1.0f };
		renderer.clear(clearColor);

		// Draw!
		mesh.draw();

		// Present frame
		renderer.endFrame();
	}

	// Destroy window
	window.release();

	return 0;
}
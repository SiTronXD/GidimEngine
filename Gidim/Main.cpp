#include "Window.h"
#include "Renderer.h"
#include "Mesh.h"
#include "Camera.h"
#include "Time.h"

int main()
{
	// Set flags for tracking memory leaks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// Create window
	int windowWidth = 1280;
	int windowHeight = 720;
	Window window(windowWidth, windowHeight, "Gidim");
	if (!window.init())
	{
		Log::error("Could not initialize window.");
	}

	Renderer renderer(window);
	Mesh mesh(renderer);
	Camera camera(3.14f * 0.5f, (float) windowWidth / (float) windowHeight, 0.1f, 1000.0f);
	Time time;

	renderer.setCamera(camera);

	float movingTimer = 0.0f;

	// Main game loop
	while (window.isRunning())
	{
		// Update delta time
		time.updateDeltaTime();

		// Move camera
		movingTimer += Time::getDeltaTime() * 3.14f * 2.0f;
		camera.setPosition(XMVectorSet(sin(movingTimer) * 3.0f, 0.0f, cos(movingTimer) * -3.0f, 1.0f));

		// Print fps once every second
		if (Time::hasOneSecondPassed())
		{
			int fps = Time::getFramesPerSecond();

			Log::print("FPS: " + std::to_string(fps) + " (" + std::to_string(1000.0f / fps) + " ms)");
		}

		/////////////////////////////////////////////////////////////////////////

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
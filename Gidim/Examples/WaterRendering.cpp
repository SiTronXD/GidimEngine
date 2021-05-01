#include "WaterRendering.h"
#include "../Engine/Application/Window.h"
#include "../Engine/Graphics/Renderer.h"
#include "../Engine/Graphics/Texture.h"
#include "../Engine/Graphics/Mesh.h"
#include "../Engine/Graphics/ComputeShader.h"
#include "../Engine/Application/Time.h"
#include "../ShaderHandlers/SkyboxShader.h"
#include "../Examples/CameraController.h"
#include "Skybox.h"
#include "Water.h"

WaterRendering::WaterRendering()
{ }

WaterRendering::~WaterRendering()
{ }

void WaterRendering::run()
{
	// Create window
	int windowWidth = 1280;
	int windowHeight = 720;
	Window window(windowWidth, windowHeight, "Gidim");
	if (!window.init())
	{
		Log::error("Could not initialize window.");
	}

	Renderer renderer(window);
	CameraController cameraController(windowWidth, windowHeight);
	renderer.setCamera(cameraController.getCamera());
	Time time;

	Skybox skybox(renderer);
	Water water(renderer);


	// Update once before starting loop
	window.update();

	bool isWireframe = false;

	// Main game loop
	while (window.isRunning())
	{
		// Handle windows events
		window.update();

		// Update delta time
		time.updateDeltaTime();

		/////////////////////////////////////////////////////////////////////////
		// GAME LOGIC

		cameraController.update();

		// Print fps once every second
		if (Time::hasOneSecondPassed())
		{
			int fps = Time::getFramesPerSecond();

			Log::print("FPS: " + std::to_string(fps) + " (" + std::to_string(1000.0f / fps) + " ms)");
		}

		if (Input::isKeyJustPressed(Keys::R))
		{
			isWireframe = !isWireframe;

			renderer.setWireframe(isWireframe);
		}

		if (Input::isKeyJustPressed(Keys::T))
		{
			water.toggleHorizontalDisplacement();
		}

		/////////////////////////////////////////////////////////////////////////

		// Prepare for rendering frame
		renderer.beginFrame();

		// Clear background color
		XMFLOAT4 clearColor;
		clearColor.x = 0.1f;
		clearColor.y = 0.1f;
		clearColor.z = 0.1f;
		clearColor.w = 1.0f;

		// Clear color and depth buffers
		renderer.clear(clearColor);

		// Render meshes
		skybox.draw();
		water.draw();

		// Present frame
		renderer.endFrame();
	}

	// Destroy window
	window.release();
}
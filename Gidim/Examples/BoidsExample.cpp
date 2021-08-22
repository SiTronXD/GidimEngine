#include "../pch.h"
#include "BoidsExample.h"
#include "../Engine/Application/Window.h"
#include "../Engine/Graphics/Renderer.h"
#include "../Engine/Graphics/Texture.h"
#include "../Engine/Graphics/Mesh.h"
#include "../Engine/Graphics/ComputeShader.h"
#include "../Engine/Application/Time.h"
#include "../ShaderHandlers/SkyboxShader.h"
#include "GameObjects/CameraController.h"
#include "GameObjects/Skybox.h"
#include "GameObjects/BoidHandler.h"

BoidsExample::BoidsExample()
{
}

BoidsExample::~BoidsExample()
{
}

void BoidsExample::run()
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

	// Update once before starting loop
	window.update();

	BoidHandler boidHandler(renderer);

	// Main game loop
	while (window.isRunning())
	{
		// Handle windows events
		window.update();

		// Update delta time
		time.updateDeltaTime();

		/////////////////////////////////////////////////////////////////////////
		// GAME LOGIC

		if(window.isFocus())
			cameraController.update();

		// Print fps once every second
		if (Time::hasOneSecondPassed())
		{
			int fps = Time::getFramesPerSecond();

			Log::print("FPS: " + std::to_string(fps) + " (" + std::to_string(1000.0f / fps) + " ms)");
		}

		boidHandler.updateBoids(Time::getDeltaTime());

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
		//skybox.draw();
		boidHandler.drawBoids();

		// Present frame
		renderer.endFrame();
	}

	// Destroy window
	window.release();
}

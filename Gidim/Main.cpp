#include "Window.h"
#include "Renderer.h"
#include "Mesh.h"
#include "CameraController.h"
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
	CameraController cameraController(windowWidth, windowHeight);
	renderer.setCamera(cameraController.getCamera());
	Time time;

	float p1[]{ -0.866f,  0.0f,  0.866f };
	float p2[]{  0.0f,  0.0f, -1.0f };
	float p3[]{ 0.866f,  0.0f,  0.866f };

	// Create mesh
	Vertex vertices[] =
	{
		{ -0.866f,  0.0f,	 0.5f,		0.0f, 1.0f },
		{    0.0f,  0.0f,	-1.0f,		0.5f, 0.0f },
		{  0.866f,  0.0f,	 0.5f,		1.0f, 1.0f },

		{    0.0f,  0.0f,	-1.0f,		1.0f, 1.0f },
		{ -0.866f,  0.0f,	 0.5f,		0.0f, 1.0f },
		{    0.0f,  1.5f,	 0.0f,		0.5f, 0.0f },

		{  0.866f,  0.0f,	  0.5f,		1.0f, 1.0f },
		{    0.0f,  0.0f,	-1.0f,		0.0f, 1.0f },
		{    0.0f,  1.5f,	 0.0f,		0.5f, 0.0f },

		{ -0.866f,  0.0f,	  0.5f,		1.0f, 1.0f },
		{  0.866f,  0.0f,	 0.5f,		0.0f, 1.0f },
		{    0.0f,  1.5f,	 0.0f,		0.5f, 0.0f }
	};
	int indices[]
	{ 
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11
	};
	int numVertices = sizeof(vertices) / sizeof(vertices[0]);
	int numIndices = sizeof(indices) / sizeof(indices[0]);

	Mesh mesh(renderer, vertices, indices, numVertices, numIndices);
	Texture texture;
	texture.loadFromFile(
		renderer.getDevice(),
		"Resources/Textures/myMan.png"
	);


	// Update once before starting loop
	window.update();

	float timer = 0.0f;

	// Main game loop
	while (window.isRunning())
	{
		// Handle windows events
		window.update();

		// Update delta time
		time.updateDeltaTime();

		/////////////////////////////////////////////////////////////////////////
		// GAME LOGIC

		timer += Time::getDeltaTime();

		cameraController.update();

		// Print fps once every second
		if (Time::hasOneSecondPassed())
		{
			int fps = Time::getFramesPerSecond();

			Log::print("FPS: " + std::to_string(fps) + " (" + std::to_string(1000.0f / fps) + " ms)");
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

		renderer.clear(clearColor);

		// Set texture
		texture.set(renderer.getDeviceContext());

		// Draw!
		XMMATRIX currentWorldMatrix = XMMatrixRotationY(timer) * XMMatrixTranslation(1.5f, 0.0f, 0.0f);
		mesh.setWorldMatrix(currentWorldMatrix);
		mesh.draw();

		currentWorldMatrix = XMMatrixRotationY(timer + 0.4) * XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		mesh.setWorldMatrix(currentWorldMatrix);
		mesh.draw();

		currentWorldMatrix = XMMatrixRotationY(timer + 0.4 * 2.0) * XMMatrixTranslation(-1.5f, 0.0f, 0.0f);
		mesh.setWorldMatrix(currentWorldMatrix);
		mesh.draw();


		// Present frame
		renderer.endFrame();
	}

	// Destroy window
	window.release();

	return 0;
}
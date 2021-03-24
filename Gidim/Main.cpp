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


	// Create mesh
	MeshData md; 
	md.createDefault(DefaultMesh::SPHERE, 100, 100);
	//md.invertFaces();

	Log::print("Mesh vertices: " + std::to_string(md.getVertices().size()));
	Log::print("Mesh num triangles: " + std::to_string(md.getIndices().size() / 3));

	Mesh mesh(renderer, md);

	// Load texture
	Texture texture(renderer);
	texture.loadFromFile(
		renderer.getDevice(),
		"Resources/Textures/poggers.dds"
	);


	// Update once before starting loop
	window.update();

	bool isWireframe = false;
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

		if (Input::isKeyJustPressed(Keys::R))
		{
			isWireframe = !isWireframe;

			renderer.setWireframe(isWireframe);
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
		texture.set(renderer.getDeviceContext(), 0);

		// Draw!
		XMMATRIX currentWorldMatrix = XMMatrixRotationY(timer) * XMMatrixScaling(2.0f, 2.0f, 2.0f);
		mesh.setWorldMatrix(currentWorldMatrix);
		mesh.draw();

		/*currentWorldMatrix = XMMatrixRotationY(timer + 0.4) * XMMatrixTranslation(1.5f, 0.0f, 0.0f);
		mesh.setWorldMatrix(currentWorldMatrix);
		mesh.draw();

		currentWorldMatrix = XMMatrixRotationY(timer + 0.4 * 2.0) * XMMatrixTranslation(-1.5f, 0.0f, 0.0f);
		mesh.setWorldMatrix(currentWorldMatrix);
		mesh.draw();*/


		// Present frame
		renderer.endFrame();
	}

	// Destroy window
	window.release();

	return 0;
}
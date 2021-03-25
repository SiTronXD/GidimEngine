#include "WaterRendering.h"
#include "Window.h"
#include "Renderer.h"
#include "Texture.h"
#include "Mesh.h"
#include "CameraController.h"
#include "Time.h"

#include "SkyboxShader.h"
#include "ColorShader.h"

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


	// Create mesh
	MeshData skyboxMeshData;
	skyboxMeshData.createDefault(DefaultMesh::SPHERE, 20, 20);
	skyboxMeshData.invertFaces();
	Mesh skyboxMesh(renderer, skyboxMeshData);
	skyboxMesh.setWorldMatrix(XMMatrixRotationY(3.1415) * XMMatrixScaling(4.0f, 4.0f, 4.0f));

	MeshData waterMeshData;
	waterMeshData.createDefault(DefaultMesh::PLANE, 64, 64);
	Mesh waterMesh(renderer, waterMeshData);

	// Create shader for rendering meshes
	SkyboxShader skyboxShader(renderer);
	ColorShader waterShader(renderer);
	

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
		//texture.set(renderer.getDeviceContext(), 0);

		// Update buffers in shader
		skyboxShader.update(renderer, skyboxMesh.getWorldMatrix());
		waterShader.update(renderer, waterMesh.getWorldMatrix());

		// Set shader to render mesh with
		skyboxShader.set(renderer.getDeviceContext());
		skyboxMesh.draw();

		// Set shader to render mesh with
		waterShader.set(renderer.getDeviceContext());
		waterMesh.draw();

		// Present frame
		renderer.endFrame();
	}

	// Destroy window
	window.release();
}
#include "WaterRendering.h"
#include "Window.h"
#include "Renderer.h"
#include "Texture.h"
#include "Mesh.h"
#include "CameraController.h"
#include "Time.h"
#include "ComputeShader.h"

#include "SkyboxShader.h"
#include "ColorShader.h"
#include "TextureShader.h"

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


	// Create skybox mesh
	MeshData skyboxMeshData;
	skyboxMeshData.createDefault(DefaultMesh::SPHERE, 20, 20);
	skyboxMeshData.invertFaces();
	Mesh skyboxMesh(renderer, skyboxMeshData);
	skyboxMesh.setWorldMatrix(XMMatrixRotationY(3.1415) * XMMatrixScaling(4.0f, 4.0f, 4.0f));

	// Create water mesh
	MeshData waterMeshData;
	waterMeshData.createDefault(DefaultMesh::PLANE, 64, 64);
	Mesh waterMesh(renderer, waterMeshData);
	waterMesh.setWorldMatrix(XMMatrixScaling(3.0f, 3.0f, 3.0f));

	// Create shader for rendering meshes
	SkyboxShader skyboxShader(renderer);
	TextureShader waterShader(renderer);
	

	// Water rendering shaders
	ComputeShader spectrumCreatorShader(16, 16);
	ComputeShader spectrumInterpolatorShader(16, 16);

	// Initial spectrum textures
	Texture spectrumTexture0(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R16G16B16A16_FLOAT);
	Texture spectrumTexture1(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R16G16B16A16_FLOAT);
	Texture finalSpectrumTexture(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R16G16B16A16_FLOAT);
	spectrumTexture0.createAsRenderTexture(renderer, 256, 256);
	spectrumTexture1.createAsRenderTexture(renderer, 256, 256);
	finalSpectrumTexture.createAsRenderTexture(renderer, 256, 256);

	// Initial spectrum texture creator shader
	spectrumCreatorShader.createFromFile(renderer, "SpectrumCreatorShader_Comp.cso");
	spectrumCreatorShader.addRenderTexture(spectrumTexture0);
	spectrumCreatorShader.addRenderTexture(spectrumTexture1);
	spectrumCreatorShader.run(renderer);

	// Spectrum interpolation shader
	spectrumInterpolatorShader.createFromFile(renderer, "SpectrumInterpolatorShader_Comp.cso");
	spectrumInterpolatorShader.addRenderTexture(spectrumTexture0);
	spectrumInterpolatorShader.addRenderTexture(spectrumTexture1);
	spectrumInterpolatorShader.addRenderTexture(finalSpectrumTexture);
	spectrumInterpolatorShader.run(renderer);

	finalSpectrumTexture.set(renderer);


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

		// Clear color and depth buffers
		renderer.clear(clearColor);

		// Update buffers in shader
		//skyboxShader.update(renderer, skyboxMesh.getWorldMatrix());
		waterShader.update(renderer, waterMesh.getWorldMatrix());

		// Set shader to render mesh with
		//skyboxShader.set(renderer.getDeviceContext());
		//skyboxMesh.draw();


		// Set shader to render mesh with
		waterShader.set(renderer.getDeviceContext());
		waterMesh.draw();


		// Present frame
		renderer.endFrame();
	}

	// Destroy window
	window.release();
}
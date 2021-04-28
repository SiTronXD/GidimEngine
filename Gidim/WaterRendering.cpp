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

struct SpectrumInterpolationBuffer
{
	float time;
	float padding1;
	float padding2;
	float padding3;
};

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
	ComputeShader butterflyTextureShader(4, 16);
	ComputeShader butterflyOperationsShader(16, 16);

	// Water rendering textures
	Texture spectrumTexture0(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R16G16B16A16_FLOAT);
	Texture spectrumTexture1(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R16G16B16A16_FLOAT);
	Texture finalSpectrumTexture(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R16G16B16A16_FLOAT);
	Texture butterflyTexture(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R16G16B16A16_FLOAT);
	Texture pingPongTexture0(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R16G16B16A16_FLOAT);
	Texture pingPongTexture1(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R16G16B16A16_FLOAT);
	spectrumTexture0.createAsRenderTexture(256, 256);
	spectrumTexture1.createAsRenderTexture(256, 256);
	finalSpectrumTexture.createAsRenderTexture(256, 256);
	butterflyTexture.createAsRenderTexture(8, 256);
	pingPongTexture0.createAsRenderTexture(256, 256);
	pingPongTexture1.createAsRenderTexture(256, 256);
	//pingPongTexture0.clear(renderer, 1.0f, 1.0f, 0.0f, 1.0f);

	// Initial spectrum texture creator shader
	spectrumCreatorShader.createFromFile(renderer, "SpectrumCreatorShader_Comp.cso");
	spectrumCreatorShader.addRenderTexture(spectrumTexture0);
	spectrumCreatorShader.addRenderTexture(spectrumTexture1);
	spectrumCreatorShader.run(renderer);

	// Constant buffer to communicate with the spectrum interpolator shader
	SDXBuffer spectrumInterpolationShaderBuffer(renderer.getDevice(), sizeof(SpectrumInterpolationBuffer));

	// Spectrum interpolator shader
	spectrumInterpolatorShader.createFromFile(renderer, "SpectrumInterpolatorShader_Comp.cso");
	spectrumInterpolatorShader.addRenderTexture(spectrumTexture0);
	spectrumInterpolatorShader.addRenderTexture(spectrumTexture1);
	spectrumInterpolatorShader.addRenderTexture(finalSpectrumTexture);
	spectrumInterpolatorShader.addConstantBuffer(spectrumInterpolationShaderBuffer);

	// Butterfly texture shader
	butterflyTextureShader.createFromFile(renderer, "ButterflyTextureShader_Comp.cso");
	butterflyTextureShader.addRenderTexture(butterflyTexture);
	butterflyTextureShader.run(renderer);

	// Butterfly operations shader
	butterflyOperationsShader.createFromFile(renderer, "ButterflyOperationsShader_Comp.cso");
	butterflyOperationsShader.addRenderTexture(pingPongTexture0);
	butterflyOperationsShader.addRenderTexture(pingPongTexture1);

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

		// Update spectrum interpolation shader
		D3D11_MAPPED_SUBRESOURCE mappedRes;
		spectrumInterpolationShaderBuffer.map(renderer.getDeviceContext(), mappedRes);
		SpectrumInterpolationBuffer* sib = (SpectrumInterpolationBuffer*)mappedRes.pData;
		sib->time = timer;
		spectrumInterpolationShaderBuffer.unmap(renderer.getDeviceContext());

		spectrumInterpolatorShader.run(renderer);

		// Update butterfly operations shader
		butterflyOperationsShader.run(renderer);

		pingPongTexture0.set();



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
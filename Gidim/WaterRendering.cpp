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

struct ButterflyOperationBuffer
{
	int stage;
	int pingPong;
	int direction;
	int padding1;
};

struct InvPermBuffer
{
	int pingPong;
	int padding1;
	int padding2;
	int padding3;
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
	ComputeShader invPermShader(16, 16);

	// Water rendering textures
	Texture spectrumTexture0(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R16G16B16A16_FLOAT);
	Texture spectrumTexture1(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R16G16B16A16_FLOAT);
	Texture finalSpectrumTexture(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R16G16B16A16_FLOAT);
	Texture butterflyTexture(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R16G16B16A16_FLOAT);
	Texture pingPongTexture(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R16G16B16A16_FLOAT);
	Texture displacementTexture(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R16G16B16A16_FLOAT);
	spectrumTexture0.createAsRenderTexture(256, 256);
	spectrumTexture1.createAsRenderTexture(256, 256);
	finalSpectrumTexture.createAsRenderTexture(256, 256);
	butterflyTexture.createAsRenderTexture(8, 256);
	pingPongTexture.createAsRenderTexture(256, 256);
	displacementTexture.createAsRenderTexture(256, 256);

	// Initial spectrum texture creator shader
	spectrumCreatorShader.createFromFile(renderer, "SpectrumCreatorShader_Comp.cso");
	spectrumCreatorShader.addRenderTexture(spectrumTexture0);
	spectrumCreatorShader.addRenderTexture(spectrumTexture1);
	spectrumCreatorShader.run(renderer);

	// Constant buffer to communicate with shaders
	SDXBuffer spectrumInterpolationShaderBuffer(renderer.getDevice(), sizeof(SpectrumInterpolationBuffer));
	SDXBuffer butterflyOperationShaderBuffer(renderer.getDevice(), sizeof(ButterflyOperationBuffer));
	SDXBuffer invPermShaderBuffer(renderer.getDevice(), sizeof(InvPermBuffer));

	// Spectrum interpolator shader
	spectrumInterpolatorShader.createFromFile(renderer, "SpectrumInterpolatorShader_Comp.cso");
	spectrumInterpolatorShader.addRenderTexture(spectrumTexture0);
	spectrumInterpolatorShader.addRenderTexture(spectrumTexture1);
	spectrumInterpolatorShader.addRenderTexture(finalSpectrumTexture);
	spectrumInterpolatorShader.addRenderTexture(pingPongTexture);
	spectrumInterpolatorShader.addConstantBuffer(spectrumInterpolationShaderBuffer);

	// Butterfly texture shader
	butterflyTextureShader.createFromFile(renderer, "ButterflyTextureShader_Comp.cso");
	butterflyTextureShader.addRenderTexture(butterflyTexture);
	butterflyTextureShader.run(renderer);

	// Butterfly operations shader
	butterflyOperationsShader.createFromFile(renderer, "ButterflyOperationsShader_Comp.cso");
	butterflyOperationsShader.addRenderTexture(butterflyTexture);
	butterflyOperationsShader.addRenderTexture(pingPongTexture);
	butterflyOperationsShader.addRenderTexture(finalSpectrumTexture);
	butterflyOperationsShader.addConstantBuffer(butterflyOperationShaderBuffer);

	// Inversion and permutation shader
	invPermShader.createFromFile(renderer, "InversionPermutationShader_Comp.cso");
	invPermShader.addRenderTexture(displacementTexture);
	invPermShader.addRenderTexture(pingPongTexture);
	invPermShader.addRenderTexture(finalSpectrumTexture);
	invPermShader.addConstantBuffer(invPermShaderBuffer);


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
		D3D11_MAPPED_SUBRESOURCE mappedSpecIntRes;
		spectrumInterpolationShaderBuffer.map(renderer.getDeviceContext(), mappedSpecIntRes);
		SpectrumInterpolationBuffer* sib = (SpectrumInterpolationBuffer*) mappedSpecIntRes.pData;
		sib->time = timer;
		sib->padding1 = 0.0f;
		sib->padding2 = 0.0f;
		sib->padding3 = 0.0f;
		spectrumInterpolationShaderBuffer.unmap(renderer.getDeviceContext());

		spectrumInterpolatorShader.run(renderer);

		// Update butterfly operations shader
		int pingPong = 0;
		D3D11_MAPPED_SUBRESOURCE mappedButterOpRes;
		for (int j = 0; j < 2; ++j)
		{
			for (int i = 0; i < 8; ++i)
			{
				// Update shader buffer
				butterflyOperationShaderBuffer.map(renderer.getDeviceContext(), mappedButterOpRes);
				ButterflyOperationBuffer* bob = (ButterflyOperationBuffer*)mappedButterOpRes.pData;

				bob->stage = i;
				bob->pingPong = pingPong;
				bob->direction = j;
				bob->padding1 = 0;

				butterflyOperationShaderBuffer.unmap(renderer.getDeviceContext());


				butterflyOperationsShader.run(renderer);
				pingPong = (pingPong + 1) % 2;
			}
		}

		// Inversion and permutation shader
		D3D11_MAPPED_SUBRESOURCE mappedInvPermRes;
		invPermShaderBuffer.map(renderer.getDeviceContext(), mappedInvPermRes);
		InvPermBuffer* ipb = (InvPermBuffer*) mappedInvPermRes.pData;
		ipb->pingPong = pingPong;
		ipb->padding1 = 0;
		ipb->padding2 = 0;
		ipb->padding3 = 0;
		invPermShaderBuffer.unmap(renderer.getDeviceContext());

		invPermShader.run(renderer);

		// Debug displacementTexture
		displacementTexture.set();



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
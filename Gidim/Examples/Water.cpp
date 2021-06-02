#include "Water.h"
#include "../Engine/Application/Time.h"

const int Water::GRID_WIDTH = 256;
const int Water::GRID_HEIGHT = 256;
const float Water::WIND_DIR_X = 1.0f;
const float Water::WIND_DIR_Y = 1.0f;
const float Water::HORIZONTAL_SIZE = 1000.0f * 0.85f;
const float Water::WIND_SPEED = 30.0f;
const float Water::AMPLITUDE = 3.0f;

const int Water::NORMAL_MAP_SIZE_SCALE = 1;

Water::Water(Renderer& renderer)
	: renderer(renderer), meshData(DefaultMesh::PLANE, GRID_WIDTH, GRID_HEIGHT),
	mesh(renderer, meshData), meshShader(renderer), skybox(nullptr), reflectedCubeMap(nullptr),

	// Compute shaders
	spectrumCreatorShader(renderer, GRID_WIDTH / 16, GRID_HEIGHT / 16),
	spectrumInterpolatorShader(renderer, GRID_WIDTH / 16, GRID_HEIGHT / 16),
	butterflyTextureShader(renderer, (int) ceil(log2(GRID_WIDTH) / 2), GRID_HEIGHT / 16),
	butterflyOperationsShader(renderer, GRID_WIDTH / 16, GRID_HEIGHT / 16),
	invPermShader(renderer, GRID_WIDTH / 16, GRID_HEIGHT / 16),

	displacementToNormalShader(renderer, GRID_WIDTH / 16 * NORMAL_MAP_SIZE_SCALE, GRID_HEIGHT / 16 * NORMAL_MAP_SIZE_SCALE),
	foamMaskShader(renderer, GRID_WIDTH / 16, GRID_HEIGHT / 16),

	// Render textures
	initialSpectrumTexture(renderer, TextureFilter::NEAREST_NEIGHBOR, TextureFormat::R32G32B32A32_FLOAT),
	finalSpectrumTextureX(renderer, TextureFilter::NEAREST_NEIGHBOR, TextureFormat::R32G32B32A32_FLOAT),
	finalSpectrumTextureY(renderer, TextureFilter::NEAREST_NEIGHBOR, TextureFormat::R32G32B32A32_FLOAT),
	finalSpectrumTextureZ(renderer, TextureFilter::NEAREST_NEIGHBOR, TextureFormat::R32G32B32A32_FLOAT),
	butterflyTexture(renderer, TextureFilter::NEAREST_NEIGHBOR, TextureFormat::R32G32B32A32_FLOAT),
	displacementTexture(renderer, TextureFilter::NEAREST_NEIGHBOR, TextureFormat::R32G32B32A32_FLOAT, TextureEdgeSampling::REPEAT),

	normalMapTexture(renderer, TextureFilter::BILINEAR, TextureFormat::R16G16B16A16_UNORM, TextureEdgeSampling::REPEAT),
	foamMaskTexture(renderer, TextureFilter::BILINEAR, TextureFormat::R16G16B16A16_UNORM, TextureEdgeSampling::REPEAT),
	foamTexture(renderer, TextureFilter::BILINEAR, TextureFormat::R8G8B8A8_UNORM, TextureEdgeSampling::REPEAT),

	// Shader buffers
	spectrumCreatorShaderBuffer(renderer, sizeof(SpectrumCreatorBuffer)),
	spectrumInterpolationShaderBuffer(renderer, sizeof(SpectrumInterpolationBuffer)),
	butterflyTextureShaderBuffer(renderer, sizeof(ButterflyTextureBuffer)),
	butterflyOperationShaderBuffer(renderer, sizeof(ButterflyOperationBuffer)),
	invPermShaderBuffer(renderer, sizeof(InvPermBuffer)),

	disToNormShaderBuffer(renderer, sizeof(HeightToNormalBuffer)),
	foamMaskShaderBuffer(renderer, sizeof(FoamMaskBuffer)),

	waterShaderBuffer(renderer, sizeof(WaterBuffer)),

	numMultiplicationStages((int) log2(GRID_WIDTH)),
	timer(0.0f),
	displaceHorizontally(true),
	numPlaneRepetitions(7),
	planeLength(2.0)
{
	// Set textures as render textures
	this->initialSpectrumTexture.createAsRenderTexture(GRID_WIDTH, GRID_HEIGHT);
	this->finalSpectrumTextureX.createAsRenderTexture(GRID_WIDTH, GRID_HEIGHT);
	this->finalSpectrumTextureY.createAsRenderTexture(GRID_WIDTH, GRID_HEIGHT);
	this->finalSpectrumTextureZ.createAsRenderTexture(GRID_WIDTH, GRID_HEIGHT);
	this->butterflyTexture.createAsRenderTexture(this->numMultiplicationStages, GRID_HEIGHT);
	this->displacementTexture.createAsRenderTexture(GRID_WIDTH, GRID_HEIGHT);
	this->normalMapTexture.createAsRenderTexture(GRID_WIDTH * NORMAL_MAP_SIZE_SCALE, GRID_HEIGHT * NORMAL_MAP_SIZE_SCALE);
	this->foamMaskTexture.createAsRenderTexture(GRID_WIDTH, GRID_HEIGHT);

	this->foamTexture.createFromFile("Resources/Textures/OceanFoam.png");

	// Update spectrum creator constant buffer
	this->scb.gridWidth = GRID_WIDTH;
	this->scb.gridHeight = GRID_HEIGHT;
	this->scb.windDirection = XMFLOAT2(WIND_DIR_X, WIND_DIR_Y);
	this->scb.horizontalSize = HORIZONTAL_SIZE;
	this->scb.windSpeed = WIND_SPEED;
	this->scb.amplitude = AMPLITUDE;
	this->scb.waveDirectionTendency = 6.0f;
	this->spectrumCreatorShaderBuffer.update(&this->scb);

	// Spectrum texture creator shader
	this->spectrumCreatorShader.createFromFile("CompiledShaders/SpectrumCreatorShader_Comp.cso");
	this->spectrumCreatorShader.addRenderTexture(this->initialSpectrumTexture);
	this->spectrumCreatorShader.addShaderBuffer(this->spectrumCreatorShaderBuffer);
	this->spectrumCreatorShader.run();


	// Spectrum interpolator buffer parameters
	this->sib.gridWidth = GRID_WIDTH;
	this->sib.gridHeight = GRID_HEIGHT;
	this->sib.horizontalSize = HORIZONTAL_SIZE;

	// Spectrum interpolator shader
	this->spectrumInterpolatorShader.createFromFile("CompiledShaders/SpectrumInterpolatorShader_Comp.cso");
	this->spectrumInterpolatorShader.addRenderTexture(this->initialSpectrumTexture);
	this->spectrumInterpolatorShader.addRenderTexture(this->finalSpectrumTextureX);
	this->spectrumInterpolatorShader.addRenderTexture(this->finalSpectrumTextureY);
	this->spectrumInterpolatorShader.addRenderTexture(this->finalSpectrumTextureZ);
	this->spectrumInterpolatorShader.addShaderBuffer(this->spectrumInterpolationShaderBuffer);

	// Update butterfly texture shader constant buffer
	this->btb.gridSize = GRID_WIDTH;
	this->butterflyTextureShaderBuffer.update(&this->btb);

	// Butterfly texture shader
	this->butterflyTextureShader.createFromFile("CompiledShaders/ButterflyTextureShader_Comp.cso");
	this->butterflyTextureShader.addRenderTexture(this->butterflyTexture);
	this->butterflyTextureShader.addShaderBuffer(this->butterflyTextureShaderBuffer);
	this->butterflyTextureShader.run();

	// Butterfly operations shader
	this->butterflyOperationsShader.createFromFile("CompiledShaders/ButterflyOperationsShader_Comp.cso");
	this->butterflyOperationsShader.addRenderTexture(this->butterflyTexture);
	this->butterflyOperationsShader.addShaderBuffer(this->butterflyOperationShaderBuffer);

	// Update inversion and permutation shader buffer
	this->ipb.gridSize = GRID_WIDTH;
	this->invPermShaderBuffer.update(&this->ipb);

	// Inversion and permutation shader
	this->invPermShader.createFromFile("CompiledShaders/InversionPermutationShader_Comp.cso");
	this->invPermShader.addRenderTexture(this->finalSpectrumTextureX);
	this->invPermShader.addRenderTexture(this->finalSpectrumTextureY);
	this->invPermShader.addRenderTexture(this->finalSpectrumTextureZ);
	this->invPermShader.addRenderTexture(this->displacementTexture);
	this->invPermShader.addShaderBuffer(this->invPermShaderBuffer);

	// Update displacementmap to normalmap shader buffer
	this->htnb.gridWidth = GRID_WIDTH;
	this->htnb.gridHeight = GRID_HEIGHT;
	this->htnb.normalMapWidth = GRID_WIDTH * NORMAL_MAP_SIZE_SCALE;
	this->htnb.normalMapHeight = GRID_HEIGHT * NORMAL_MAP_SIZE_SCALE;
	this->htnb.unitLength = (float)GRID_WIDTH / (float)HORIZONTAL_SIZE * 0.8f;
	this->disToNormShaderBuffer.update(&this->htnb);

	// Heightmap to normal map shader
	this->displacementToNormalShader.createFromFile("CompiledShaders/HeightToNormal_Comp.cso");
	this->displacementToNormalShader.addRenderTexture(this->displacementTexture);
	this->displacementToNormalShader.addRenderTexture(this->normalMapTexture);
	this->displacementToNormalShader.addShaderBuffer(this->disToNormShaderBuffer);

	// Update foam mask buffer
	this->fmb.gridWidth = GRID_WIDTH;
	this->fmb.gridHeight = GRID_HEIGHT;
	this->foamMaskShaderBuffer.update(&this->fmb);

	// Foam mask shader
	this->foamMaskShader.createFromFile("CompiledShaders/FoamMaskCreator_Comp.cso");
	this->foamMaskShader.addRenderTexture(this->displacementTexture);
	this->foamMaskShader.addRenderTexture(this->foamMaskTexture);
	this->foamMaskShader.addShaderBuffer(this->foamMaskShaderBuffer);

	// Scale up plane
	this->mesh.setWorldMatrix(XMMatrixScaling(planeLength, planeLength, planeLength));
}

void Water::setSkybox(Skybox& skybox)
{
	this->skybox = &skybox;
	this->reflectedCubeMap = &skybox.getCubeMap();
}

void Water::toggleHorizontalDisplacement()
{
	this->displaceHorizontally = !this->displaceHorizontally;
}

void Water::runIFFT(Texture& currentSpectrumTexture)
{
	// Add textures to perform and store IFFT in
	this->butterflyOperationsShader.addRenderTexture(currentSpectrumTexture);

	// Update butterfly operations shader
	int pingPong = 0;
	for (int j = 0; j < 2; ++j)		// Horizontal and vertical
	{
		for (int i = 0; i < this->numMultiplicationStages; ++i)
		{
			// Update shader buffer
			this->bob.stage = i;
			this->bob.pingPong = pingPong;
			this->bob.direction = j;
			butterflyOperationShaderBuffer.update(&this->bob);

			// Run butterfly operation shader
			butterflyOperationsShader.run();
			pingPong = (pingPong + 1) % 2;
		}
	}

	// Remove added textures
	butterflyOperationsShader.removeRenderTextureAt(1);

	// Update ping pong
	this->ipb.pingPong = pingPong;
}

Water::~Water() {}

void Water::draw()
{
	// Animate
	this->timer += Time::getDeltaTime() * 3.0f;

	// Clear displacement textures
	this->displacementTexture.clearRenderTexture(0.0f, 0.0f, 0.0f, 1.0f);

	// Update spectrum interpolation shader
	this->sib.time = this->timer;
	spectrumInterpolationShaderBuffer.update(&this->sib);

	// Run spectrum interpolator shader
	spectrumInterpolatorShader.run();

	// FFTs
	this->runIFFT(finalSpectrumTextureY);
	if (this->displaceHorizontally)
	{
		this->runIFFT(finalSpectrumTextureX);
		this->runIFFT(finalSpectrumTextureZ);
	}

	// Run inversion and permutation constant buffer
	this->invPermShader.run();



	// Create normal map from displacement map
	this->displacementToNormalShader.run();

	// Create foam mask from displacement map
	this->foamMaskShader.run();

	// Set texture for vertex shader
	this->displacementTexture.setVS(0);

	// Set textures for pixel shader
	this->normalMapTexture.setPS(0);
	this->foamMaskTexture.setPS(1);
	this->foamTexture.setPS(2);
	this->reflectedCubeMap->setPS(3);

	// Update water pixel shader buffer
	XMFLOAT3 camPos = renderer.getCameraPosition();
	this->wb.cameraPosition = camPos;
	this->wb.sunDirection = this->skybox->getSunDir();
	this->waterShaderBuffer.update(&this->wb);

	// Set water pixel shader buffer
	this->waterShaderBuffer.setPS();

	// Set shader to render mesh with
	this->meshShader.set();

	// Repeat mesh
	for (int z = -this->numPlaneRepetitions / 2; z <= this->numPlaneRepetitions / 2; ++z)
	{
		for (int x = -this->numPlaneRepetitions / 2; x <= this->numPlaneRepetitions / 2; ++x)
		{
			// Update water shader matrices
			this->meshShader.update(renderer, this->mesh.getWorldMatrix() *
				XMMatrixTranslation(x * planeLength, 0.0, z * planeLength));

			// Draw
			this->mesh.draw();
		}
	}
}

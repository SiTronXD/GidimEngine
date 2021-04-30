#include "Water.h"
#include "Time.h"

const int Water::GRID_WIDTH = 256;
const int Water::GRID_HEIGHT = 256;
const float Water::WIND_DIR_X = 1.0f;
const float Water::WIND_DIR_Y = 1.0f;
const float Water::HORIZONTAL_SIZE = 1000.0f;
const float Water::WIND_SPEED = 40.0f;
const float Water::AMPLITUDE = 4.0f;

Water::Water(Renderer& renderer)
	: renderer(renderer), meshData(DefaultMesh::PLANE, GRID_WIDTH, GRID_HEIGHT),
	mesh(renderer, meshData), shader(renderer),

	// Compute shaders
	spectrumCreatorShader(GRID_WIDTH / 16, GRID_HEIGHT / 16),
	spectrumInterpolatorShader(GRID_WIDTH / 16, GRID_HEIGHT / 16),
	butterflyTextureShader(log2(GRID_WIDTH) / 2, GRID_HEIGHT / 16),
	butterflyOperationsShader(GRID_WIDTH / 16, GRID_HEIGHT / 16),
	invPermShader(GRID_WIDTH / 16, GRID_HEIGHT / 16),

	// Render textures
	initialSpectrumTexture(renderer, TextureFilter::NEAREST_NEIGHBOR, TextureFormat::R32G32B32A32_FLOAT),
	finalSpectrumTextureX(renderer, TextureFilter::NEAREST_NEIGHBOR, TextureFormat::R32G32B32A32_FLOAT),
	finalSpectrumTextureY(renderer, TextureFilter::NEAREST_NEIGHBOR, TextureFormat::R32G32B32A32_FLOAT),
	finalSpectrumTextureZ(renderer, TextureFilter::NEAREST_NEIGHBOR, TextureFormat::R32G32B32A32_FLOAT),
	butterflyTexture(renderer, TextureFilter::NEAREST_NEIGHBOR, TextureFormat::R32G32B32A32_FLOAT),
	displacementTexture(renderer, TextureFilter::NEAREST_NEIGHBOR, TextureFormat::R32G32B32A32_FLOAT),

	// Shader buffers
	spectrumCreatorShaderBuffer(renderer, sizeof(SpectrumCreatorBuffer)),
	spectrumInterpolationShaderBuffer(renderer, sizeof(SpectrumInterpolationBuffer)),
	butterflyTextureShaderBuffer(renderer, sizeof(ButterflyTextureBuffer)),
	butterflyOperationShaderBuffer(renderer, sizeof(ButterflyOperationBuffer)),
	invPermShaderBuffer(renderer, sizeof(InvPermBuffer)),

	numMultiplicationStages(log2(GRID_WIDTH)),
	timer(0.0f),
	displaceHorizontally(true)
{
	// Set textures as render textures
	this->initialSpectrumTexture.createAsRenderTexture(GRID_WIDTH, GRID_HEIGHT);
	this->finalSpectrumTextureX.createAsRenderTexture(GRID_WIDTH, GRID_HEIGHT);
	this->finalSpectrumTextureY.createAsRenderTexture(GRID_WIDTH, GRID_HEIGHT);
	this->finalSpectrumTextureZ.createAsRenderTexture(GRID_WIDTH, GRID_HEIGHT);
	this->butterflyTexture.createAsRenderTexture(this->numMultiplicationStages, GRID_HEIGHT);
	this->displacementTexture.createAsRenderTexture(GRID_WIDTH, GRID_HEIGHT);

	// Update spectrum creator constant buffer
	this->scb.gridWidth = GRID_WIDTH;
	this->scb.gridHeight = GRID_HEIGHT;
	this->scb.windDirection[0] = WIND_DIR_X;
	this->scb.windDirection[1] = WIND_DIR_Y;
	this->scb.horizontalSize = HORIZONTAL_SIZE;
	this->scb.windSpeed = WIND_SPEED;
	this->scb.amplitude = AMPLITUDE;
	this->scb.waveDirectionTendency = 6.0f;
	this->spectrumCreatorShaderBuffer.update(&this->scb);

	// Initial spectrum texture creator shader
	this->spectrumCreatorShader.createFromFile(renderer, "SpectrumCreatorShader_Comp.cso");
	this->spectrumCreatorShader.addRenderTexture(this->initialSpectrumTexture);
	this->spectrumCreatorShader.addShaderBuffer(this->spectrumCreatorShaderBuffer);
	this->spectrumCreatorShader.run(renderer);

	// Spectrum interpolator shader
	this->spectrumInterpolatorShader.createFromFile(renderer, "SpectrumInterpolatorShader_Comp.cso");
	this->spectrumInterpolatorShader.addRenderTexture(this->initialSpectrumTexture);
	this->spectrumInterpolatorShader.addRenderTexture(this->finalSpectrumTextureX);
	this->spectrumInterpolatorShader.addRenderTexture(this->finalSpectrumTextureY);
	this->spectrumInterpolatorShader.addRenderTexture(this->finalSpectrumTextureZ);
	this->spectrumInterpolatorShader.addShaderBuffer(this->spectrumInterpolationShaderBuffer);

	// Update butterfly texture shader constant buffer
	this->btb.gridSize = GRID_WIDTH;
	butterflyTextureShaderBuffer.update(&this->btb);

	// Butterfly texture shader
	this->butterflyTextureShader.createFromFile(renderer, "ButterflyTextureShader_Comp.cso");
	this->butterflyTextureShader.addRenderTexture(this->butterflyTexture);
	this->butterflyTextureShader.addShaderBuffer(this->butterflyTextureShaderBuffer);
	this->butterflyTextureShader.run(renderer);

	// Butterfly operations shader
	this->butterflyOperationsShader.createFromFile(renderer, "ButterflyOperationsShader_Comp.cso");
	this->butterflyOperationsShader.addRenderTexture(this->butterflyTexture);
	this->butterflyOperationsShader.addShaderBuffer(this->butterflyOperationShaderBuffer);

	// Inversion and permutation shader
	this->invPermShader.createFromFile(renderer, "InversionPermutationShader_Comp.cso");
	this->invPermShader.addRenderTexture(this->finalSpectrumTextureX);
	this->invPermShader.addRenderTexture(this->finalSpectrumTextureY);
	this->invPermShader.addRenderTexture(this->finalSpectrumTextureZ);
	this->invPermShader.addRenderTexture(this->displacementTexture);
	this->invPermShader.addShaderBuffer(this->invPermShaderBuffer);

	// Scale up plane
	this->mesh.setWorldMatrix(XMMatrixScaling(3.0f, 3.0f, 3.0f));
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
			butterflyOperationsShader.run(renderer);
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
	this->timer += Time::getDeltaTime() * 2.0f;

	// Clear displacement textures
	this->displacementTexture.clearRenderTexture(0.0f, 0.0f, 0.0f, 1.0f);

	// Update spectrum interpolation shader
	this->sib.gridWidth = GRID_WIDTH;
	this->sib.gridHeight = GRID_HEIGHT;
	this->sib.horizontalSize = HORIZONTAL_SIZE;
	this->sib.time = this->timer;
	spectrumInterpolationShaderBuffer.update(&this->sib);

	// Run spectrum interpolator shader
	spectrumInterpolatorShader.run(renderer);

	// FFTs
	this->runIFFT(finalSpectrumTextureY);
	if (this->displaceHorizontally)
	{
		this->runIFFT(finalSpectrumTextureX);
		this->runIFFT(finalSpectrumTextureZ);
	}

	// Update inversion and permutation shader buffer
	this->ipb.gridSize = GRID_WIDTH;
	invPermShaderBuffer.update(&this->ipb);

	// Run inversion and permutation constant buffer
	invPermShader.run(renderer);

	// Set displacement texture
	displacementTexture.setVS(0);

	// Update water shader
	this->shader.update(renderer, this->mesh.getWorldMatrix());

	// Set shader to render mesh with
	this->shader.set(renderer.getDeviceContext());
	this->mesh.draw();
}

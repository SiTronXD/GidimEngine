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
	spectrumTexture0(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R32G32B32A32_FLOAT),
	spectrumTexture1(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R32G32B32A32_FLOAT),
	finalSpectrumTexture(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R32G32B32A32_FLOAT),
	butterflyTexture(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R32G32B32A32_FLOAT),
	pingPongTexture(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R32G32B32A32_FLOAT),
	displacementTexture(renderer, TextureFilter::NEAREST_NEIGHBOR, DXGI_FORMAT_R32G32B32A32_FLOAT),

	// Shader buffers
	spectrumCreatorShaderBuffer(renderer.getDevice(), sizeof(SpectrumCreatorBuffer)),
	spectrumInterpolationShaderBuffer(renderer.getDevice(), sizeof(SpectrumInterpolationBuffer)),
	butterflyTextureShaderBuffer(renderer.getDevice(), sizeof(ButterflyTextureBuffer)),
	butterflyOperationShaderBuffer(renderer.getDevice(), sizeof(ButterflyOperationBuffer)),
	invPermShaderBuffer(renderer.getDevice(), sizeof(InvPermBuffer)),

	numMultiplicationStages(log2(GRID_WIDTH)),
	timer(0.0f)
{
	// Set textures as render textures
	this->spectrumTexture0.createAsRenderTexture(GRID_WIDTH, GRID_HEIGHT);
	this->spectrumTexture1.createAsRenderTexture(GRID_WIDTH, GRID_HEIGHT);
	this->finalSpectrumTexture.createAsRenderTexture(GRID_WIDTH, GRID_HEIGHT);
	this->butterflyTexture.createAsRenderTexture(this->numMultiplicationStages, GRID_HEIGHT);
	this->pingPongTexture.createAsRenderTexture(GRID_WIDTH, GRID_HEIGHT);
	this->displacementTexture.createAsRenderTexture(GRID_WIDTH, GRID_HEIGHT);

	// Update spectrum creator constant buffer
	D3D11_MAPPED_SUBRESOURCE mappedSpecCreatRes;
	this->spectrumCreatorShaderBuffer.map(renderer.getDeviceContext(), mappedSpecCreatRes);
	SpectrumCreatorBuffer* scb = (SpectrumCreatorBuffer*) mappedSpecCreatRes.pData;
	scb->gridWidth = GRID_WIDTH;
	scb->gridHeight = GRID_HEIGHT;
	scb->windDirection[0] = WIND_DIR_X;
	scb->windDirection[1] = WIND_DIR_Y;
	scb->horizontalSize = HORIZONTAL_SIZE;
	scb->windSpeed = WIND_SPEED;
	scb->amplitude = AMPLITUDE;
	scb->padding = 0.0f;
	this->spectrumCreatorShaderBuffer.unmap(renderer.getDeviceContext());

	// Initial spectrum texture creator shader
	this->spectrumCreatorShader.createFromFile(renderer, "SpectrumCreatorShader_Comp.cso");
	this->spectrumCreatorShader.addRenderTexture(this->spectrumTexture0);
	this->spectrumCreatorShader.addRenderTexture(this->spectrumTexture1);
	this->spectrumCreatorShader.addConstantBuffer(this->spectrumCreatorShaderBuffer);
	this->spectrumCreatorShader.run(renderer);

	// Spectrum interpolator shader
	this->spectrumInterpolatorShader.createFromFile(renderer, "SpectrumInterpolatorShader_Comp.cso");
	this->spectrumInterpolatorShader.addRenderTexture(this->spectrumTexture0);
	this->spectrumInterpolatorShader.addRenderTexture(this->spectrumTexture1);
	this->spectrumInterpolatorShader.addRenderTexture(this->finalSpectrumTexture);
	this->spectrumInterpolatorShader.addRenderTexture(this->pingPongTexture);
	this->spectrumInterpolatorShader.addConstantBuffer(this->spectrumInterpolationShaderBuffer);

	// Update butterfly texture shader constant buffer
	D3D11_MAPPED_SUBRESOURCE mappedButterTexRes;
	butterflyTextureShaderBuffer.map(renderer.getDeviceContext(), mappedButterTexRes);
	ButterflyTextureBuffer* btb = (ButterflyTextureBuffer*) mappedButterTexRes.pData;
	btb->gridSize = GRID_WIDTH;
	btb->padding[0] = 0;
	btb->padding[1] = 0;
	btb->padding[2] = 0;
	butterflyTextureShaderBuffer.unmap(renderer.getDeviceContext());

	// Butterfly texture shader
	this->butterflyTextureShader.createFromFile(renderer, "ButterflyTextureShader_Comp.cso");
	this->butterflyTextureShader.addRenderTexture(this->butterflyTexture);
	this->butterflyTextureShader.addConstantBuffer(this->butterflyTextureShaderBuffer);
	this->butterflyTextureShader.run(renderer);

	// Butterfly operations shader
	this->butterflyOperationsShader.createFromFile(renderer, "ButterflyOperationsShader_Comp.cso");
	this->butterflyOperationsShader.addRenderTexture(this->butterflyTexture);
	this->butterflyOperationsShader.addRenderTexture(this->pingPongTexture);
	this->butterflyOperationsShader.addRenderTexture(this->finalSpectrumTexture);
	this->butterflyOperationsShader.addConstantBuffer(this->butterflyOperationShaderBuffer);

	// Inversion and permutation shader
	this->invPermShader.createFromFile(renderer, "InversionPermutationShader_Comp.cso");
	this->invPermShader.addRenderTexture(this->displacementTexture);
	this->invPermShader.addRenderTexture(this->pingPongTexture);
	this->invPermShader.addRenderTexture(this->finalSpectrumTexture);
	this->invPermShader.addConstantBuffer(this->invPermShaderBuffer);

	// Scale up plane
	this->mesh.setWorldMatrix(XMMatrixScaling(3.0f, 3.0f, 3.0f));
}

Water::~Water() {}

void Water::draw()
{
	this->timer += Time::getDeltaTime();

	// Update spectrum interpolation shader
	D3D11_MAPPED_SUBRESOURCE mappedSpecIntRes;
	spectrumInterpolationShaderBuffer.map(renderer.getDeviceContext(), mappedSpecIntRes);
	SpectrumInterpolationBuffer* sib = (SpectrumInterpolationBuffer*)mappedSpecIntRes.pData;
	sib->gridWidth = GRID_WIDTH;
	sib->gridHeight = GRID_HEIGHT;
	sib->horizontalSize = HORIZONTAL_SIZE;
	sib->time = this->timer;
	spectrumInterpolationShaderBuffer.unmap(renderer.getDeviceContext());

	spectrumInterpolatorShader.run(renderer);

	// Update butterfly operations shader
	int pingPong = 0;
	D3D11_MAPPED_SUBRESOURCE mappedButterOpRes;
	for (int j = 0; j < 2; ++j)		// Horizontal, Vertical
	{
		for (int i = 0; i < this->numMultiplicationStages; ++i)
		{
			// Update shader buffer
			butterflyOperationShaderBuffer.map(renderer.getDeviceContext(), mappedButterOpRes);
			ButterflyOperationBuffer* bob = (ButterflyOperationBuffer*)mappedButterOpRes.pData;

			bob->stage = i;
			bob->pingPong = pingPong;
			bob->direction = j;
			bob->padding = 0;

			butterflyOperationShaderBuffer.unmap(renderer.getDeviceContext());


			butterflyOperationsShader.run(renderer);
			pingPong = (pingPong + 1) % 2;
		}
	}

	// Update inversion and permutation constant buffer
	D3D11_MAPPED_SUBRESOURCE mappedInvPermRes;
	invPermShaderBuffer.map(renderer.getDeviceContext(), mappedInvPermRes);
	InvPermBuffer* ipb = (InvPermBuffer*)mappedInvPermRes.pData;
	ipb->gridSize = GRID_WIDTH;
	ipb->pingPong = pingPong;
	ipb->padding[0] = 0;
	ipb->padding[1] = 0;
	invPermShaderBuffer.unmap(renderer.getDeviceContext());

	// Run inversion and permutation constant buffer
	invPermShader.run(renderer);

	// Set displacement texture
	displacementTexture.setVS();

	// Update water shader
	this->shader.update(renderer, this->mesh.getWorldMatrix());

	// Set shader to render mesh with
	this->shader.set(renderer.getDeviceContext());
	this->mesh.draw();
}

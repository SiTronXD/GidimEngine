#include "../../pch.h"
#include "BoidHandler.h"

unsigned int BoidHandler::getWangHash(unsigned int seed)
{
	seed = ((unsigned int) (seed ^ 61)) ^ ((unsigned int)(seed >> 16));
	seed *= 9;
	seed = seed ^ (seed >> 4);
	seed *= (unsigned int) 0x27d4eb2d;
	seed = seed ^ (seed >> 15);

	return seed;
}

float BoidHandler::getWangHashFloat(unsigned int state)
{
	return this->getWangHash(state) / 4294967296.0;
}

void BoidHandler::createGPUBuffer()
{
	ID3D11Device* device = renderer.getDevice();

	// Initial boid buffer data
	XMFLOAT4X4* initialMatrices = new XMFLOAT4X4[NUM_BOIDS];
	for (int i = 0; i < NUM_BOIDS; ++i)
	{
		XMFLOAT4X4 tempMat;
		XMStoreFloat4x4(
			&tempMat, 
			XMMatrixTranspose(
				XMMatrixTranslation(
					(rand() % 1000) / 1000.0f * 2.0f - 1.0f,
					(rand() % 1000) / 1000.0f * 2.0f - 1.0f,
					(rand() % 1000) / 1000.0f * 2.0f - 1.0f
				)
			)
		);

		initialMatrices[i] = tempMat;
	}
	D3D11_SUBRESOURCE_DATA initialBufferData;
	ZeroMemory(&initialBufferData, sizeof(initialBufferData));
	initialBufferData.pSysMem = initialMatrices;
	
	// Buffer
	this->boidBuffer.createBuffer(
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
		sizeof(XMFLOAT4X4),
		NUM_BOIDS,
		initialBufferData
	);

	delete[] initialMatrices;

	// Get description from buffer
	D3D11_BUFFER_DESC descBuf = this->boidBuffer.getDesc();
	

	// Buffer UAV
	this->boidBufferUAV.createUAV(
		this->boidBuffer.getBuffer(),
		DXGI_FORMAT_UNKNOWN,
		D3D11_UAV_DIMENSION_BUFFER,
		descBuf.ByteWidth / descBuf.StructureByteStride
	);

	// Buffer SRV
	this->boidBufferSRV.createSRV(
		this->boidBuffer.getBuffer(), 
		DXGI_FORMAT_UNKNOWN,
		D3D11_SRV_DIMENSION_BUFFER,
		descBuf.ByteWidth / descBuf.StructureByteStride
	);
}

void BoidHandler::debugBoidsBuffer()
{
	// --- Read buffer from GPU to CPU
	ID3D11Device* device = this->renderer.getDevice();
	ID3D11DeviceContext* deviceContext = this->renderer.getDeviceContext();

	// Initialize temporary readable buffer description 
	ID3D11Buffer* debugbuf = nullptr;
	D3D11_BUFFER_DESC desc = this->boidBuffer.getDesc();
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;

	// Create temporary readable buffer
	if (SUCCEEDED(device->CreateBuffer(&desc, NULL, &debugbuf)))
	{
		// Copy data from GPU buffer to readable buffer
		deviceContext->CopyResource(debugbuf, this->boidBuffer.getBuffer());

		// Map data
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (deviceContext->Map(debugbuf, 0, D3D11_MAP_READ, 0, &mappedResource) != S_OK)
		{
			Log::error("Failed to map debugbuf.");
		}

		// Use data
		Log::print("X: " +
			std::to_string(
				((float*)mappedResource.pData)[0]
			)
		);

		// Unmap and release temporary buffer
		deviceContext->Unmap(debugbuf, 0);
		debugbuf->Release();
	}
}

BoidHandler::BoidHandler(Renderer& renderer)
	: renderer(renderer),
	boidBuffer(renderer, "boidBuffer"),
	boidBufferUAV(renderer, "boidBufferUAV"),
	boidBufferSRV(renderer, "boidBufferSRV"),
	boidLogicShader(renderer, "CompiledShaders/Boid_Comp.cso", 1, 1, 1),
	boidClone(renderer),
	boidLogicShaderBuffer(renderer, sizeof(BoidLogicBuffer))
{
	// Prepare buffer
	this->createGPUBuffer();

	// Add buffers to compute shader
	this->boidLogicShader.addUAV(this->boidBufferUAV.getUAV());
	this->boidLogicShader.addShaderBuffer(this->boidLogicShaderBuffer);
}

BoidHandler::~BoidHandler()
{

}

void BoidHandler::updateBoids(float deltaTime)
{
	// Update logic shader buffer
	this->blb.deltaTime = deltaTime;
	this->boidLogicShaderBuffer.update(&this->blb.deltaTime);

	// Run boids logic shader
	this->boidLogicShader.run();

	//this->debugBoidsBuffer();
}

void BoidHandler::drawBoids()
{
	// Set boid buffer SRV in vertex shader
	this->boidBufferSRV.setVS();

	// Draw
	this->boidClone.draw(NUM_BOIDS);
}

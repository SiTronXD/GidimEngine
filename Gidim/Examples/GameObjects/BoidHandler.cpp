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

	// Buffer
	D3D11_BUFFER_DESC descGPUBuffer;
	ZeroMemory(&descGPUBuffer, sizeof(descGPUBuffer));
	descGPUBuffer.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	descGPUBuffer.ByteWidth = sizeof(XMFLOAT4X4) * NUM_BOIDS;
	descGPUBuffer.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	descGPUBuffer.StructureByteStride = sizeof(XMFLOAT4X4);
	HRESULT result = device->CreateBuffer(&descGPUBuffer, NULL, &this->boidsBuffer);
	if (FAILED(result))
	{
		Log::resultFailed("Failed creating boids buffer.", result);
	}

	// Get description from buffer
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	this->boidsBuffer->GetDesc(&descBuf);


	// Buffer UAV
	D3D11_UNORDERED_ACCESS_VIEW_DESC descView;
	ZeroMemory(&descView, sizeof(descView));
	descView.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	descView.Format = DXGI_FORMAT_UNKNOWN;
	descView.Buffer.FirstElement = 0;
	descView.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
	result = device->CreateUnorderedAccessView(this->boidsBuffer, &descView, &this->boidsBufferUAV);
	if (FAILED(result))
	{
		Log::resultFailed("Failed creating boids buffer UAV.", result);
	}


	// Buffer SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
	ZeroMemory(&descSRV, sizeof(descSRV));
	descSRV.Format = DXGI_FORMAT_UNKNOWN;
	descSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	descSRV.Buffer.FirstElement = 0;
	descSRV.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;

	result = device->CreateShaderResourceView(this->boidsBuffer, &descSRV, &this->boidsBufferSRV);
	if (FAILED(result))
	{
		Log::resultFailed("Failed creating boids buffer SRV.", result);
	}
}

void BoidHandler::debugBoidsBuffer()
{
	// --- Read buffer from GPU to CPU
	ID3D11Device* device = this->renderer.getDevice();
	ID3D11DeviceContext* deviceContext = this->renderer.getDeviceContext();

	// Initialize temporary readable buffer description 
	ID3D11Buffer* debugbuf = nullptr;
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	this->boidsBuffer->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;

	// Create temporary readable buffer
	if (SUCCEEDED(device->CreateBuffer(&desc, NULL, &debugbuf)))
	{
		// Copy data from GPU buffer to readable buffer
		deviceContext->CopyResource(debugbuf, this->boidsBuffer);

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
	boidsBuffer(nullptr),
	boidsBufferUAV(nullptr),
	boidsBufferSRV(nullptr),
	boidsLogicShader(renderer, "CompiledShaders/Boid_Comp.cso", 1, 1, 1),
	boidIDShaderBuffer(renderer, sizeof(BoidIDBuffer)),
	boidClone(renderer)
{
	// Add all boids
	for (int i = 0; i < this->NUM_BOIDS; ++i)
	{
		this->boidColors.push_back(
			{ 
				this->getWangHashFloat(i * 3 + 0),
				this->getWangHashFloat(i * 3 + 1),
				this->getWangHashFloat(i * 3 + 2),
			}
		);
	}

	// Prepare buffer
	this->createGPUBuffer();

	// Add buffer to compute shader
	this->boidsLogicShader.addUAV(this->boidsBufferUAV);
}

BoidHandler::~BoidHandler()
{
	S_RELEASE(this->boidsBuffer);
	S_RELEASE(this->boidsBufferUAV);
	S_RELEASE(this->boidsBufferSRV);
}

void BoidHandler::updateBoids()
{
	// Run boids logic shader
	this->boidsLogicShader.run();

	//this->debugBoidsBuffer();
}

void BoidHandler::drawBoids()
{
	// Set boid buffer
	this->renderer.getDeviceContext()->VSSetShaderResources(0, 1, &this->boidsBufferSRV);

	for (int i = 0; i < NUM_BOIDS; ++i)
	{
		// Update boid ID buffer
		bib.id = i;
		bib.color = this->boidColors[i];
		this->boidIDShaderBuffer.update(&bib);
		this->boidIDShaderBuffer.setVS(1);

		// Draw
		this->boidClone.draw();
	}
}

#include "../../pch.h"
#include "BoidHandler.h"

void BoidHandler::createGPUBuffer()
{
	ID3D11Device* device = renderer.getDevice();

	// Buffer
	D3D11_BUFFER_DESC descGPUBuffer;
	ZeroMemory(&descGPUBuffer, sizeof(descGPUBuffer));
	descGPUBuffer.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	descGPUBuffer.ByteWidth = sizeof(float) * 3 * NUM_BOIDS;
	descGPUBuffer.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	descGPUBuffer.StructureByteStride = sizeof(float) * 3;
	HRESULT result = device->CreateBuffer(&descGPUBuffer, NULL, &this->boidsBuffer);
	if (FAILED(result))
	{
		Log::resultFailed("Failed creating dest gpu buffer.", result);
	}


	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	this->boidsBuffer->GetDesc(&descBuf);

	// Buffer UAV
	D3D11_UNORDERED_ACCESS_VIEW_DESC descView;
	ZeroMemory(&descView, sizeof(descView));
	descView.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	descView.Buffer.FirstElement = 0;
	descView.Format = DXGI_FORMAT_UNKNOWN;
	descView.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
	result = device->CreateUnorderedAccessView(this->boidsBuffer, &descView, &this->boidsBufferUAV);
	if (FAILED(result))
	{
		Log::resultFailed("Failed creating dest gpu buffer view.", result);
	}
}

BoidHandler::BoidHandler(Renderer& renderer)
	: renderer(renderer),
	boidsLogicShader(renderer, "CompiledShaders/Boid_Comp.cso", 1, 1, 1)
{
	// Add all boids
	for (int i = 0; i < this->NUM_BOIDS; ++i)
	{
		this->boids.push_back(new Boid(renderer));
	}

	// Prepare buffer
	this->createGPUBuffer();

	// Add buffer to compute shader
	this->boidsLogicShader.addUAV(this->boidsBufferUAV);
}

BoidHandler::~BoidHandler()
{
	for (int i = this->boids.size() - 1; i >= 0; --i)
	{
		delete this->boids[i];

		this->boids.erase(this->boids.begin() + i);
	}

	S_RELEASE(this->boidsBuffer);
	S_RELEASE(this->boidsBufferUAV);
}

void BoidHandler::updateBoids()
{
	// Run boids logic shader
	this->boidsLogicShader.run();

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
				((float*) mappedResource.pData)[0]
			)
		);

		// Unmap and release temporary buffer
		deviceContext->Unmap(debugbuf, 0);
		debugbuf->Release();
	}
}

void BoidHandler::drawBoids()
{
	for (int i = 0; i < NUM_BOIDS; ++i)
	{
		this->boids[i]->draw();
	}
}

#include "../../pch.h"
#include "BoidHandler.h"

BoidHandler::BoidHandler(Renderer& renderer)
	: renderer(renderer),
	boidsLogicShader(renderer, 1, 1, 1)
{
	boidsLogicShader.createFromFile("CompiledShaders/Boid_Comp.cso");

	// Add all boids
	for (int i = 0; i < this->NUM_BOIDS; ++i)
	{
		this->boids.push_back(new Boid(renderer));
	}

	ID3D11Device* device = renderer.getDevice();

	D3D11_BUFFER_DESC descGPUBuffer;
	ZeroMemory(&descGPUBuffer, sizeof(descGPUBuffer));
	descGPUBuffer.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	descGPUBuffer.ByteWidth = sizeof(float) * 3 * NUM_BOIDS;
	descGPUBuffer.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	descGPUBuffer.StructureByteStride = sizeof(float) * 3;
	HRESULT result = device->CreateBuffer(&descGPUBuffer, NULL, &this->destDataGPUBuffer);
	if (FAILED(result))
	{
		Log::resultFailed("Failed creating dest gpu buffer.", result);
	}


	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	this->destDataGPUBuffer->GetDesc(&descBuf);

	D3D11_UNORDERED_ACCESS_VIEW_DESC descView;
	ZeroMemory(&descView, sizeof(descView));
	descView.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	descView.Buffer.FirstElement = 0;
	descView.Format = DXGI_FORMAT_UNKNOWN;
	descView.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
	result = device->CreateUnorderedAccessView(destDataGPUBuffer, &descView, &this->destDataGPUBufferView);
	if (FAILED(result))
	{
		Log::resultFailed("Failed creating dest gpu buffer view.", result);
	}

	this->boidsLogicShader.addUAV(this->destDataGPUBufferView);
}

BoidHandler::~BoidHandler()
{
	for (int i = this->boids.size() - 1; i >= 0; --i)
	{
		delete this->boids[i];

		this->boids.erase(this->boids.begin() + i);
	}

	S_RELEASE(this->destDataGPUBuffer);
	S_RELEASE(this->destDataGPUBufferView);
}

void BoidHandler::updateBoids()
{
	// Run boids logic shader
	this->boidsLogicShader.run();


	ID3D11Device* device = this->renderer.getDevice();
	ID3D11DeviceContext* deviceContext = this->renderer.getDeviceContext();

	ID3D11Buffer* debugbuf = NULL;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	this->destDataGPUBuffer->GetDesc(&desc);

	UINT byteSize = desc.ByteWidth;

	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;

	if (SUCCEEDED(device->CreateBuffer(&desc, NULL, &debugbuf)))
	{
		deviceContext->CopyResource(debugbuf, this->destDataGPUBuffer);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (deviceContext->Map(debugbuf, 0, D3D11_MAP_READ, 0, &mappedResource) != S_OK)
		{
			Log::error("Failed to map debugbuf.");
		}

		byte* outBuff = new byte[byteSize];
		memcpy(outBuff, mappedResource.pData, byteSize);

		Log::print("X: " + 
			std::to_string(
				((float*) mappedResource.pData)[0]
			)
		);

		deviceContext->Unmap(debugbuf, 0);

		debugbuf->Release();


		// ----- outBuff -----


		delete outBuff;
	}
}

void BoidHandler::drawBoids()
{
	for (int i = 0; i < NUM_BOIDS; ++i)
	{
		this->boids[i]->draw();
	}
}

#include "../../pch.h"
#include "BoidHandler.h"
#include "../../Engine/Application/Time.h"

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
	return this->getWangHash(state) / 4294967296.0f;
}

void BoidHandler::createGPUBuffers()
{
	ID3D11Device* device = renderer.getDevice();

	// ---------- Unsorted list buffer ----------

	// Buffer
	this->boidListBuffer.createArrayBuffer(
		D3D11_BIND_UNORDERED_ACCESS,
		sizeof(unsigned int) * 2,
		NUM_BOIDS
	);

	// Buffer UAV
	this->boidListBufferUAV.createUAV(
		this->boidListBuffer.getBuffer(),
		DXGI_FORMAT_UNKNOWN,
		D3D11_UAV_DIMENSION_BUFFER,
		this->boidListBuffer.getNumElements()
	);

	// ---------- Velocity vectors ----------

	// Initial velocity vectors
	XMFLOAT3* initialVeloc = new XMFLOAT3[NUM_BOIDS];
	for (int i = 0; i < NUM_BOIDS; ++i)
	{
		// Create random unit vector
		XMFLOAT3 tempVec;
		XMStoreFloat3(
			&tempVec,
			XMVector3Normalize(XMVectorSet(
				(rand() % 1000) / 1000.0f * 2.0f - 1.0f,
				(rand() % 1000) / 1000.0f * 2.0f - 1.0f,
				(rand() % 1000) / 1000.0f * 2.0f - 1.0f,
				0.0f
			))
		);

		initialVeloc[i] = tempVec;
	}
	D3D11_SUBRESOURCE_DATA initialVelocData;
	ZeroMemory(&initialVelocData, sizeof(initialVelocData));
	initialVelocData.pSysMem = initialVeloc;

	// Buffer
	this->boidVelocBuffer.createArrayBuffer(
		D3D11_BIND_UNORDERED_ACCESS,
		sizeof(XMFLOAT3),
		NUM_BOIDS,
		&initialVelocData
	);
	delete[] initialVeloc;

	// Buffer UAV
	this->boidVelocUAV.createUAV(
		this->boidVelocBuffer.getBuffer(),
		DXGI_FORMAT_UNKNOWN,
		D3D11_UAV_DIMENSION_BUFFER,
		this->boidVelocBuffer.getNumElements()
	);

	// ---------- Transformation matrices ----------

	// Initial boid buffer data
	XMFLOAT4X4* initialMatrices = new XMFLOAT4X4[NUM_BOIDS];
	for (int i = 0; i < NUM_BOIDS; ++i)
	{
		XMFLOAT4X4 tempMat;
		XMStoreFloat4x4(
			&tempMat, 
			XMMatrixTranspose(
				XMMatrixTranslation(
					(rand() % 1000) / 1000.0f * 2.0f * PLAY_HALF_VOLUME_SIZE - PLAY_HALF_VOLUME_SIZE,
					(rand() % 1000) / 1000.0f * 2.0f * PLAY_HALF_VOLUME_SIZE - PLAY_HALF_VOLUME_SIZE,
					(rand() % 1000) / 1000.0f * 2.0f * PLAY_HALF_VOLUME_SIZE - PLAY_HALF_VOLUME_SIZE
				)
			)
		);

		initialMatrices[i] = tempMat;
	}
	D3D11_SUBRESOURCE_DATA initialBufferData;
	ZeroMemory(&initialBufferData, sizeof(initialBufferData));
	initialBufferData.pSysMem = initialMatrices;
	
	// Buffer
	this->boidBuffer.createArrayBuffer(
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
		sizeof(XMFLOAT4X4),
		NUM_BOIDS,
		&initialBufferData
	);

	delete[] initialMatrices;

	// Buffer UAV
	this->boidBufferUAV.createUAV(
		this->boidBuffer.getBuffer(),
		DXGI_FORMAT_UNKNOWN,
		D3D11_UAV_DIMENSION_BUFFER,
		this->boidBuffer.getNumElements()
	);

	// Buffer SRV
	this->boidBufferSRV.createSRV(
		this->boidBuffer.getBuffer(), 
		DXGI_FORMAT_UNKNOWN,
		D3D11_SRV_DIMENSION_BUFFER,
		this->boidBuffer.getNumElements()
	);
}

void BoidHandler::printBoidBufferElement(D3DBuffer& debugBuffer, unsigned int index)
{
	// --- Read buffer from GPU to CPU
	ID3D11Device* device = this->renderer.getDevice();
	ID3D11DeviceContext* deviceContext = this->renderer.getDeviceContext();

	// Initialize temporary readable buffer description 
	ID3D11Buffer* debugbuf = nullptr;
	D3D11_BUFFER_DESC desc = debugBuffer.getDesc();
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;

	// Create temporary readable buffer
	if (SUCCEEDED(device->CreateBuffer(&desc, NULL, &debugbuf)))
	{
		// Copy data from GPU buffer to readable buffer
		deviceContext->CopyResource(debugbuf, debugBuffer.getBuffer());

		// Map data
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (deviceContext->Map(debugbuf, 0, D3D11_MAP_READ, 0, &mappedResource) != S_OK)
		{
			Log::error("Failed to map debugbuf.");
		}

		// Print data
		for (int i = 0; i < NUM_BOIDS; ++i)
		{
			Log::print("uint(" +
				std::to_string(
					((unsigned int*)mappedResource.pData)[i * 2 + 0]
				) + ", " +
				std::to_string(
					((unsigned int*)mappedResource.pData)[i * 2 + 1]
				) + ")"
			);
		}

		// Unmap and release temporary buffer
		deviceContext->Unmap(debugbuf, 0);
		debugbuf->Release();
	}
}

#define SORT_LOCAL_WORKGROUP_SIZE 8

void BoidHandler::localBMS(unsigned int h)
{
	this->bSortb.subAlgorithmEnum = BitonicAlgorithmType::LOCAL_BMS;

	this->dispatchSort(h);
}
void BoidHandler::bigFlip(unsigned int h)
{
	this->bSortb.subAlgorithmEnum = BitonicAlgorithmType::BIG_FLIP;

	this->dispatchSort(h);
}
void BoidHandler::localDisperse(unsigned int h)
{
	this->bSortb.subAlgorithmEnum = BitonicAlgorithmType::LOCAL_DISPERSE;

	this->dispatchSort(h);
}
void BoidHandler::bigDisperse(unsigned int h)
{
	this->bSortb.subAlgorithmEnum = BitonicAlgorithmType::BIG_DISPERSE;
	
	this->dispatchSort(h);
}
void BoidHandler::dispatchSort(unsigned int h)
{
	this->bSortb.parameterH = h;
	this->boidSortShaderBuffer.update(&this->bSortb);

	this->boidSortShader.run();
}

void BoidHandler::sortBoidList()
{
	unsigned int workgroupCount = NUM_BOIDS / THREAD_GROUP_SIZE;
	unsigned int h = THREAD_GROUP_SIZE;

	// Calculate the first smaller iterations within thread groups
	// (flip + disperse)
	this->localBMS(h);

	h <<= 1;

	for (; h <= NUM_BOIDS; h <<= 1)
	{
		// Calculate flip that overlaps several thread groups
		this->bigFlip(h);

		// Disperse
		for (unsigned int hh = h >> 1; hh > 1; hh >>= 1)
		{
			// If possible, calculate disperse within thread group
			if (hh <= THREAD_GROUP_SIZE)
			{
				this->localDisperse(hh);
				break;
			}
			// If not, calculate disperse that overlaps several thread groups
			else
			{
				this->bigDisperse(hh);
			}
		}
	}
}

BoidHandler::BoidHandler(Renderer& renderer)
	: renderer(renderer),

	boidListBuffer(renderer, "boidListBuffer"),
	boidListBufferUAV(renderer, "boidListBufferUAV"),

	boidVelocBuffer(renderer, "boidVelocBuffer"),
	boidVelocUAV(renderer, "boidVelocUAV"),

	boidBuffer(renderer, "boidBuffer"),
	boidBufferUAV(renderer, "boidBufferUAV"),
	boidBufferSRV(renderer, "boidBufferSRV"),

	boidInsertShader(renderer, "CompiledShaders/BoidInsertList_Comp.cso", NUM_BOIDS / THREAD_GROUP_SIZE, 1, 1),
	boidSortShader(renderer, "CompiledShaders/BoidListSort_Comp.cso", NUM_BOIDS / THREAD_GROUP_SIZE, 1, 1),
	boidLogicShader(renderer, "CompiledShaders/Boid_Comp.cso", NUM_BOIDS / THREAD_GROUP_SIZE, 1, 1),
	boidClone(renderer),
	boidInsertShaderBuffer(renderer, sizeof(BoidInsertBuffer)),
	boidSortShaderBuffer(renderer, sizeof(BoidSortBuffer)),
	boidLogicShaderBuffer(renderer, sizeof(BoidLogicBuffer))
{
	// Prepare buffers
	this->createGPUBuffers();

	// Add buffers to list compute shader
	this->boidInsertShader.addUAV(this->boidListBufferUAV.getUAV());
	this->boidInsertShader.addUAV(this->boidBufferUAV.getUAV());
	this->boidInsertShader.addConstantBuffer(this->boidInsertShaderBuffer);

	// Add buffers to sort compute shader
	this->boidSortShader.addUAV(this->boidListBufferUAV.getUAV());
	this->boidSortShader.addConstantBuffer(this->boidSortShaderBuffer);

	// Add buffers to logic compute shader
	this->boidLogicShader.addUAV(this->boidVelocUAV.getUAV());
	this->boidLogicShader.addUAV(this->boidBufferUAV.getUAV());
	this->boidLogicShader.addConstantBuffer(this->boidLogicShaderBuffer);

	// Set values in list constant buffer once
	this->bInsertb.halfVolumeSize = PLAY_HALF_VOLUME_SIZE;
	this->bInsertb.maxSearchRadius = 5.0f;
	this->boidInsertShaderBuffer.update(&this->bInsertb);

	// Set values in sort constant buffer once
	this->bSortb.numElements = NUM_BOIDS;

	// Set values in logic constant buffer once
	this->bLogicb.halfVolumeSize = (float) PLAY_HALF_VOLUME_SIZE;
	this->bLogicb.numBoids = NUM_BOIDS;
}

BoidHandler::~BoidHandler()
{

}

#include <chrono>
bool hasPrintedSortedBuffer = false;

void BoidHandler::updateBoids(float deltaTime)
{
	// ---------- Boid insert list shader ----------
	this->boidInsertShader.run();

	// ---------- Boid list sort shader ----------

	// Print boid list before sorting
	/*if (Time::hasOneSecondPassed() && !hasPrintedSortedBuffer)
	{
		// Debug GPU buffer
		Log::print("--------------------");
		Log::print("before sort:");
		this->printBoidBufferElement(this->boidListBuffer, 0);
	}*/

	// Record start time
	auto startTime = std::chrono::steady_clock::now();

	// Sort!
	this->sortBoidList();

	// Record end time and print sorting time
	auto endTime = std::chrono::steady_clock::now();
	Log::print("Sort time: " + 
		std::to_string(
			std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()
		) + " ms"
	);

	// Print boid list after sorting
	/*if (Time::hasOneSecondPassed() && !hasPrintedSortedBuffer)
	{
		// Debug GPU buffer
		Log::print("after sort:");
		this->printBoidBufferElement(this->boidListBuffer, 0);

		hasPrintedSortedBuffer = true;
	}*/

	// ---------- Boid logic shader ----------

	// Update logic shader buffer
	this->bLogicb.deltaTime = deltaTime;
	this->boidLogicShaderBuffer.update(&this->bLogicb);

	// Run boids logic shader
	this->boidLogicShader.run();
}

void BoidHandler::drawBoids()
{
	// Set boid buffer SRV in vertex shader
	this->boidBufferSRV.setVS();

	// Draw
	this->boidClone.draw(NUM_BOIDS);
}

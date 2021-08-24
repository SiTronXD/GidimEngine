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

	// ---------- List buffer ----------

	// Buffer
	this->boidListBuffer.createStructuredBuffer(
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
		sizeof(unsigned int) * 2,
		NUM_BOIDS
	);

	// Buffer UAV
	this->boidListBufferUAV.createUAV(
		this->boidListBuffer,
		DXGI_FORMAT_UNKNOWN,
		D3D11_UAV_DIMENSION_BUFFER
	);

	// Buffer SRV
	this->boidListBufferSRV.createSRV(
		this->boidListBuffer,
		DXGI_FORMAT_UNKNOWN,
		D3D11_SRV_DIMENSION_BUFFER
	);

	// ---------- Offset buffer ----------

	// Buffer
	this->boidOffsetBuffer.createStructuredBuffer(
		D3D11_BIND_UNORDERED_ACCESS,
		sizeof(unsigned int),
		NUM_GRID_CELLS
	);

	// Buffer UAV
	this->boidOffsetBufferUAV.createUAV(
		this->boidOffsetBuffer,
		DXGI_FORMAT_UNKNOWN,
		D3D11_UAV_DIMENSION_BUFFER
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
	this->boidVelocBuffer.createStructuredBuffer(
		D3D11_BIND_UNORDERED_ACCESS,
		sizeof(XMFLOAT3),
		NUM_BOIDS,
		&initialVelocData
	);
	delete[] initialVeloc;

	// Buffer UAV
	this->boidVelocUAV.createUAV(
		this->boidVelocBuffer,
		DXGI_FORMAT_UNKNOWN,
		D3D11_UAV_DIMENSION_BUFFER
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
	this->boidBuffer.createStructuredBuffer(
		D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
		sizeof(XMFLOAT4X4),
		NUM_BOIDS,
		&initialBufferData
	);

	delete[] initialMatrices;

	// Buffer UAV
	this->boidBufferUAV.createUAV(
		this->boidBuffer,
		DXGI_FORMAT_UNKNOWN,
		D3D11_UAV_DIMENSION_BUFFER
	);

	// Buffer SRV
	this->boidBufferSRV.createSRV(
		this->boidBuffer, 
		DXGI_FORMAT_UNKNOWN,
		D3D11_SRV_DIMENSION_BUFFER
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
		if (index <= 0)
		{
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
		}
		else
		{
			for (int i = 0; i < NUM_GRID_CELLS; ++i)
			{
				Log::print("uint(" +
					std::to_string(
						i//((unsigned int*)mappedResource.pData)[i * 1 + 0]
					) + ", " +
					std::to_string(
						((unsigned int*)mappedResource.pData)[i * 1 + 0]
					) + ")"
				);
			}
		}

		// Unmap and release temporary buffer
		deviceContext->Unmap(debugbuf, 0);
		debugbuf->Release();
	}
}

#define SORT_LOCAL_WORKGROUP_SIZE 8

void BoidHandler::localBMS(unsigned int h)
{
	this->bSortB.subAlgorithmEnum = BitonicAlgorithmType::LOCAL_BMS;

	this->dispatchSort(h);
}
void BoidHandler::bigFlip(unsigned int h)
{
	this->bSortB.subAlgorithmEnum = BitonicAlgorithmType::BIG_FLIP;

	this->dispatchSort(h);
}
void BoidHandler::localDisperse(unsigned int h)
{
	this->bSortB.subAlgorithmEnum = BitonicAlgorithmType::LOCAL_DISPERSE;

	this->dispatchSort(h);
}
void BoidHandler::bigDisperse(unsigned int h)
{
	this->bSortB.subAlgorithmEnum = BitonicAlgorithmType::BIG_DISPERSE;
	
	this->dispatchSort(h);
}
void BoidHandler::dispatchSort(unsigned int h)
{
	this->bSortB.parameterH = h;
	this->boidSortShaderBuffer.update(&this->bSortB);

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

	// List of cell- and boid IDs
	boidListBuffer(renderer, "boidListBuffer"),
	boidListBufferUAV(renderer, "boidListBufferUAV"),
	boidListBufferSRV(renderer, "boidListBufferSRV"),

	// Offsets into sorted list
	boidOffsetBuffer(renderer, "boidOffsetBuffer"),
	boidOffsetBufferUAV(renderer, "boidOffsetBufferUAV"),

	// Boid velocities
	boidVelocBuffer(renderer, "boidVelocBuffer"),
	boidVelocUAV(renderer, "boidVelocUAV"),

	// Boid transformations
	boidBuffer(renderer, "boidBuffer"),
	boidBufferUAV(renderer, "boidBufferUAV"),
	boidBufferSRV(renderer, "boidBufferSRV"),

	// Compute shaders
	boidInsertShader(renderer, "CompiledShaders/BoidInsertList_Comp.cso", NUM_BOIDS / THREAD_GROUP_SIZE, 1, 1),
	boidSortShader(renderer, "CompiledShaders/BoidListSort_Comp.cso", NUM_BOIDS / THREAD_GROUP_SIZE, 1, 1),
	boidOffsetClearShader(renderer, "CompiledShaders/BoidOffsetClear_Comp.cso", (NUM_GRID_CELLS-1) / 32 + 1, 1, 1),
	boidOffsetInsertShader(renderer, "CompiledShaders/BoidOffsetInsert_Comp.cso", NUM_BOIDS / THREAD_GROUP_SIZE, 1, 1),
	boidLogicShader(renderer, "CompiledShaders/Boid_Comp.cso", NUM_BOIDS / THREAD_GROUP_SIZE, 1, 1),

	boidInstancer(renderer),
	boidInsertShaderBuffer(renderer, sizeof(BoidInsertBuffer)),
	boidSortShaderBuffer(renderer, sizeof(BoidSortBuffer)),
	boidOffsetClearShaderBuffer(renderer, sizeof(BoidOffsetClearBuffer)),
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
	this->boidSortShader.addUAV(this->boidVelocUAV.getUAV());
	this->boidSortShader.addUAV(this->boidBufferUAV.getUAV());
	this->boidSortShader.addConstantBuffer(this->boidSortShaderBuffer);

	// Add buffers to offset clear compute shader
	this->boidOffsetClearShader.addUAV(this->boidOffsetBufferUAV.getUAV());
	this->boidOffsetClearShader.addConstantBuffer(this->boidOffsetClearShaderBuffer);

	// Add bufers to offset insert compute shader
	this->boidOffsetInsertShader.addUAV(this->boidListBufferUAV.getUAV());
	this->boidOffsetInsertShader.addUAV(this->boidOffsetBufferUAV.getUAV());

	// Add buffers to logic compute shader
	this->boidLogicShader.addUAV(this->boidVelocUAV.getUAV());
	this->boidLogicShader.addUAV(this->boidBufferUAV.getUAV());
	this->boidLogicShader.addUAV(this->boidListBufferUAV.getUAV());
	this->boidLogicShader.addUAV(this->boidOffsetBufferUAV.getUAV());
	this->boidLogicShader.addConstantBuffer(this->boidLogicShaderBuffer);

	// Set values in list constant buffer once
	this->bInsertB.halfVolumeSize = PLAY_HALF_VOLUME_SIZE;
	this->bInsertB.maxSearchRadius = (float) BOID_MAX_SEARCH_RADIUS;
	this->boidInsertShaderBuffer.update(&this->bInsertB);

	// Set values in sort constant buffer once
	this->bSortB.numElements = NUM_BOIDS;

	// Set values in offset clear buffer once
	this->bOffsetClearB.numGridCells = NUM_GRID_CELLS;
	this->boidOffsetClearShaderBuffer.update(&this->bOffsetClearB);

	// Set values in logic constant buffer once
	this->bLogicB.halfVolumeSize = (float) PLAY_HALF_VOLUME_SIZE;
	this->bLogicB.maxSearchRadius = (float) BOID_MAX_SEARCH_RADIUS;
	this->bLogicB.numBoids = NUM_BOIDS;
}

BoidHandler::~BoidHandler()
{

}

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

	// Sort!
	this->sortBoidList();

	// Print boid list after sorting
	/*if (Time::hasOneSecondPassed() && !hasPrintedSortedBuffer)
	{
		// Debug GPU buffer
		Log::print("after sort:");
		this->printBoidBufferElement(this->boidListBuffer, 0);

		hasPrintedSortedBuffer = true;
	}*/

	// ---------- Boid offset clear shader ----------

	this->boidOffsetClearShader.run();

	// ---------- Boid offset insert shader ----------

	this->boidOffsetInsertShader.run();

	/*Log::print("---------- LIST BUFFER ----------");
	this->printBoidBufferElement(this->boidListBuffer, 0);
	Log::print("---------- OFFSET ----------");
	this->printBoidBufferElement(this->boidOffsetBuffer, 1);*/

	// ---------- Coherent shaders ----------

	// this->boidCoherentVelocityShader.run();
	// this->boidCoherentTransformShader.run();

	// ---------- Boid logic shader ----------

	// Update logic shader buffer
	this->bLogicB.deltaTime = deltaTime;
	this->boidLogicShaderBuffer.update(&this->bLogicB);

	// Run boids logic shader
	this->boidLogicShader.run();
}

void BoidHandler::drawBoids()
{
	// Set boid buffer SRV in vertex shader
	this->boidBufferSRV.setVS(0);
	this->boidListBufferSRV.setVS(1);

	// Draw all boids
	this->boidInstancer.draw(NUM_BOIDS);
}

#pragma once

#include "Boid.h"
#include "../../Engine/Graphics/ComputeShader.h"
#include "../../Engine/Graphics/DirectX11/D3DBuffer.h"
#include "../../Engine/Graphics/DirectX11/D3DUAV.h"
#include "../../Engine/Graphics/DirectX11/D3DSRV.h"

enum BitonicSubAlgorithmType
{
	LOCAL_BMS = 0,
	LOCAL_DISPERSE = 1,
	BIG_FLIP = 2,
	BIG_DISPERSE = 3
};

class BoidHandler
{
private:
	static const unsigned int THREAD_GROUP_SIZE = 1024;
	static const unsigned int NUM_BOIDS = 1024 * 512; // 1024, 1024 * 1024
	static const int PLAY_HALF_VOLUME_SIZE = 200;	// 15, 200
	static const int BOID_MAX_SEARCH_RADIUS = 5;
	static const unsigned int NUM_GRID_CELLS =
		(PLAY_HALF_VOLUME_SIZE / BOID_MAX_SEARCH_RADIUS * 2) *
		(PLAY_HALF_VOLUME_SIZE / BOID_MAX_SEARCH_RADIUS * 2) *
		(PLAY_HALF_VOLUME_SIZE / BOID_MAX_SEARCH_RADIUS * 2);

	struct BoidInsertBuffer
	{
		float halfVolumeSize;
		float maxSearchRadius;

		XMFLOAT2 padding;
	} bInsertB{};

	struct BoidSortBuffer
	{
		int numElements;
		int subAlgorithmEnum;
		int parameterH;

		float padding;
	} bSortB{};

	struct BoidOffsetClearBuffer 
	{
		unsigned int numGridCells;

		XMFLOAT3 padding;
	} bOffsetClearB{};

	struct BoidLogicBuffer
	{
		float deltaTime;
		float halfVolumeSize;
		float maxSearchRadius;

		int numBoids;
		int evaluationOffset;

		XMFLOAT3 padding;
	} bLogicB{};

	Renderer& renderer;

	Boid boidInstancer;

	// List of cell- and boid IDs
	D3DBuffer boidListBuffer;
	D3DUAV boidListUAV;
	D3DSRV boidListSRV;

	// Offsets into sorted list
	D3DBuffer boidOffsetBuffer;
	D3DUAV boidOffsetUAV;

	// Boid velocities
	D3DBuffer boidVelocityBuffer;
	D3DUAV boidVelocityUAV;

	// New boid velocities
	D3DBuffer boidNewVelocityBuffer;
	D3DUAV boidNewVelocityUAV;

	// Boid transformation matrices
	D3DBuffer boidTransformBuffer;
	D3DUAV boidTransformUAV;
	D3DSRV boidTransformSRV;

	// New boid transformation matrices
	D3DBuffer boidNewTransformBuffer;
	D3DUAV boidNewTransformUAV;

	// Compute shader
	ComputeShader boidInsertShader;
	ComputeShader boidSortShader;
	ComputeShader boidOffsetClearShader;
	ComputeShader boidOffsetInsertShader;
	ComputeShader boidLogicShader;
	ComputeShader boidUpdateVelocityTransformShader;

	ConstantBuffer boidInsertShaderBuffer;
	ConstantBuffer boidSortShaderBuffer;
	ConstantBuffer boidOffsetClearShaderBuffer;
	ConstantBuffer boidLogicShaderBuffer;


	void createGPUBuffers();
	void printBoidBufferElement(D3DBuffer& debugBuffer);

	void localBMS(unsigned int h);
	void bigFlip(unsigned int h);
	void localDisperse(unsigned int h);
	void bigDisperse(unsigned int h);
	void dispatchSort(unsigned int h);

	void sortBoidList();

	unsigned int getWangHash(unsigned int seed);
	float getWangHashFloat(unsigned int state);

public:
	BoidHandler(Renderer& renderer);
	~BoidHandler();

	void updateBoids(float deltaTime);
	void drawBoids();
};
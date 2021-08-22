#pragma once

#include "Boid.h"
#include "../../Engine/Graphics/ComputeShader.h"
#include "../../Engine/Graphics/DirectX11/D3DBuffer.h"
#include "../../Engine/Graphics/DirectX11/D3DUAV.h"
#include "../../Engine/Graphics/DirectX11/D3DSRV.h"

enum BitonicAlgorithmType
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
	static const unsigned int NUM_BOIDS = 1024 * 2; // 128, 1024 * 32
	static const int PLAY_HALF_VOLUME_SIZE = 10;	// 10, 50

	struct BoidInsertBuffer
	{
		float halfVolumeSize;
		float maxSearchRadius;

		XMFLOAT2 padding;
	} bInsertb{};

	struct BoidSortBuffer
	{
		int numElements;
		int subAlgorithmEnum;
		int parameterH;

		float padding;
	} bSortb{};

	struct BoidLogicBuffer
	{
		float deltaTime;
		float halfVolumeSize;

		int numBoids;

		float padding;
	} bLogicb{};

	Renderer& renderer;

	Boid boidClone;

	D3DBuffer boidListBuffer;
	D3DUAV boidListBufferUAV;

	D3DBuffer boidVelocBuffer;
	D3DUAV boidVelocUAV;

	D3DBuffer boidBuffer;
	D3DUAV boidBufferUAV;
	D3DSRV boidBufferSRV;

	ComputeShader boidInsertShader;
	ComputeShader boidSortShader;
	ComputeShader boidLogicShader;

	ConstantBuffer boidInsertShaderBuffer;
	ConstantBuffer boidSortShaderBuffer;
	ConstantBuffer boidLogicShaderBuffer;


	void createGPUBuffers();
	void printBoidBufferElement(D3DBuffer& debugBuffer, unsigned int index);

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
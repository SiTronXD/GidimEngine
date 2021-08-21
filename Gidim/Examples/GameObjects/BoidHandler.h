#pragma once

#include "Boid.h"
#include "../../Engine/Graphics/ComputeShader.h"
#include "../../Engine/Graphics/DirectX11/D3DBuffer.h"
#include "../../Engine/Graphics/DirectX11/D3DUAV.h"
#include "../../Engine/Graphics/DirectX11/D3DSRV.h"

class BoidHandler
{
private:
	static const unsigned int NUM_BOIDS = 2; // 128, 1024 * 32
	static const int PLAY_HALF_VOLUME_SIZE = 5;	// 10, 50

	struct BoidInsertBuffer
	{
		float halfVolumeSize;
		float maxSearchRadius;

		XMFLOAT2 padding;
	} bInsertb{};

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
	ComputeShader boidLogicShader;

	ConstantBuffer boidInsertShaderBuffer;
	ConstantBuffer boidLogicShaderBuffer;


	void createGPUBuffers();
	void printBoidBufferElement(D3DBuffer& debugBuffer, unsigned int index);

	unsigned int getWangHash(unsigned int seed);
	float getWangHashFloat(unsigned int state);

public:
	BoidHandler(Renderer& renderer);
	~BoidHandler();

	void updateBoids(float deltaTime);
	void drawBoids();
};
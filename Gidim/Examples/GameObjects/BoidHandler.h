#pragma once

#include "Boid.h"
#include "../../Engine/Graphics/ComputeShader.h"
#include "../../Engine/Graphics/DirectX11/D3DBuffer.h"
#include "../../Engine/Graphics/DirectX11/D3DUAV.h"
#include "../../Engine/Graphics/DirectX11/D3DSRV.h"

class BoidHandler
{
private:
	static const unsigned int NUM_BOIDS = 32;
	static const int PLAY_HALF_VOLUME_SIZE = 5;

	Renderer& renderer;

	Boid boidClone;

	D3DBuffer boidVelocBuffer;
	D3DUAV boidVelocUAV;

	D3DBuffer boidBuffer;
	D3DUAV boidBufferUAV;
	D3DSRV boidBufferSRV;

	ComputeShader boidLogicShader;

	ShaderBuffer boidLogicShaderBuffer;

	struct BoidLogicBuffer
	{
		float deltaTime;

		int numBoids;

		XMFLOAT2 padding;
	} blb{};


	void createGPUBuffer();
	void debugBoidsBuffer();

	unsigned int getWangHash(unsigned int seed);
	float getWangHashFloat(unsigned int state);

public:
	BoidHandler(Renderer& renderer);
	~BoidHandler();

	void updateBoids(float deltaTime);
	void drawBoids();
};
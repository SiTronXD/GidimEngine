#pragma once

#include "Boid.h"
#include "../../Engine/Graphics/ComputeShader.h"

class BoidHandler
{
private:
	static const int NUM_BOIDS = 1024 * 1024 * 4;

	Renderer& renderer;

	Boid boidClone;

	ID3D11Buffer* boidBuffer;
	ID3D11UnorderedAccessView* boidBufferUAV;
	ID3D11ShaderResourceView* boidBufferSRV;

	ComputeShader boidLogicShader;

	ShaderBuffer boidLogicShaderBuffer;

	struct BoidLogicBuffer
	{
		float deltaTime;

		XMFLOAT3 padding;
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
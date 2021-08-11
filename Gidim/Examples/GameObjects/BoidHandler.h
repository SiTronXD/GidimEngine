#pragma once

#include "Boid.h"
#include "../../Engine/Graphics/ComputeShader.h"

class BoidHandler
{
private:
	static const int NUM_BOIDS = 2;

	Renderer& renderer;

	std::vector<Boid*> boids;
	std::vector<XMFLOAT3> boidColors;

	ID3D11Buffer* boidsBuffer;
	ID3D11UnorderedAccessView* boidsBufferUAV;
	ID3D11ShaderResourceView* boidsBufferSRV;

	ComputeShader boidsLogicShader;

	ShaderBuffer boidIDShaderBuffer;

	struct BoidIDBuffer
	{
		int id;

		XMFLOAT3 color;
	} bib{};

	void createGPUBuffer();
	void debugBoidsBuffer();

	unsigned int getWangHash(unsigned int seed);
	float getWangHashFloat(unsigned int state);

public:
	BoidHandler(Renderer& renderer);
	~BoidHandler();

	void updateBoids();
	void drawBoids();
};
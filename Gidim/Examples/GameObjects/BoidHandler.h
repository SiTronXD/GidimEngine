#pragma once

#include "Boid.h"
#include "../../Engine/Graphics/ComputeShader.h"

class BoidHandler
{
private:
	static const int NUM_BOIDS = 1;

	Renderer& renderer;

	std::vector<Boid*> boids;

	ID3D11Buffer* boidsBuffer;
	ID3D11UnorderedAccessView* boidsBufferUAV;

	ComputeShader boidsLogicShader;

	void createGPUBuffer();

public:
	BoidHandler(Renderer& renderer);
	~BoidHandler();

	void updateBoids();
	void drawBoids();
};
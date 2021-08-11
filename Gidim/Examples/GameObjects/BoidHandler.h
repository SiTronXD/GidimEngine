#pragma once

#include "Boid.h"
#include "../../Engine/Graphics/ComputeShader.h"

class BoidHandler
{
private:
	static const int NUM_BOIDS = 1;

	Renderer& renderer;

	std::vector<Boid*> boids;

	ID3D11Buffer* destDataGPUBuffer;
	ID3D11UnorderedAccessView* destDataGPUBufferView;

	ComputeShader boidsLogicShader;

public:
	BoidHandler(Renderer& renderer);
	~BoidHandler();

	void updateBoids();
	void drawBoids();
};
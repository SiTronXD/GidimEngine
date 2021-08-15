#pragma once

#include "../../ShaderHandlers/BoidRenderShader.h"
#include "../../Engine/Graphics/MeshInstancer.h"

class Boid
{
private:
	Renderer& renderer;

	BoidRenderShader shader;

	MeshData meshData;
	MeshInstancer meshInstancer;

public:
	Boid(Renderer& renderer);
	~Boid();

	void draw(unsigned int numInstances);
};
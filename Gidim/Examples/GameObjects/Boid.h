#pragma once

#include "../../ShaderHandlers/BoidRenderShader.h"
#include "../../Engine/Graphics/MeshData.h"
#include "../../Engine/Graphics/Mesh.h"

class Boid
{
private:
	Renderer& renderer;

	BoidRenderShader shader;

	MeshData meshData;
	Mesh mesh;

public:
	Boid(Renderer& renderer);
	~Boid();

	void draw();
};
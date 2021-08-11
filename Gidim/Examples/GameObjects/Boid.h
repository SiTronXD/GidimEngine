#pragma once

#include "../../ShaderHandlers/ColorShader.h"
#include "../../Engine/Graphics/MeshData.h"
#include "../../Engine/Graphics/Mesh.h"

class Boid
{
private:
	Renderer& renderer;

	ColorShader shader;

	MeshData meshData;
	Mesh mesh;

public:
	Boid(Renderer& renderer);
	~Boid();

	void draw();
};
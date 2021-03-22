#pragma once

#include "Renderer.h"
#include "Shader.h"

class Mesh
{
private:
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	Shader shader;
	Renderer& renderer;

	unsigned int vertexCount;
	unsigned int indexCount;

	bool createMesh();

public:
	Mesh(Renderer& renderer);
	~Mesh();

	void draw();
};
#pragma once

#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"

struct Vertex
{
	float x, y, z;
	float u, v;
};

class Mesh
{
private:
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	Shader shader;
	Renderer& renderer;

	XMMATRIX worldMatrix;

	unsigned int vertexCount;
	unsigned int indexCount;

	bool createBuffers(Vertex vertices[], int indices[]);

public:
	Mesh(Renderer& renderer, Vertex vertices[], int indices[], 
		unsigned int vertexCount, unsigned int indexCount);
	~Mesh();

	void setWorldMatrix(XMMATRIX newWorldMatrix);

	void draw();
};
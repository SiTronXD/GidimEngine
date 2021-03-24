#pragma once

#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "MeshData.h"

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

	bool createBuffers(MeshData& meshData);

public:
	Mesh(Renderer& renderer, MeshData& meshData);
	~Mesh();

	void setWorldMatrix(XMMATRIX newWorldMatrix);

	void draw();
};
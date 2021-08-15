#pragma once

#include "Renderer.h"
#include "MeshData.h"

class Mesh
{
private:
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	Renderer& renderer;

	XMMATRIX worldMatrix;

	unsigned int vertexCount;
	unsigned int indexCount;

	bool createBuffers(MeshData& meshData);

public:
	Mesh(Renderer& renderer, MeshData& meshData);
	~Mesh();

	void setWorldMatrix(XMMATRIX newWorldMatrix);

	void draw(bool callDrawCommand = true);

	const XMMATRIX& getWorldMatrix() const;
};
#pragma once

#include "Renderer.h"
#include "MeshData.h"

class Mesh
{
private:
	const UINT vertexStride = sizeof(Vertex);
	const UINT vertexOffset = 0;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	Renderer& renderer;

	XMMATRIX worldMatrix;

	unsigned int vertexCount;
	unsigned int indexCount;

	bool createBuffers(MeshData& meshData);

protected:
	void prepareToDraw(ID3D11DeviceContext*& deviceContext);

	const Renderer& getRenderer() const;

	ID3D11Buffer*& getVertexBuffer();
	ID3D11Buffer*& getIndexBuffer();

	const unsigned int& getVertexCount() const;
	const unsigned int& getIndexCount() const;

	const UINT& getVertexStride() const;
	const UINT& getVertexOffset() const;

public:
	Mesh(Renderer& renderer, MeshData& meshData);
	virtual ~Mesh();

	void setWorldMatrix(XMMATRIX newWorldMatrix);

	virtual void draw();

	const XMMATRIX& getWorldMatrix() const;
};
#pragma once

#include "Renderer.h"

class Mesh
{
private:
	ID3D11Buffer* vertexBuffer;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;

	Renderer& renderer;

	void createMesh();
	void createShaders();

public:
	Mesh(Renderer& renderer);
	~Mesh();

	void draw();
};
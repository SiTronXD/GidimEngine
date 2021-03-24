#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include "Renderer.h"

using namespace DirectX;

class Shader
{
private:
	// Needs 16 byte alignment
	struct MatrixBuffer
	{
		XMMATRIX projectionMatrix;	// 64 bytes
		XMMATRIX viewMatrix;		// 64 bytes
		XMMATRIX worldMatrix;		// 64 bytes
	};

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* matrixBuffer;

public:
	Shader();
	~Shader();

	bool loadFromFile(
		ID3D11Device* device, 
		std::string vertexShaderFilePath, 
		std::string pixelShaderFilePath
	);

	void update(
		Renderer& renderer, XMMATRIX currentWorldMatrix
	);
	void set(ID3D11DeviceContext* context);
};
#pragma once

#include "../../pch.h"
#include "Renderer.h"
#include "ShaderBuffer.h"

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
	} matrixBufferValues{};

	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;

	ID3D11DeviceContext* deviceContext;

	ShaderBuffer matrixBuffer;

	bool loadFromFile(
		ID3D11Device* device,
		std::string vertexShaderFilePath,
		std::string pixelShaderFilePath
	);

public:
	Shader(Renderer& renderer, std::string vertexShaderFilePath, std::string pixelShaderFilePath);
	virtual ~Shader();

	virtual void update(
		Renderer& renderer, XMMATRIX currentWorldMatrix
	);
	virtual void set();
};
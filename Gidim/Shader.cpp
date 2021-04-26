#include "Shader.h"
#include "Log.h"
#include <fstream>
#include <vector>

Shader::Shader(Renderer& renderer)
	: vertexShader(nullptr), pixelShader(nullptr), inputLayout(nullptr), 
	matrixBuffer(renderer.getDevice(), sizeof(MatrixBuffer))
{
	
}

Shader::~Shader()
{
	S_RELEASE(this->vertexShader);
	S_RELEASE(this->pixelShader);
	S_RELEASE(this->inputLayout);
}

bool Shader::loadFromFile(
	ID3D11Device* device, 
	std::string vertexShaderFilePath, std::string pixelShaderFilePath
)
{
	HRESULT result;

	// Open vertex shader file
	std::ifstream vsFile(vertexShaderFilePath, std::ios::binary);

	// Check if the file could not be found or opened
	if (vsFile.fail())
	{
		Log::error("Could not open vertex shader file from: " + vertexShaderFilePath);

		return false;
	}
	std::vector<char> vsData =
	{
		std::istreambuf_iterator<char>(vsFile),
		std::istreambuf_iterator<char>()
	};


	// Open pixel shader file
	std::ifstream psFile(pixelShaderFilePath, std::ios::binary);

	// Check if the file could not be found or opened
	if (psFile.fail())
	{
		Log::error("Could not open pixel shader file from: " + pixelShaderFilePath);

		return false;
	}
	std::vector<char> psData =
	{
		std::istreambuf_iterator<char>(psFile),
		std::istreambuf_iterator<char>()
	};


	// Create vertex shader
	result = device->CreateVertexShader(
		vsData.data(), vsData.size(), nullptr, &this->vertexShader
	);
	if (FAILED(result))
	{
		Log::error("Failed calling device->CreateVertexShader.");

		return false;
	}

	// Create pixel shader
	result = device->CreatePixelShader(
		psData.data(), psData.size(), nullptr, &this->pixelShader
	);
	if (FAILED(result))
	{
		Log::error("Failed calling device->CreatePixelShader().");

		return false;
	}

	// Create input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	unsigned int numLayoutElements = sizeof(layout) / sizeof(layout[0]);

	result = device->CreateInputLayout(layout, numLayoutElements, vsData.data(), vsData.size(), &this->inputLayout);
	if (FAILED(result))
	{
		Log::error("Failed calling device->CreateInputLayout().");

		return false;
	}

	return true;
}

void Shader::update(Renderer& renderer, XMMATRIX currentWorldMatrix)
{
	// Transpose matrices
	XMMATRIX projectionMatrix = XMMatrixTranspose(renderer.getProjectionMatrix());
	XMMATRIX viewMatrix = XMMatrixTranspose(renderer.getViewMatrix());
	XMMATRIX worldMatrix = XMMatrixTranspose(currentWorldMatrix);

	ID3D11DeviceContext* deviceContext = renderer.getDeviceContext();

	// Lock the constant buffer so it can be written to
	D3D11_MAPPED_SUBRESOURCE mappedSubResource;
	this->matrixBuffer.map(deviceContext, mappedSubResource);

	// Get a pointer to the data in the constant buffer
	MatrixBuffer* dataPtr = (MatrixBuffer*) mappedSubResource.pData;
	dataPtr->projectionMatrix = projectionMatrix;
	dataPtr->viewMatrix = viewMatrix;
	dataPtr->worldMatrix = worldMatrix;

	// Unlock the constant buffer
	this->matrixBuffer.unmap(deviceContext);
}

void Shader::set(ID3D11DeviceContext* context)
{
	// Set the current constant buffer in the vertex shader
	ID3D11Buffer* buf = matrixBuffer.getBuffer();
	context->VSSetConstantBuffers(0, 1, &buf);

	// Set current input layout
	context->IASetInputLayout(this->inputLayout);

	// Set current vertex and pixel shaders
	context->VSSetShader(this->vertexShader, nullptr, 0);
	context->PSSetShader(this->pixelShader, nullptr, 0);
}

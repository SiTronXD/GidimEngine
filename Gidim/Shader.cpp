#include "Shader.h"
#include "Log.h"
#include "SDXHelpers.h"
#include <fstream>
#include <vector>

Shader::Shader()
	: vertexShader(nullptr), pixelShader(nullptr), inputLayout(nullptr), 
	matrixBuffer(nullptr)
{
	
}

Shader::~Shader()
{
	S_RELEASE(this->matrixBuffer);
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
		Log::error("Could not open shader file from: " + vertexShaderFilePath);

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
		Log::error("Could not open shader file from: " + pixelShaderFilePath);

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
		Log::error("Failed calling device->CreatePixelShader.");

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
		Log::error("Failed calling device->CreateInputLayout.");

		return false;
	}


	// Set description for the dynamic constant buffer that is in the vertex shader
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the
	// vertex shader constant buffer from the class
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
	if (FAILED(result))
	{
		Log::error("Failed creating matrix buffer.");

		return false;
	}


	return true;
}

void Shader::update(Renderer& renderer, XMMATRIX currentWorldMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedSubResource;

	// Transpose matrices
	XMMATRIX projectionMatrix = XMMatrixTranspose(renderer.getProjectionMatrix());
	XMMATRIX viewMatrix = XMMatrixTranspose(renderer.getViewMatrix());
	XMMATRIX worldMatrix = XMMatrixTranspose(currentWorldMatrix);

	// Lock the constant buffer so it can be written to
	ID3D11DeviceContext* deviceContext = renderer.getDeviceContext();
	result = deviceContext->Map(
		this->matrixBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubResource
	);
	if (FAILED(result))
	{
		Log::error("Could not map matrix buffer.");
	}

	// Get a pointer to the data in the constant buffer
	MatrixBuffer* dataPtr = (MatrixBuffer*) mappedSubResource.pData;
	dataPtr->projectionMatrix = projectionMatrix;
	dataPtr->viewMatrix = viewMatrix;
	dataPtr->worldMatrix = worldMatrix;

	// Unlock the constant buffer
	deviceContext->Unmap(this->matrixBuffer, NULL);
}

void Shader::set(ID3D11DeviceContext* context)
{
	// Set the current constant buffer in the vertex shader
	context->VSSetConstantBuffers(0, 1, &matrixBuffer);

	// Set current input layout
	context->IASetInputLayout(this->inputLayout);

	// Set current vertex and pixel shaders
	context->VSSetShader(this->vertexShader, nullptr, 0);
	context->PSSetShader(this->pixelShader, nullptr, 0);
}

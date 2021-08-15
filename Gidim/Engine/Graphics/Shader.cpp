#include "Shader.h"
#include "../Dev/Log.h"

bool Shader::loadFromFile(
	ID3D11Device* device,
	std::string vertexShaderFilePath, std::string pixelShaderFilePath,
	std::vector<D3D11_INPUT_ELEMENT_DESC>& layout
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
		Log::error("Failed creating vertex shader.");

		return false;
	}

	// Create pixel shader
	result = device->CreatePixelShader(
		psData.data(), psData.size(), nullptr, &this->pixelShader
	);
	if (FAILED(result))
	{
		Log::error("Failed creating pixel shader.");

		return false;
	}

	// Create input layout
	result = device->CreateInputLayout(layout.data(), layout.size(), vsData.data(), vsData.size(), &this->inputLayout);
	if (FAILED(result))
	{
		Log::error("Failed creating input layout.");

		return false;
	}

	return true;
}

// Use default input layout
Shader::Shader(Renderer& renderer,
	std::string vertexShaderFilePath,
	std::string pixelShaderFilePath
)
	: vertexShader(nullptr), pixelShader(nullptr), inputLayout(nullptr),
	deviceContext(renderer.getDeviceContext()),
	matrixBuffer(renderer, sizeof(MatrixBuffer))
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> defaultLayout
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	this->loadFromFile(
		renderer.getDevice(),
		vertexShaderFilePath,
		pixelShaderFilePath,
		defaultLayout
	);
}

// Use custom input layout
Shader::Shader(Renderer& renderer, 
	std::string vertexShaderFilePath, 
	std::string pixelShaderFilePath,
	std::vector<D3D11_INPUT_ELEMENT_DESC> layout
	)
	: vertexShader(nullptr), pixelShader(nullptr), inputLayout(nullptr), 
	deviceContext(renderer.getDeviceContext()), 
	matrixBuffer(renderer, sizeof(MatrixBuffer))
{
	this->loadFromFile(
		renderer.getDevice(), 
		vertexShaderFilePath, 
		pixelShaderFilePath, 
		layout
	);
}

Shader::~Shader()
{
	S_RELEASE(this->vertexShader);
	S_RELEASE(this->pixelShader);
	S_RELEASE(this->inputLayout);
}

void Shader::update(Renderer& renderer, XMMATRIX currentWorldMatrix)
{
	// Transpose matrices
	XMMATRIX projectionMatrix = XMMatrixTranspose(renderer.getProjectionMatrix());
	XMMATRIX viewMatrix = XMMatrixTranspose(renderer.getViewMatrix());
	XMMATRIX worldMatrix = XMMatrixTranspose(currentWorldMatrix);

	// Update values in the structure before passing it to the shader
	this->matrixBufferValues.projectionMatrix = projectionMatrix;
	this->matrixBufferValues.viewMatrix = viewMatrix;
	this->matrixBufferValues.worldMatrix = worldMatrix;

	// Pass values to the shader buffer
	this->matrixBuffer.update(&this->matrixBufferValues);
}

void Shader::set()
{
	// Set the current constant buffer in the vertex shader
	matrixBuffer.setVS();

	// Set current input layout
	this->deviceContext->IASetInputLayout(this->inputLayout);

	// Set current vertex and pixel shaders
	this->deviceContext->VSSetShader(this->vertexShader, nullptr, 0);
	this->deviceContext->PSSetShader(this->pixelShader, nullptr, 0);
}

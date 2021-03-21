#include "Mesh.h"
#include <fstream>
#include <vector>

struct Vertex
{
	float x, y;
	float r, g, b;
};

void Mesh::createMesh()
{
	// Actual vertices
	Vertex vertices[] =
	{
		{ -1.0f, -1.0f,		1.0f, 0.0f, 0.0f },
		{  0.0f,  1.0f,		0.0f, 1.0f, 0.0f },
		{  1.0f, -1.0f,		0.0f, 0.0f, 1.0f }
	};

	// Create vertex buffer desc
	CD3D11_BUFFER_DESC vertexBufferDesc = CD3D11_BUFFER_DESC(sizeof(vertices), D3D11_BIND_VERTEX_BUFFER);
	
	// Create sub resource data for vertex data
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = vertices;

	// Create vertex buffer
	renderer.getDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &this->vertexBuffer);
}

void Mesh::createShaders()
{
	// Load vertex shader
	std::ifstream vsFile("DefaultShader_Vert.cso", std::ios::binary);
	std::vector<char> vsData = 
	{ 
		std::istreambuf_iterator<char>(vsFile), 
		std::istreambuf_iterator<char>() 
	};

	// Load pixel shader
	std::ifstream psFile("DefaultShader_Pix.cso", std::ios::binary);
	std::vector<char> psData =
	{
		std::istreambuf_iterator<char>(psFile),
		std::istreambuf_iterator<char>()
	};

	// Create shaders
	renderer.getDevice()->CreateVertexShader(
		vsData.data(), vsData.size(), nullptr, &this->vertexShader
	);
	renderer.getDevice()->CreatePixelShader(
		psData.data(), psData.size(), nullptr, &this->pixelShader
	);

	// Create input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	renderer.getDevice()->CreateInputLayout(layout, 2, vsData.data(), vsData.size(), &this->inputLayout);
}

Mesh::Mesh(Renderer& renderer)
	: renderer(renderer), vertexBuffer(nullptr), vertexShader(nullptr), 
	pixelShader(nullptr), inputLayout(nullptr)
{
	this->createMesh();
	this->createShaders();
}

Mesh::~Mesh()
{
	if (this->vertexBuffer)
	{
		this->vertexBuffer->Release();
		this->vertexBuffer = nullptr;
	}

	if (this->vertexShader)
	{
		this->vertexShader->Release();
		this->vertexShader = nullptr;
	}

	if (this->pixelShader)
	{
		this->pixelShader->Release();
		this->pixelShader = nullptr;
	}

	if (this->inputLayout)
	{
		this->inputLayout->Release();
		this->inputLayout = nullptr;
	}
}

void Mesh::draw()
{
	ID3D11DeviceContext* deviceContext = renderer.getDeviceContext();

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(this->inputLayout);

	// Bind the triangle shaders
	deviceContext->VSSetShader(this->vertexShader, nullptr, 0);
	deviceContext->PSSetShader(this->pixelShader, nullptr, 0);

	// Bind the vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);

	// Draw
	deviceContext->Draw(3, 0);
}

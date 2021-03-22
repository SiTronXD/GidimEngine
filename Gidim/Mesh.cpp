#include "Mesh.h"

struct Vertex
{
	float x, y;
	float r, g, b;
};

bool Mesh::createMesh()
{
	HRESULT result;

	// Actual vertices
	Vertex vertices[] =
	{
		{ -1.0f, -1.0f,		1.0f, 0.0f, 0.0f },
		{  0.0f,  1.0f,		0.0f, 1.0f, 0.0f },
		{  1.0f, -1.0f,		0.0f, 0.0f, 1.0f }
	};
	this->vertexCount = sizeof(vertices) / sizeof(vertices[0]);

	// Has to be in clock-wise order for front faces
	int indices[]{ 0, 1, 2 };
	this->indexCount = sizeof(indices) / sizeof(indices[0]);


	// Create vertex buffer desc
	CD3D11_BUFFER_DESC vertexBufferDesc = CD3D11_BUFFER_DESC(
		sizeof(vertices), D3D11_BIND_VERTEX_BUFFER
	);
	
	// Create sub resource data containing pointer to vertex data
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;			// Distance in bytes from one line to the next one (2D/3D textures)
	vertexData.SysMemSlicePitch = 0;	// Distance in bytes from one depth level to the next one (3D textures)

	// Create vertex buffer
	result = renderer.getDevice()->CreateBuffer(
		&vertexBufferDesc, &vertexData, &this->vertexBuffer
	);
	if (FAILED(result))
	{
		Log::error("Could not create vertex buffer.");

		return false;
	}

	// Create index buffer desc
	CD3D11_BUFFER_DESC indexBufferDesc = CD3D11_BUFFER_DESC(
		sizeof(indices), D3D11_BIND_INDEX_BUFFER
	);

	// Create sub resource data containing pointer to index data
	D3D11_SUBRESOURCE_DATA indexData = { 0 };
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer
	result = renderer.getDevice()->CreateBuffer(
		&indexBufferDesc, &indexData, &this->indexBuffer
	);
	if (FAILED(result))
	{
		Log::error("Could not create index buffer.");

		return false;
	}

	return true;
}

Mesh::Mesh(Renderer& renderer)
	: renderer(renderer), vertexBuffer(nullptr)
{
	this->createMesh();

	this->shader.loadFromFile(
		this->renderer.getDevice(), 
		"DefaultShader_Vert.cso", 
		"DefaultShader_Pix.cso"
	);
}

Mesh::~Mesh()
{
	this->indexBuffer->Release();
	this->vertexBuffer->Release();
}

void Mesh::draw()
{
	ID3D11DeviceContext* deviceContext = renderer.getDeviceContext();

	// Set topology
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	XMMATRIX projectionMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX worldMatrix = XMMatrixIdentity();
	//worldMatrix = XMMatrixScaling(0.2f, 0.5f, 0.5f);

	// Update buffers in shader
	this->shader.update(renderer);

	// Set shader
	this->shader.bind(deviceContext);

	// Set the vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);

	// Set the index buffer
	deviceContext->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R32_UINT, 0);


	// Draw
	deviceContext->DrawIndexed(this->indexCount, 0, 0);
}

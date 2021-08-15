#include "Mesh.h"
#include "../Dev/Helpers.h"

bool Mesh::createBuffers(MeshData& meshData)
{
	HRESULT result;

	// Make sure there are vertices to create buffers from
	if (meshData.getVertices().empty())
	{
		Log::error("MeshData contained no vertices.");

		return false;
	}

	// Create vertex buffer desc
	CD3D11_BUFFER_DESC vertexBufferDesc = CD3D11_BUFFER_DESC(
		sizeof(meshData.getVertices()[0]) * this->vertexCount, D3D11_BIND_VERTEX_BUFFER
	);

	// Create sub resource data containing pointer to vertex data
	D3D11_SUBRESOURCE_DATA vertexData = { 0 };
	vertexData.pSysMem = &meshData.getVertices()[0];
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
		sizeof(meshData.getIndices()[0]) * this->indexCount, D3D11_BIND_INDEX_BUFFER
	);

	// Create sub resource data containing pointer to index data
	D3D11_SUBRESOURCE_DATA indexData = { 0 };
	indexData.pSysMem = &meshData.getIndices()[0];
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

Mesh::Mesh(Renderer& renderer, MeshData& meshData)
	: renderer(renderer), vertexBuffer(nullptr), 
	vertexCount(meshData.getVertices().size()), 
	indexCount(meshData.getIndices().size())
{
	// Make sure indices are multiple of 3
	int remainingIndices = meshData.getIndices().size() % 3;
	if (remainingIndices != 0)
	{
		Log::warning(
			std::to_string(meshData.getIndices().size()) +
			" indices is not a multiple of 3. Removing the last " +
			std::to_string(remainingIndices) + " indices."
		);

		// Remove overflowing indices
		meshData.getIndices().resize(meshData.getIndices().size() - remainingIndices);
	}


	// Create vertex buffers and index buffers
	this->createBuffers(meshData);

	
	// Initialize world matrix to identity matrix
	this->setWorldMatrix(XMMatrixIdentity());
}

Mesh::~Mesh()
{
	S_RELEASE(this->indexBuffer);
	S_RELEASE(this->vertexBuffer);
}

void Mesh::setWorldMatrix(XMMATRIX newWorldMatrix)
{
	this->worldMatrix = newWorldMatrix;
}

void Mesh::draw(bool callDrawCommand)
{
	ID3D11DeviceContext* deviceContext = renderer.getDeviceContext();

	// Set topology
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	// Set the vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);

	// Set the index buffer
	deviceContext->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R32_UINT, 0);


	// Draw
	if(callDrawCommand)
		deviceContext->DrawIndexed(this->indexCount, 0, 0);
}

const XMMATRIX& Mesh::getWorldMatrix() const
{
	return this->worldMatrix;
}

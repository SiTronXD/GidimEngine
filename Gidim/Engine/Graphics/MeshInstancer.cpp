#include "MeshInstancer.h"

MeshInstancer::MeshInstancer(Renderer& renderer, MeshData& meshData)
	: Mesh(renderer, meshData)
{
}

MeshInstancer::~MeshInstancer()
{
}

void MeshInstancer::draw(unsigned int numInstances)
{
	ID3D11DeviceContext* deviceContext = Mesh::getRenderer().getDeviceContext();

	// Set buffers
	Mesh::prepareToDraw(deviceContext);

	// Draw instances
	deviceContext->DrawIndexedInstanced(
		Mesh::getIndexCount(), numInstances,
		0, 0, 0
	);
}

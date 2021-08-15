#include "D3DBuffer.h"

D3DBuffer::D3DBuffer(Renderer& renderer, std::string debugName)
	: buffer(nullptr), renderer(renderer), debugName(debugName)
{
}

D3DBuffer::~D3DBuffer()
{
	S_RELEASE(this->buffer);
}

void D3DBuffer::createBuffer(
	UINT bindFlags,
	UINT elementSize,
	UINT numElements,
	D3D11_SUBRESOURCE_DATA& initialBufferData
)
{
	// Deallocate old buffer
	S_RELEASE(this->buffer);

	// Init desc to zero
	D3D11_BUFFER_DESC descGPUBuffer;
	ZeroMemory(&descGPUBuffer, sizeof(descGPUBuffer));

	// Fill desc
	descGPUBuffer.BindFlags = bindFlags;
	descGPUBuffer.ByteWidth = elementSize * numElements;
	descGPUBuffer.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	descGPUBuffer.StructureByteStride = elementSize;

	// Create buffer
	HRESULT result = this->renderer.getDevice()->CreateBuffer(&descGPUBuffer, &initialBufferData, &this->buffer);
	if (FAILED(result))
	{
		Log::resultFailed("Failed creating buffer: " + this->debugName + ".", result);
	}
}

D3D11_BUFFER_DESC D3DBuffer::getDesc()
{
	// Create empty desc
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	// Get desc
	this->buffer->GetDesc(&desc);

	return desc;
}

ID3D11Buffer* D3DBuffer::getBuffer() const
{
	return this->buffer;
}

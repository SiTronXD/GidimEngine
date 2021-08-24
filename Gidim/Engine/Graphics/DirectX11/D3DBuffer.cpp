#include "D3DBuffer.h"

D3DBuffer::D3DBuffer(Renderer& renderer, std::string debugName)
	: buffer(nullptr), renderer(renderer), debugName(debugName)
{
}

D3DBuffer::~D3DBuffer()
{
	S_RELEASE(this->buffer);
}

void D3DBuffer::createStructuredBuffer(
	UINT bindFlags,
	UINT elementSize,
	UINT numElements,
	D3D11_SUBRESOURCE_DATA* initialBufferData
)
{
	// Set number of elements
	this->numElements = numElements;

	// Deallocate old buffer
	S_RELEASE(this->buffer);

	// Init desc to zero
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	// Fill desc
	bufferDesc.BindFlags = bindFlags;
	bufferDesc.ByteWidth = elementSize * numElements;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = elementSize;

	// Create buffer
	HRESULT result = this->renderer.getDevice()->CreateBuffer(&bufferDesc, initialBufferData, &this->buffer);
	if (FAILED(result))
	{
		Log::resultFailed("Failed creating buffer: " + this->debugName + ".", result);
	}
}

void D3DBuffer::createConstantBuffer(UINT structSize)
{
	// Deallocate old buffer
	S_RELEASE(this->buffer);

	// Init desc to zero
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));

	// Fill desc
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = structSize;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	// Create buffer
	HRESULT result = this->renderer.getDevice()->CreateBuffer(&bufferDesc, NULL, &this->buffer);
	if (FAILED(result))
	{
		Log::resultFailed("Failed creating constant buffer: " + this->debugName + ".", result);
	}
}

void D3DBuffer::map(
	D3D11_MAPPED_SUBRESOURCE& inputMappedSubresource
)
{
	HRESULT result = this->renderer.getDeviceContext()->Map(
		this->buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &inputMappedSubresource
	);
	if (FAILED(result))
	{
		Log::error("Could not map buffer.");
	}
}

void D3DBuffer::unmap()
{
	this->renderer.getDeviceContext()->Unmap(this->buffer, NULL);
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
#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer(Renderer& renderer, UINT structSize)
	: buffer(renderer, "constantBufferWrapper"), mappedSubresource{ NULL },
	deviceContext(renderer.getDeviceContext()), structSize(structSize) 
{
	this->buffer.createConstantBuffer(structSize);
}

ConstantBuffer::~ConstantBuffer() {}

void ConstantBuffer::update(void* bufferStruct)
{
	buffer.map(this->mappedSubresource);

	// Copy over data to the subresource
	memcpy(mappedSubresource.pData, bufferStruct, structSize);

	buffer.unmap();
}

void ConstantBuffer::setVS(UINT slot)
{
	ID3D11Buffer* buf = this->buffer.getBuffer();
	deviceContext->VSSetConstantBuffers(slot, 1, &buf);
}

void ConstantBuffer::setPS(UINT slot)
{
	ID3D11Buffer* buf = this->buffer.getBuffer();
	deviceContext->PSSetConstantBuffers(slot, 1, &buf);
}
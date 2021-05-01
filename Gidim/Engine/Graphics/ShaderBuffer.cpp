#include "ShaderBuffer.h"

ShaderBuffer::ShaderBuffer(Renderer& renderer, UINT structSize)
	: buffer(renderer.getDevice(), structSize), mappedSubresource{ NULL },
	deviceContext(renderer.getDeviceContext()), structSize(structSize) {}

ShaderBuffer::~ShaderBuffer() {}

void ShaderBuffer::update(void* bufferStruct)
{
	buffer.map(this->deviceContext, this->mappedSubresource);

	// Copy over data to the subresource
	memcpy(mappedSubresource.pData, bufferStruct, structSize);

	buffer.unmap(this->deviceContext);
}

void ShaderBuffer::setVS(UINT slot)
{
	ID3D11Buffer* buf = this->buffer.getBuffer();
	deviceContext->VSSetConstantBuffers(slot, 1, &buf);
}

void ShaderBuffer::setPS(UINT slot)
{
	ID3D11Buffer* buf = this->buffer.getBuffer();
	deviceContext->PSSetConstantBuffers(slot, 1, &buf);
}

const SDXBuffer& ShaderBuffer::getSDXBuffer() const
{
	return this->buffer;
}

ID3D11Buffer* ShaderBuffer::getBuffer() const
{
	return this->buffer.getBuffer();
}

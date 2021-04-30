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

const SDXBuffer& ShaderBuffer::getSDXBuffer() const
{
	return this->buffer;
}

ID3D11Buffer* ShaderBuffer::getBuffer() const
{
	return this->buffer.getBuffer();
}

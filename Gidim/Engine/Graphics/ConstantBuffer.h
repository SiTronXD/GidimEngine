#pragma once

#include "DirectX11/D3DBuffer.h"

class ConstantBuffer
{
private:
	D3DBuffer buffer;

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;

	ID3D11DeviceContext* deviceContext;

	size_t structSize;

public:
	ConstantBuffer(Renderer& renderer, UINT structSize);
	~ConstantBuffer();

	void update(void* bufferStruct);
	void setVS(UINT slot = 0);
	void setPS(UINT slot = 0);

	inline ID3D11Buffer* getBuffer() const { return this->buffer.getBuffer(); };
};
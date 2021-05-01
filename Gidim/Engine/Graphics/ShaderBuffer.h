#pragma once

#include "DirectX11/SDXBuffer.h"

class ShaderBuffer
{
private:
	SDXBuffer buffer;

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;

	ID3D11DeviceContext* deviceContext;

	size_t structSize;

public:
	ShaderBuffer(Renderer& renderer, UINT structSize);
	~ShaderBuffer();

	void update(void* bufferStruct);

	const SDXBuffer& getSDXBuffer() const;
	ID3D11Buffer* getBuffer() const;
};
#pragma once

#include "Renderer.h"

class SDXBuffer
{
private:
	ID3D11Buffer* buffer;

public:
	SDXBuffer(ID3D11Device* device, UINT structSize);
	~SDXBuffer();

	void map(ID3D11DeviceContext* deviceContext, D3D11_MAPPED_SUBRESOURCE& inputMappedSubResource);
	void unmap(ID3D11DeviceContext* deviceContext);

	ID3D11Buffer* getBuffer() const;
};
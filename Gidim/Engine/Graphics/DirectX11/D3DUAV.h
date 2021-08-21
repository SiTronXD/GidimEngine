#pragma once

#include "../../../pch.h"
#include "../Renderer.h"

class D3DUAV
{
private:
	ID3D11UnorderedAccessView* bufferUAV;

	Renderer& renderer;

	std::string debugName;

public:
	D3DUAV(Renderer& renderer, std::string debugName);
	~D3DUAV();

	void createUAV(
		ID3D11Resource* buffer,
		DXGI_FORMAT format,
		D3D11_UAV_DIMENSION viewDimension,
		UINT numElements
	);

	inline ID3D11UnorderedAccessView* getUAV() const { return this->bufferUAV; }
};
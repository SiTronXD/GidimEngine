#pragma once

#include "../../../pch.h"
#include "../Renderer.h"
#include "D3DBuffer.h"

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
		D3DBuffer& buffer,
		DXGI_FORMAT format,
		D3D11_UAV_DIMENSION viewDimension
	);

	inline ID3D11UnorderedAccessView* getUAV() const { return this->bufferUAV; }
};
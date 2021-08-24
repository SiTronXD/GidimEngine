#pragma once

#include "../../../pch.h"
#include "../Renderer.h"
#include "D3DBuffer.h"

class D3DSRV
{
private:
	ID3D11ShaderResourceView* bufferSRV;

	Renderer& renderer;

	std::string debugName;

public:
	D3DSRV(Renderer& renderer, std::string debugName);
	~D3DSRV();

	void createSRV(
		D3DBuffer& buffer,
		DXGI_FORMAT format,
		D3D11_SRV_DIMENSION viewDimension
	);

	void setVS(UINT startSlot = 0);
};
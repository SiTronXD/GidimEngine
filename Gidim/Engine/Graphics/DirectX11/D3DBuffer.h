#pragma once

#include "../../../pch.h"
#include "../Renderer.h"

class D3DBuffer
{
private:
	ID3D11Buffer* buffer;

	Renderer& renderer;

	std::string debugName;

public:
	D3DBuffer(Renderer& renderer, std::string debugName);
	~D3DBuffer();

	void createBuffer(
		UINT bindFlags,
		UINT elementSize,
		UINT numElements,
		D3D11_SUBRESOURCE_DATA& initialBufferData
	);

	D3D11_BUFFER_DESC getDesc();

	ID3D11Buffer* getBuffer() const;
};
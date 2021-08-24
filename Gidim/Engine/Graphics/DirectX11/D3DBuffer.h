#pragma once

#include "../../../pch.h"
#include "../Renderer.h"

class D3DBuffer
{
private:
	ID3D11Buffer* buffer;

	Renderer& renderer;

	UINT numElements;

	std::string debugName;

public:
	D3DBuffer(Renderer& renderer, std::string debugName);
	~D3DBuffer();

	void createStructuredBuffer(
		UINT bindFlags,
		UINT elementSize,
		UINT numElements,
		D3D11_SUBRESOURCE_DATA* initialBufferData = nullptr
	);
	void createConstantBuffer(UINT structSize);

	void map(
		D3D11_MAPPED_SUBRESOURCE& inputMappedSubresource
	);
	void unmap();

	D3D11_BUFFER_DESC getDesc();

	inline ID3D11Buffer* getBuffer() const { return this->buffer; }
	inline const UINT& getNumElements() const { return this->numElements; }
};
#pragma once

#include "Renderer.h"

class CubeMap
{
private:
	ID3D11SamplerState* samplerState;
	ID3D11Texture2D* texture;

	ID3D11UnorderedAccessView* textureUAV;

	ID3D11ShaderResourceView* textureSRV;

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	bool createSampler();

public:
	CubeMap(Renderer& renderer);
	~CubeMap();

	void setPS();

	bool createSRVasRenderTexture();

	ID3D11UnorderedAccessView* getTextureUAV() const;
};
#pragma once

#include <iostream>
#include <string>
#include "Renderer.h"

class Texture
{
private:
	ID3D11SamplerState* samplerState;

	ID3D11Texture2D* texture;
	ID3D11UnorderedAccessView* textureUAV;
	ID3D11ShaderResourceView* textureSRV;

	bool createSamplerState(ID3D11Device* device);

public:
	Texture(Renderer& renderer);
	~Texture();

	void set(Renderer& renderer, UINT startSlot = 0);

	bool createAsRenderTexture(Renderer& renderer, unsigned int width, unsigned int height);
	bool createFromFile(Renderer& renderer, std::string path);
	bool recreateSRVAsRenderTexture(Renderer& renderer);

	ID3D11ShaderResourceView* getTextureSRV() const;
	ID3D11UnorderedAccessView* getTextureUAV() const;
};
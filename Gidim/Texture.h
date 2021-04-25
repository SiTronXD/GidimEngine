#pragma once

#include <iostream>
#include <string>
#include "Renderer.h"

enum class TextureFilter
{
	BILINEAR,
	NEAREST_NEIGHBOR
};

class Texture
{
private:
	ID3D11SamplerState* samplerState;

	ID3D11Texture2D* texture;
	ID3D11UnorderedAccessView* textureUAV;
	ID3D11ShaderResourceView* textureSRV;

	DXGI_FORMAT textureFormat;

	bool createSamplerState(ID3D11Device* device, TextureFilter filter);

public:
	Texture(Renderer& renderer, TextureFilter filter = TextureFilter::BILINEAR, 
		DXGI_FORMAT textureFormat = DXGI_FORMAT_R8G8B8A8_UNORM);
	~Texture();

	void set(Renderer& renderer, UINT startSlot = 0);

	bool createAsRenderTexture(Renderer& renderer, unsigned int width, unsigned int height);
	bool createFromFile(Renderer& renderer, std::string path);
	bool recreateSRVAsRenderTexture(Renderer& renderer);

	ID3D11ShaderResourceView* getTextureSRV() const;
	ID3D11UnorderedAccessView* getTextureUAV() const;
};
#pragma once

#include "../../pch.h"
#include "Renderer.h"

enum class TextureFilter
{
	BILINEAR = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
	NEAREST_NEIGHBOR = D3D11_FILTER_MIN_MAG_MIP_POINT
};

enum class TextureFormat
{
	R8G8B8A8_UNORM = DXGI_FORMAT_R8G8B8A8_UNORM,

	R16G16B16A16_FLOAT = DXGI_FORMAT_R16G16B16A16_FLOAT,
	R16G16B16A16_UNORM = DXGI_FORMAT_R16G16B16A16_UNORM,

	R32G32B32A32_FLOAT = DXGI_FORMAT_R32G32B32A32_FLOAT
};

class Texture
{
private:
	ID3D11SamplerState* samplerState;

	ID3D11Texture2D* texture;
	ID3D11UnorderedAccessView* textureUAV;
	ID3D11ShaderResourceView* textureSRV;

	DXGI_FORMAT textureFormat;

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	bool createSamplerState(TextureFilter filter);

public:
	Texture(Renderer& renderer, TextureFilter filter = TextureFilter::BILINEAR, 
		TextureFormat textureFormat = TextureFormat::R8G8B8A8_UNORM);
	~Texture();

	void setPS(UINT startSlot = 0);
	void setVS(UINT startSlot = 0);
	void clearRenderTexture(float red, float green, float blue, float alpha);

	bool createAsRenderTexture(unsigned int width, unsigned int height);
	bool createFromFile(std::string path);
	bool createSRVAsRenderTexture();

	ID3D11ShaderResourceView* getTextureSRV() const;
	ID3D11UnorderedAccessView* getTextureUAV() const;
};
#pragma once

#include "Texture.h"

class CubeMap : public Texture
{
private:
	// These functions does not have or need an implementation
	// at the moment.
	using Texture::clearRenderTexture;
	using Texture::createFromFile;

public:
	CubeMap(Renderer& renderer, TextureFilter filter = TextureFilter::BILINEAR,
		TextureFormat textureFormat = TextureFormat::R8G8B8A8_UNORM,
		TextureEdgeSampling textureEdgeSampling = TextureEdgeSampling::CLAMP);
	~CubeMap();

	virtual bool createAsRenderTexture(unsigned int faceWidth, unsigned int faceHeight) override;
	virtual bool createSRV() override;
};
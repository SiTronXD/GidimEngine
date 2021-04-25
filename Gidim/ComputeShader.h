#pragma once

#include <vector>
#include "Renderer.h"
#include "Texture.h"

class ComputeShader
{
private:
	static const int NUM_MAX_RENDER_TEXTURES = 4;

	ID3D11ComputeShader* computeShader;

	// Array filled with NULL for resetting current UAVs in compute shader after dispatch
	ID3D11UnorderedAccessView* uavNULL[NUM_MAX_RENDER_TEXTURES];

	std::vector<Texture*> renderTextures;
	std::vector<ID3D11UnorderedAccessView*> renderTextureUAVs;

	int threadGroupX;
	int threadGroupY;

public:
	ComputeShader(int threadGroupX, int threadGroupY);
	~ComputeShader();

	bool createFromFile(Renderer& renderer, std::string path);

	void run(Renderer& renderer);
	void addRenderTexture(Texture& texture);
};
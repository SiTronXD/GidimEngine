#pragma once

#include "../../pch.h"
#include "Renderer.h"
#include "Texture.h"
#include "CubeMap.h"
#include "ShaderBuffer.h"

class ComputeShader
{
private:
	static const int NUM_MAX_RENDER_TEXTURES = 4;
	static const int NUM_MAX_CONSTANT_BUFFERS = 2;

	ID3D11ComputeShader* computeShader;

	// Arrays filled with NULL for resetting current views in compute shader after dispatch
	ID3D11UnorderedAccessView* uavNULL[NUM_MAX_RENDER_TEXTURES];
	ID3D11Buffer* constantBufferNULL[NUM_MAX_CONSTANT_BUFFERS];

	std::vector<Texture*> renderTextures;
	std::vector<CubeMap*> renderCubeMaps;
	std::vector<ID3D11UnorderedAccessView*> renderTextureUAVs;
	std::vector<ID3D11Buffer*> constantBuffers;

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	int threadGroupX;
	int threadGroupY;
	int threadGroupZ;

public:
	ComputeShader(Renderer& renderer, int threadGroupX, int threadGroupY, 
		int threadGroupZ = 1);
	~ComputeShader();

	bool createFromFile(std::string path);

	void run();
	void addRenderTexture(Texture& texture);
	void addRenderCubeMap(CubeMap& cubeMap);
	void addShaderBuffer(ShaderBuffer& buffer);

	void removeRenderTextureAt(int index);
	void removeAllRenderTextures();
};
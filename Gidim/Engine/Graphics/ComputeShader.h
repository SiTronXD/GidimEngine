#pragma once

#include "../../pch.h"
#include "Renderer.h"
#include "Texture.h"
#include "CubeMap.h"
#include "ConstantBuffer.h"

class ComputeShader
{
private:
	static const int NUM_MAX_UAV = 8;
	static const int NUM_MAX_RENDER_TEXTURES = 4;
	static const int NUM_MAX_CONSTANT_BUFFERS = 2;

	ID3D11ComputeShader* computeShader;

	// Arrays filled with NULL for resetting current views in compute shader after dispatch
	ID3D11UnorderedAccessView* uavNULL[NUM_MAX_UAV];
	ID3D11Buffer* constantBufferNULL[NUM_MAX_CONSTANT_BUFFERS];

	std::vector<Texture*> renderTextures;
	std::vector<ID3D11UnorderedAccessView*> unorderedAccessViews;
	std::vector<ID3D11Buffer*> constantBuffers;

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	int threadGroupX;
	int threadGroupY;
	int threadGroupZ;

	bool createFromFile(std::string path);

public:
	ComputeShader(Renderer& renderer, const std::string path, 
		int threadGroupX, 
		int threadGroupY = 1, 
		int threadGroupZ = 1);
	~ComputeShader();

	void run();

	void addConstantBuffer(ConstantBuffer& buffer);
	void addUAV(ID3D11UnorderedAccessView* uavToAdd);
	void addRenderTexture(Texture& texture);

	void removeRenderTextureAt(int index);
	void removeAllRenderTextures();
};
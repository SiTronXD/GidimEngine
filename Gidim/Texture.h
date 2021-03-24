#pragma once

#include <iostream>
#include <string>
#include "Renderer.h"

class Texture
{
private:
	ID3D11SamplerState* samplerState;
	ID3D11ShaderResourceView* texture;

	bool setSamplerState(ID3D11Device* device);

public:
	Texture(Renderer& renderer);
	~Texture();

	bool loadFromFile(ID3D11Device* device, std::string path);

	void set(ID3D11DeviceContext* deviceContext, UINT startSlot = 0);

	ID3D11ShaderResourceView* getTextureSRV() const;
};
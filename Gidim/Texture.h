#pragma once

#include <iostream>
#include <string>
#include <d3d11.h>

class Texture
{
private:
	ID3D11ShaderResourceView* texture;

	void release();

public:
	Texture();
	~Texture();

	bool loadFromFile(ID3D11Device* device, std::string path);

	void set(ID3D11DeviceContext* deviceContext, UINT startSlot = 0);

	ID3D11ShaderResourceView* getTextureSRV() const;
};
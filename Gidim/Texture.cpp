#include "Texture.h"
#include "External/DDSTextureLoader11.h"
#include "External/WICTextureLoader11.h"
#include "Log.h"

void Texture::release()
{
	if (this->texture)
	{
		this->texture->Release();
		this->texture = nullptr;
	}
}

Texture::Texture()
	: texture(nullptr)
{
	
}

Texture::~Texture()
{
	this->release();
}

bool Texture::loadFromFile(ID3D11Device* device, std::string path)
{
	// Deallocate old texture, if it can
	this->release();


	HRESULT result;
	ID3D11Resource* texResource = nullptr;

	// Remake path as wstring
	std::wstring widePath;
	for (int i = 0; i < path.length(); ++i)
		widePath += wchar_t(path[i]);

	// Load DDS
	if (path.substr(path.size() - 3) == "dds")
	{
		result = DirectX::CreateDDSTextureFromFile(
			device, widePath.c_str(), &texResource, &this->texture
		);
	}
	else // Load non-DDS
	{
		result = DirectX::CreateWICTextureFromFile(
			device, widePath.c_str(), &texResource, &this->texture
		);
	}

	// Did the loading fail?
	if (FAILED(result))
	{
		Log::error("Could not create texture from file.");

		return false;
	}

	// Release resource, since we only care about the SRV
	texResource->Release();

	return true;
}

void Texture::set(ID3D11DeviceContext* deviceContext, UINT startSlot)
{
	// Set shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(startSlot, 1, &this->texture);
}

ID3D11ShaderResourceView* Texture::getTextureSRV() const
{
	return this->texture;
}

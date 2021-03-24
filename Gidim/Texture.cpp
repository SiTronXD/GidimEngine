#include "Texture.h"
#include "External/DDSTextureLoader11.h"
#include "External/WICTextureLoader11.h"
#include "Log.h"
#include "SDXHelpers.h"

bool Texture::setSamplerState(ID3D11Device* device)
{
	// Create texture sampler desc for the sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state
	HRESULT result = device->CreateSamplerState(&samplerDesc, &this->samplerState);
	if (FAILED(result))
	{
		Log::error("Could not create texture sampler state.");

		return false;
	}

	return true;
}

Texture::Texture(Renderer& renderer)
	: samplerState(nullptr), texture(nullptr)
{
	this->setSamplerState(renderer.getDevice());
}

Texture::~Texture()
{
	S_RELEASE(this->samplerState);
	S_RELEASE(this->texture);
}

bool Texture::loadFromFile(ID3D11Device* device, std::string path)
{
	// Deallocate old texture, if it exists
	S_RELEASE(this->texture);


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
		Log::error("Could not create texture from file path: " + path);

		return false;
	}

	// Release resource, since we only care about the SRV
	S_RELEASE(texResource);

	return true;
}

void Texture::set(ID3D11DeviceContext* deviceContext, UINT startSlot)
{
	// Set sampler state in the pixel shader
	deviceContext->PSSetSamplers(startSlot, 1, &this->samplerState);

	// Set shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(startSlot, 1, &this->texture);
}

ID3D11ShaderResourceView* Texture::getTextureSRV() const
{
	return this->texture;
}

#include "Texture.h"
#include "External/DDSTextureLoader11.h"
#include "External/WICTextureLoader11.h"
#include "Log.h"
#include "SDXHelpers.h"

bool Texture::createSamplerState(ID3D11Device* device, TextureFilter filter)
{
	// Create texture sampler desc for the sampler state
	D3D11_SAMPLER_DESC samplerDesc;
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

	// Set filter based on options
	switch (filter)
	{
	case TextureFilter::BILINEAR:

		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		break;

	case TextureFilter::NEAREST_NEIGHBOR:

		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

		break;

	default:

		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		break;
	}


	// Create the texture sampler state
	HRESULT result = device->CreateSamplerState(&samplerDesc, &this->samplerState);
	if (FAILED(result))
	{
		Log::error("Could not create texture sampler state.");

		return false;
	}

	return true;
}

Texture::Texture(Renderer& renderer, TextureFilter filter, DXGI_FORMAT textureFormat)
	: samplerState(nullptr), texture(nullptr), textureUAV(nullptr), textureSRV(nullptr),
	textureFormat(textureFormat)
{
	this->createSamplerState(renderer.getDevice(), filter);
}

Texture::~Texture()
{
	S_RELEASE(this->samplerState);
	S_RELEASE(this->texture);
	S_RELEASE(this->textureUAV);
	S_RELEASE(this->textureSRV);
}

bool Texture::createAsRenderTexture(Renderer& renderer, 
	unsigned int width, unsigned int height)
{
	HRESULT result;

	// Deallocate old texture, if it exists
	S_RELEASE(this->texture);
	S_RELEASE(this->textureUAV);
	S_RELEASE(this->textureSRV);

	// Create texture
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.Format = this->textureFormat;

	result = renderer.getDevice()->CreateTexture2D(&textureDesc, NULL, &this->texture);
	if (FAILED(result))
	{
		Log::error("Failed creating texture in compute shader.");

		return false;
	}

	// Create UAV for texture
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = this->textureFormat;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	result = renderer.getDevice()->CreateUnorderedAccessView(this->texture, &uavDesc, &this->textureUAV);
	if (FAILED(result))
	{
		Log::error("Failed creating UAV for texture in compute shader.");

		return false;
	}

	return true;
}

bool Texture::createFromFile(Renderer& renderer, std::string path)
{
	ID3D11Device* device = renderer.getDevice();

	// Deallocate old texture, if it exists
	S_RELEASE(this->texture);
	S_RELEASE(this->textureUAV);
	S_RELEASE(this->textureSRV);


	HRESULT result;

	// Remake path as wstring
	std::wstring widePath;
	for (int i = 0; i < path.length(); ++i)
		widePath += wchar_t(path[i]);

	// Load DDS
	if (path.substr(path.size() - 3) == "dds")
	{
		/*result = DirectX::CreateDDSTextureFromFile(
			device, widePath.c_str(), &texResource, &this->texture
		);*/

		result = DirectX::CreateDDSTextureFromFileEx(
			device, widePath.c_str(), 0U, D3D11_USAGE_DEFAULT, 
			D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
			0, 0, false, (ID3D11Resource**) &this->texture, &this->textureSRV
		);
	}
	else // Load non-DDS
	{
		/*result = DirectX::CreateWICTextureFromFile(
			device, widePath.c_str(), &texResource, &this->texture
		);*/

		result = DirectX::CreateWICTextureFromFileEx(
			device, widePath.c_str(), 0U, D3D11_USAGE_DEFAULT,
			D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
			0, 0, false, (ID3D11Resource**) &this->texture, &this->textureSRV
		);
	}

	// Did the loading fail?
	if (FAILED(result))
	{
		Log::error("Could not create texture from file path: " + path);

		return false;
	}

	return true;
}

void Texture::set(Renderer& renderer, UINT startSlot)
{
	ID3D11DeviceContext* deviceContext = renderer.getDeviceContext();

	// Set sampler state in the pixel shader
	deviceContext->PSSetSamplers(startSlot, 1, &this->samplerState);

	// Set shader texture resource in the pixel shader
	deviceContext->PSSetShaderResources(startSlot, 1, &this->textureSRV);
}

bool Texture::recreateSRVAsRenderTexture(Renderer& renderer)
{
	S_RELEASE(this->textureSRV);

	// Create SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = this->textureFormat;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	HRESULT result = renderer.getDevice()->CreateShaderResourceView(this->texture, &srvDesc, &this->textureSRV);
	if (FAILED(result))
	{
		Log::error("Failed recreating SRV from texture.");

		return false;
	}

	return true;
}

ID3D11ShaderResourceView* Texture::getTextureSRV() const
{
	return this->textureSRV;
}

ID3D11UnorderedAccessView* Texture::getTextureUAV() const
{
	return this->textureUAV;
}

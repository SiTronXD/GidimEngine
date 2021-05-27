#include "Texture.h"
#include "../../External/DDSTextureLoader11.h"
#include "../../External/WICTextureLoader11.h"
#include "../Dev/Log.h"
#include "../Dev/Helpers.h"

bool Texture::createSampler()
{
	// Create texture sampler desc for the sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.AddressU = this->textureEdgeSamplingMode;
	samplerDesc.AddressV = this->textureEdgeSamplingMode;
	samplerDesc.AddressW = this->textureEdgeSamplingMode;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerDesc.Filter = this->textureFilter;

	// Create the texture sampler state
	HRESULT result = this->device->CreateSamplerState(&samplerDesc, &this->samplerState);
	if (FAILED(result))
	{
		Log::resultFailed("Could not create texture sampler.", result);

		return false;
	}

	return true;
}

Texture::Texture(Renderer& renderer, TextureFilter filter, 
	TextureFormat textureFormat, TextureEdgeSampling textureEdgeSampling)
	: device(renderer.getDevice()), deviceContext(renderer.getDeviceContext()),
	samplerState(nullptr), texture(nullptr), textureUAV(nullptr), textureSRV(nullptr),
	textureFilter((D3D11_FILTER) filter), textureFormat((DXGI_FORMAT) textureFormat),
	textureEdgeSamplingMode((D3D11_TEXTURE_ADDRESS_MODE) textureEdgeSampling)
{
	this->createSampler();
}

Texture::~Texture()
{
	S_RELEASE(this->samplerState);
	S_RELEASE(this->texture);
	S_RELEASE(this->textureUAV);
	S_RELEASE(this->textureSRV);
}

bool Texture::createFromFile(std::string path)
{
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
		result = DirectX::CreateDDSTextureFromFileEx(
			this->device, widePath.c_str(), 0U, D3D11_USAGE_DEFAULT,
			D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
			0, 0, false, (ID3D11Resource**)&this->texture, &this->textureSRV
		);
	}
	else // Load non-DDS
	{
		result = DirectX::CreateWICTextureFromFileEx(
			this->device, widePath.c_str(), 0U, D3D11_USAGE_DEFAULT,
			D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE,
			0, 0, false, (ID3D11Resource**)&this->texture, &this->textureSRV
		);
	}

	// Did the loading fail?
	if (FAILED(result))
	{
		Log::resultFailed("Could not create texture from file path: " + path, result);

		return false;
	}

	return true;
}

bool Texture::createAsRenderTexture(unsigned int width, unsigned int height)
{
	HRESULT result;

	// Deallocate old texture, if it exists
	S_RELEASE(this->texture);
	S_RELEASE(this->textureUAV);

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

	result = this->device->CreateTexture2D(&textureDesc, NULL, &this->texture);
	if (FAILED(result))
	{
		Log::resultFailed(
			"Failed creating texture as render texture.",
			result
		);

		return false;
	}

	// Create UAV for texture
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = this->textureFormat;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	result = this->device->CreateUnorderedAccessView(this->texture, &uavDesc, &this->textureUAV);
	if (FAILED(result))
	{
		Log::resultFailed("Failed creating UAV for texture.", result);

		return false;
	}

	this->createSRV();

	return true;
}

void Texture::setPS(UINT startSlot)
{
	// Set sampler state in the pixel shader
	this->deviceContext->PSSetSamplers(startSlot, 1, &this->samplerState);

	// Set shader texture resource in the pixel shader
	this->deviceContext->PSSetShaderResources(startSlot, 1, &this->textureSRV);
}

void Texture::setVS(UINT startSlot)
{
	// Set sampler state in the vertex shader
	this->deviceContext->VSSetSamplers(startSlot, 1, &this->samplerState);

	// Set shader texture resource in the vertex shader
	this->deviceContext->VSSetShaderResources(startSlot, 1, &this->textureSRV);
}

void Texture::clearRenderTexture(float red, float green, float blue, float alpha)
{
	float colors[4]{ red, green, blue, alpha };

	this->deviceContext->ClearUnorderedAccessViewFloat(
		this->textureUAV,
		colors
	);
}

bool Texture::createSRV()
{
	S_RELEASE(this->textureSRV);

	// Create SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = this->textureFormat;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	HRESULT result = this->device->CreateShaderResourceView(this->texture, &srvDesc, &this->textureSRV);
	if (FAILED(result))
	{
		if (!this->texture)
			Log::resultFailed("Failed creating SRV from texture, since a texture object has not been created...", result);
		else
			Log::resultFailed("Failed creating SRV from texture.", result);

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

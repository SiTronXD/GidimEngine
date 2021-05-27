#include "CubeMap.h"
#include "../Dev/Log.h"
#include "../Dev/Helpers.h"

bool CubeMap::createSampler()
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
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

	// Create the texture sampler state
	HRESULT result = this->device->CreateSamplerState(&samplerDesc, &this->samplerState);
	if (FAILED(result))
	{
		Log::resultFailed("Could not create texture sampler state.", result);

		return false;
	}

	return true;
}

CubeMap::CubeMap(Renderer& renderer)
	: samplerState(nullptr), texture(nullptr), 
	textureUAV(nullptr), textureSRV(nullptr)
{
	this->device = renderer.getDevice();
	this->deviceContext = renderer.getDeviceContext();

	int width = 256;
	int height = 256;

	// Create texture
	S_RELEASE(this->texture);
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 6;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HRESULT result = device->CreateTexture2D(&textureDesc, NULL, &this->texture);
	if (FAILED(result))
	{
		Log::resultFailed(
			"Failed creating cube map as render texture.",
			result
		);

		return;
	}

	// Create UAV for texture
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	uavDesc.Texture2DArray.ArraySize = 6;
	uavDesc.Texture2DArray.MipSlice = 0;
	uavDesc.Texture2DArray.FirstArraySlice = 0;

	result = device->CreateUnorderedAccessView(this->texture, &uavDesc, &this->textureUAV);
	if (FAILED(result))
	{
		Log::resultFailed("Failed creating UAV for cube map", result);
	}

	this->createSRVasRenderTexture();

	this->createSampler();
}

CubeMap::~CubeMap()
{
	S_RELEASE(this->samplerState);
	S_RELEASE(this->texture);
	S_RELEASE(this->textureUAV);
	S_RELEASE(this->textureSRV);
}

void CubeMap::setPS(UINT startSlot)
{
	// Set sampler state in the pixel shader
	this->deviceContext->PSSetSamplers(startSlot, 1, &this->samplerState);

	// Set shader texture resource in the pixel shader
	this->deviceContext->PSSetShaderResources(startSlot, 1, &this->textureSRV);
}

bool CubeMap::createSRVasRenderTexture()
{
	// Create SRV
	S_RELEASE(this->textureSRV);
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = 1;
	srvDesc.TextureCube.MostDetailedMip = 0;

	HRESULT result = this->device->CreateShaderResourceView(this->texture, &srvDesc, &this->textureSRV);
	if (FAILED(result))
	{
		Log::resultFailed("Failed recreating SRV from texture.", result);
		return false;
	}

	return true;
}

ID3D11UnorderedAccessView* CubeMap::getTextureUAV() const
{
	return this->textureUAV;
}

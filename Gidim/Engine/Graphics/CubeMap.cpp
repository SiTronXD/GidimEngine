#include "CubeMap.h"
#include "../Dev/Log.h"
#include "../Dev/Helpers.h"

CubeMap::CubeMap(Renderer& renderer, TextureFilter filter,
	TextureFormat textureFormat, TextureEdgeSampling textureEdgeSampling)
	: Texture(renderer, filter, textureFormat, textureEdgeSampling)
{
	
}

CubeMap::~CubeMap()
{

}

bool CubeMap::createAsRenderTexture(unsigned int faceWidth, unsigned int faceHeight)
{
	HRESULT result;

	// Deallocate old texture, if it exists
	S_RELEASE(this->texture);
	S_RELEASE(this->textureUAV);

	// Create texture
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = faceWidth;
	textureDesc.Height = faceHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 6;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.Format = this->textureFormat;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	result = device->CreateTexture2D(&textureDesc, NULL, &this->texture);
	if (FAILED(result))
	{
		Log::resultFailed(
			"Failed creating cube map as render texture.",
			result
		);

		return false;
	}

	// Create UAV for texture
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = this->textureFormat;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	uavDesc.Texture2DArray.ArraySize = 6;
	uavDesc.Texture2DArray.MipSlice = 0;
	uavDesc.Texture2DArray.FirstArraySlice = 0;

	result = device->CreateUnorderedAccessView(this->texture, &uavDesc, &this->textureUAV);
	if (FAILED(result))
	{
		Log::resultFailed("Failed creating UAV for cube map", result);

		return false;
	}

	// Create SRV for texture
	return this->createSRV();
}

bool CubeMap::createSRV()
{
	S_RELEASE(this->textureSRV);

	// Create SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = this->textureFormat;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = 1;
	srvDesc.TextureCube.MostDetailedMip = 0;

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
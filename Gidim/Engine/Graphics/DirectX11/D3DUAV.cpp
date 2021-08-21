#include "D3DUAV.h"

D3DUAV::D3DUAV(Renderer& renderer, std::string debugName)
	: bufferUAV(nullptr), renderer(renderer), debugName(debugName)
{
}

D3DUAV::~D3DUAV()
{
	S_RELEASE(this->bufferUAV);
}

void D3DUAV::createUAV(
	ID3D11Resource* buffer,
	DXGI_FORMAT format,
	D3D11_UAV_DIMENSION viewDimension,
	UINT numElements
)
{
	// Deallocate old UAV
	S_RELEASE(this->bufferUAV);

	// Init desc to zero
	D3D11_UNORDERED_ACCESS_VIEW_DESC descUAV;
	ZeroMemory(&descUAV, sizeof(descUAV));

	// Fill desc
	descUAV.Format = format;
	descUAV.ViewDimension = viewDimension;
	descUAV.Buffer.FirstElement = 0;
	descUAV.Buffer.NumElements = numElements;

	// Create buffer from desc
	HRESULT result = this->renderer.getDevice()->CreateUnorderedAccessView(buffer, &descUAV, &this->bufferUAV);
	if (FAILED(result))
	{
		Log::resultFailed("Failed creating UAV: " + this->debugName + ".", result);
	}
}
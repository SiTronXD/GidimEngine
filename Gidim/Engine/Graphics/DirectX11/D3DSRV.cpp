#include "D3DSRV.h"

D3DSRV::D3DSRV(Renderer& renderer, std::string debugName)
	: bufferSRV(nullptr), renderer(renderer), debugName(debugName)
{
}

D3DSRV::~D3DSRV()
{
	S_RELEASE(this->bufferSRV);
}

void D3DSRV::createSRV(
	D3DBuffer& buffer,
	DXGI_FORMAT format,
	D3D11_SRV_DIMENSION viewDimension
	)
{
	// Deallocate old SRV
	S_RELEASE(this->bufferSRV);

	// Init desc to zero
	D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
	ZeroMemory(&descSRV, sizeof(descSRV));

	// Fill desc
	descSRV.Format = format;
	descSRV.ViewDimension = viewDimension;
	descSRV.Buffer.FirstElement = 0;
	descSRV.Buffer.NumElements = buffer.getNumElements();

	// Create buffer from desc
	HRESULT result = this->renderer.getDevice()->CreateShaderResourceView(buffer.getBuffer(), &descSRV, &this->bufferSRV);
	if (FAILED(result))
	{
		Log::resultFailed("Failed creating SRV: " + this->debugName + ".", result);
	}
}

void D3DSRV::setVS(UINT startSlot)
{
	this->renderer.getDeviceContext()->VSSetShaderResources(
		startSlot, 
		1, 
		&this->bufferSRV
	);
}

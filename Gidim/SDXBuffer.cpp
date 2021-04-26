#include "SDXBuffer.h"
#include "SDXHelpers.h"
#include "Log.h"

SDXBuffer::SDXBuffer(ID3D11Device* device, UINT structSize)
	: buffer(nullptr)
{
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = structSize;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	HRESULT result = device->CreateBuffer(&bufferDesc, NULL, &this->buffer);
	if (FAILED(result))
	{
		Log::error("Failed creating matrix buffer.");
	}
}

SDXBuffer::~SDXBuffer()
{
	S_RELEASE(this->buffer);

	Log::print("DEALLOCATED BUFFER");
}

void SDXBuffer::map(ID3D11DeviceContext* deviceContext, 
	D3D11_MAPPED_SUBRESOURCE& inputMappedSubResource)
{
	HRESULT result = deviceContext->Map(
		this->buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &inputMappedSubResource
	);
	if (FAILED(result))
	{
		Log::error("Could not map matrix buffer.");
	}
}

void SDXBuffer::unmap(ID3D11DeviceContext* deviceContext)
{
	deviceContext->Unmap(this->buffer, NULL);
}

ID3D11Buffer* SDXBuffer::getBuffer() const
{
	return this->buffer;
}

#include "Renderer.h"
#include "Log.h"

bool Renderer::createDevice(Window& window)
{
	// Swap chain desc
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
	swapChainDesc.BufferCount = 1; // One back buffer, (and one front buffer)
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = window.getHandle();
	swapChainDesc.SampleDesc.Count = 1; // Number of samples for AA
	swapChainDesc.Windowed = true;

	// Create the swap chain, device and device context
	auto result = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
		nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc,
		&this->swapChain, &this->device, nullptr, &this->deviceContext
	);

	// Check for errors
	if (result != S_OK)
	{
		Log::Error("Swap chain creation failed.");
		return false;
	}

	return true;
}

bool Renderer::createRenderTarget()
{
	// Get back buffer from swap chain
	ID3D11Texture2D* backBuffer;
	this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**) &backBuffer);

	// Create render target view from back buffer
	this->device->CreateRenderTargetView(backBuffer, nullptr, &this->renderTargetView);

	// Get desc before releasing
	backBuffer->GetDesc(&backBufferDesc);
	backBuffer->Release();

	return true;
}

Renderer::Renderer(Window& window)
	: swapChain(nullptr), device(nullptr), deviceContext(nullptr),
	renderTargetView(nullptr)
{
	this->createDevice(window);
	this->createRenderTarget();
}

Renderer::~Renderer()
{
	// Set to windowed mode or the swap chain will throw an exception
	if (this->swapChain)
	{
		this->swapChain->SetFullscreenState(false, NULL);
	}


	if (this->renderTargetView)
	{
		this->renderTargetView->Release();
		this->renderTargetView = nullptr;
	}

	if (this->deviceContext)
	{
		this->deviceContext->Release();
		this->deviceContext = nullptr;
	}

	if (this->device)
	{
		this->device->Release();
		this->device = nullptr;
	}

	if (this->swapChain)
	{
		this->swapChain->Release();
		this->device = nullptr;
	}
}

void Renderer::beginFrame()
{
	// Bind render target
	this->deviceContext->OMSetRenderTargets(1, &this->renderTargetView, nullptr);

	// Set viewport
	auto viewport = CD3D11_VIEWPORT(
		0.0f, 0.0f, 
		(float) backBufferDesc.Width, (float) backBufferDesc.Height
	);
	this->deviceContext->RSSetViewports(1, &viewport);
}

void Renderer::endFrame()
{
	// Swap buffers
	this->swapChain->Present(1, 0);
}

void Renderer::clear(float clearColor[])
{
	// Set the background color
	this->deviceContext->ClearRenderTargetView(this->renderTargetView, clearColor);
}

ID3D11Device* Renderer::getDevice() const
{
	return this->device;
}

ID3D11DeviceContext* Renderer::getDeviceContext() const
{
	return this->deviceContext;
}

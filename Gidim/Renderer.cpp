#include "Renderer.h"
#include "Log.h"

bool Renderer::createDevice(Window& window, bool vsyncEnabled)
{
	// Swap chain desc
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
	swapChainDesc.BufferCount = 1; // One back buffer, (and one front buffer)
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = window.getHandle();
	swapChainDesc.SampleDesc.Count = 1; // Number of samples for AA
	swapChainDesc.Windowed = true;

	unsigned int refreshRateNumerator = 0;
	unsigned int refreshRateDenominator = 0;

	// Handle vsync
	if (vsyncEnabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = refreshRateNumerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = refreshRateDenominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set feature level of DirectX 11
	D3D_FEATURE_LEVEL featureLevel;
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swap chain, device and device context
	auto result = D3D11CreateDeviceAndSwapChain(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
		&featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc,
		&this->swapChain, &this->device, nullptr, &this->deviceContext
	);

	// Check for errors
	if (result != S_OK)
	{
		Log::error("Swap chain creation failed.");
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
	if (backBuffer != nullptr)
	{
		this->device->CreateRenderTargetView(
			backBuffer, nullptr, &this->renderTargetView
		);
	}
	else
		Log::error("Backbuffer was nullptr when trying to create render target view.");

	// Get desc before releasing
	backBuffer->GetDesc(&backBufferDesc);
	backBuffer->Release();

	return true;
}

Renderer::Renderer(Window& window)
	: swapChain(nullptr), device(nullptr), deviceContext(nullptr),
	renderTargetView(nullptr), camera(nullptr)
{
	this->projectionMatrix = XMMatrixIdentity();
	this->viewMatrix = XMMatrixIdentity();

	this->createDevice(window, false);
	this->createRenderTarget();
}

Renderer::~Renderer()
{
	// Set to windowed mode or the swap chain will throw an exception
	if (this->swapChain)
		this->swapChain->SetFullscreenState(false, NULL);


	this->renderTargetView->Release();
	this->deviceContext->Release();
	this->device->Release();
	this->swapChain->Release();
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

	// Update view matrix
	if (this->camera != nullptr)
		this->viewMatrix = this->camera->getViewMatrix();
}

void Renderer::endFrame()
{
	// Swap buffers
	this->swapChain->Present(0, 0);
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

void Renderer::setCamera(Camera& cam)
{
	this->camera = &cam;

	this->projectionMatrix = this->camera->getProjectionMatrix();
	this->viewMatrix = this->camera->getViewMatrix();
}

XMMATRIX Renderer::getProjectionMatrix()
{
	return this->projectionMatrix;
}

XMMATRIX Renderer::getViewMatrix()
{
	return this->viewMatrix;
}

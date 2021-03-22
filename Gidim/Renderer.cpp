#include "Renderer.h"
#include "Log.h"
#include <string>

bool Renderer::createDevice(Window& window, bool vsyncEnabled)
{
	HRESULT result;

	// Create a DirectX graphics interface factory
	IDXGIFactory* factory;
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**) &factory);
	if (FAILED(result))
	{
		Log::error("Could not create graphics interface factory.");

		return false;
	}

	// Use the factory to create an adapter for the primary
	// graphics interface (video card)
	IDXGIAdapter* adapter;
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		Log::error("Could not create adapter.");

		return false;
	}

	// Use the adapter to get the adapter output (monitor)
	IDXGIOutput* adapterOutput;
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		Log::error("Could not create adapter output.");

		return false;
	}

	// Get the number of modes that fit the 
	// DXGI_FORMAT_R8G8B8A8_UNORM display format for the
	// adapter output
	unsigned int numModes = 0;
	result = adapterOutput->GetDisplayModeList(
		DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
		&numModes, NULL
	);
	if (FAILED(result))
	{
		Log::error("Could not get display modes.");

		return false;
	}

	// Create an array to hold all possible display modes for
	// this monitor/video card combination
	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];

	// Fill display mode array
	result = adapterOutput->GetDisplayModeList(
		DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED,
		&numModes, displayModeList
	);

	// Save numerator and denominator after finding the correct display mode
	unsigned int refreshRateNumerator = 0;
	unsigned int refreshRateDenominator = 1;
	for (int i = 0; i < numModes; ++i)
	{
		if (displayModeList[i].Width == (unsigned int) window.getWidth() &&
			displayModeList[i].Height == (unsigned int) window.getHeight())
		{
			refreshRateNumerator = displayModeList[i].RefreshRate.Numerator;
			refreshRateDenominator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	// Deallocate dynamic array of display modes
	delete[] displayModeList;
	displayModeList = 0;

	// Swap chain desc
	DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
	swapChainDesc.BufferCount = 1; // One back buffer, (and one front buffer)
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.Width = window.getWidth();
	swapChainDesc.BufferDesc.Height = window.getHeight();
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = window.getHandle();
	swapChainDesc.SampleDesc.Count = 1; // Number of samples for AA
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = true;

	// Handle vsync
	if (vsyncEnabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = refreshRateNumerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = refreshRateDenominator;

		Log::print("refreshRateNumerator: " + std::to_string(refreshRateNumerator) + "  refreshRateDenominator: " + std::to_string(refreshRateDenominator));
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
	result = D3D11CreateDeviceAndSwapChain(
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

	this->createDevice(window, vsyncEnabled);
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
	this->swapChain->Present(vsyncEnabled, 0);
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

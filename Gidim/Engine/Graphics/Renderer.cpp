#include "Renderer.h"
#include "../Dev/Log.h"

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
	for (unsigned int i = 0; i < numModes; ++i)
	{
		if (displayModeList[i].Width == (UINT) window.getWidth() &&
			displayModeList[i].Height == (UINT) window.getHeight())
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
	{
		Log::error("Backbuffer was nullptr when trying to create render target view.");

		return false;
	}

	// Get desc before releasing
	backBuffer->GetDesc(&backBufferDesc);
	S_RELEASE(backBuffer);

	return true;
}

bool Renderer::createDepthStencilBuffers(Window& window)
{
	HRESULT result;

	// Create a description of the depth and stencil buffers
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc = { 0 };
	depthStencilBufferDesc.Width = window.getWidth();
	depthStencilBufferDesc.Height = window.getHeight();
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.SampleDesc.Quality = 0;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;

	// Create a texture for the depth and stencil buffers using the filled out
	// description
	result = device->CreateTexture2D(
		&depthStencilBufferDesc, NULL, &this->depthStencilBuffer
	);
	if (FAILED(result))
	{
		Log::error("Could not create depth/stencil texture.");

		return false;
	}

	// Create a description of the depth and stencil states
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = { 0 };

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; // Write on/off
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = 0xFF; // Portion for reading
	depthStencilDesc.StencilWriteMask = 0xFF; // Portion for writing

	// Stencil operations if pixel is front-facing
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


	// Create a depth stencil state
	result = device->CreateDepthStencilState(&depthStencilDesc, &this->depthStencilState);
	if (FAILED(result))
	{
		Log::error("Could not create depth stencil state.");

		return false;
	}

	// Set the depth stencil state
	deviceContext->OMSetDepthStencilState(this->depthStencilState, 1);

	// Set up the depth stencil views desc
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;


	// Create the depth stencil view
	result = device->CreateDepthStencilView(
		this->depthStencilBuffer, &depthStencilViewDesc, &this->depthStencilView
	);
	if (FAILED(result))
	{
		Log::error("Failed creating depth stencil view.");

		return false;
	}

	// Bind the render target view and depth stencil view to the 
	// output merger pipeline
	this->deviceContext->OMSetRenderTargets(
		1, &this->renderTargetView, depthStencilView
	);

	// Setup the raster description which will determine how and what
	// polygons will be drawn
	this->rasterDesc.AntialiasedLineEnable = false;
	this->rasterDesc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE;
	this->rasterDesc.DepthBias = 0;
	this->rasterDesc.DepthBiasClamp = 0.0f;
	this->rasterDesc.DepthClipEnable = true;
	this->rasterDesc.FillMode = D3D11_FILL_SOLID;
	this->rasterDesc.FrontCounterClockwise = false;
	this->rasterDesc.MultisampleEnable = false;
	this->rasterDesc.ScissorEnable = false;
	this->rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out
	result = device->CreateRasterizerState(&rasterDesc, &this->rasterState);
	if (FAILED(result))
	{
		Log::error("Could not create rasterizer state.");

		return false;
	}

	// Set rasteriser state
	deviceContext->RSSetState(this->rasterState);

	// Set viewport
	CD3D11_VIEWPORT viewport = CD3D11_VIEWPORT(
		0.0f, 0.0f,
		(float)backBufferDesc.Width, (float)backBufferDesc.Height
	);
	this->deviceContext->RSSetViewports(1, &viewport);

	return true;
}

Renderer::Renderer(Window& window)
	: swapChain(nullptr), device(nullptr), deviceContext(nullptr),
	renderTargetView(nullptr), camera(nullptr)
{
	// Initialize matrices
	XMStoreFloat4x4(&this->projectionMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&this->viewMatrix, XMMatrixIdentity());

	this->createDevice(window, vsyncEnabled);
	this->createRenderTarget();
	this->createDepthStencilBuffers(window);
}

Renderer::~Renderer()
{
	// Set to windowed mode or the swap chain will throw an exception
	if (this->swapChain)
		this->swapChain->SetFullscreenState(false, NULL);

	S_RELEASE(this->rasterState);
	S_RELEASE(this->depthStencilView);
	S_RELEASE(this->depthStencilState);
	S_RELEASE(this->depthStencilBuffer);
	S_RELEASE(this->renderTargetView);
	S_RELEASE(this->deviceContext);
	S_RELEASE(this->device);
	S_RELEASE(this->swapChain);
}

void Renderer::beginFrame()
{
	// Update view matrix
	if (this->camera != nullptr)
		XMStoreFloat4x4(&this->viewMatrix, this->camera->getViewMatrix());
}

void Renderer::endFrame()
{
	// Swap buffers
	this->swapChain->Present(vsyncEnabled, 0);
}

void Renderer::clear(XMFLOAT4 clearColor)
{
	// Set the background color
	float cc[4]{ clearColor.x, clearColor.y, clearColor.z, clearColor.w };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView, cc);

	// Clear the depth buffer
	this->deviceContext->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Renderer::setWireframe(bool wireFrame)
{
	// Destroy old raster state
	S_RELEASE(this->rasterState);

	// Set fill mode when toggling wireframe
	this->rasterDesc.FillMode = wireFrame ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;

	// Create the rasterizer state from the description we just filled out
	HRESULT result = device->CreateRasterizerState(&rasterDesc, &this->rasterState);
	if (FAILED(result))
	{
		Log::error("Could not create rasterizer state.");
	}

	// Set rasteriser state
	deviceContext->RSSetState(this->rasterState);
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

	XMStoreFloat4x4(&this->projectionMatrix, this->camera->getProjectionMatrix());
	XMStoreFloat4x4(&this->viewMatrix, this->camera->getViewMatrix());
}

XMMATRIX Renderer::getProjectionMatrix()
{
	XMMATRIX tempProjectionMatrix = XMLoadFloat4x4(&this->projectionMatrix);

	return tempProjectionMatrix;
}

XMMATRIX Renderer::getViewMatrix()
{
	XMMATRIX tempViewMatrix = XMLoadFloat4x4(&this->viewMatrix);

	return tempViewMatrix;
}

XMFLOAT3 Renderer::getCameraPosition() const
{
	return this->camera->getPosition();
}

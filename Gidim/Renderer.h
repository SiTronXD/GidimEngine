#pragma once

#include <d3d11.h>
#include "Window.h"

class Renderer
{
private:
	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	ID3D11RenderTargetView* renderTargetView;
	D3D11_TEXTURE2D_DESC backBufferDesc;

	bool createDevice(Window& window);
	bool createRenderTarget();

public:
	Renderer(Window& window);
	~Renderer();

	void beginFrame();
	void clear(float clearColor[]);
	void endFrame();

	ID3D11Device* getDevice() const;
	ID3D11DeviceContext* getDeviceContext() const;
};
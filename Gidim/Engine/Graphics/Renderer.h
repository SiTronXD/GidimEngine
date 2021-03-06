#pragma once

#include <d3d11.h>
#include "../Application/Window.h"
#include "../Dev/Helpers.h"
#include "Camera.h"

class Renderer
{
private:
	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	ID3D11RenderTargetView* renderTargetView;

	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11RasterizerState* rasterState;

	D3D11_TEXTURE2D_DESC backBufferDesc;
	D3D11_RASTERIZER_DESC rasterDesc;

	Camera* camera;
	XMFLOAT4X4 projectionMatrix;
	XMFLOAT4X4 viewMatrix;

	const bool vsyncEnabled = false;

	bool createDevice(Window& window, bool vsyncEnabled);
	bool createRenderTarget();
	bool createDepthStencilBuffers(Window& window);

public:
	Renderer(Window& window);
	~Renderer();

	void beginFrame();
	void clear(XMFLOAT4 clearColor);
	void endFrame();

	void setCamera(Camera& cam);

	void setWireframe(bool wireFrame);

	ID3D11Device* getDevice() const;
	ID3D11DeviceContext* getDeviceContext() const;

	XMMATRIX getProjectionMatrix();
	XMMATRIX getViewMatrix();

	XMFLOAT3 getCameraPosition() const;
};
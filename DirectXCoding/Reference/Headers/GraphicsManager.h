#pragma once

#include "Base.h"
#include "Viewport.h"

BEGIN(Engine)

class GraphicsManager : public Base
{
	DECLARE_SINGLETON(GraphicsManager)

private:
	GraphicsManager();
	virtual ~GraphicsManager() = default;

public:
	HRESULT	Initialize(GRAPHIC_DESC desc, _Inout_ ID3D11Device** device, _Inout_ ID3D11DeviceContext** deviceContext);
	HRESULT ClearBackBuffer(Color color);
	HRESULT ClearDepthStencil();
	HRESULT Present();

	void SetViewport(float width, float height, float x = 0, float y = 0, float minDepth = 0, float maxDepth = 1);
	Viewport& GetViewPort() { return _vp; }

private:
	ID3D11Device* _device = nullptr;
	ID3D11DeviceContext* _deviceContext = nullptr;
	IDXGISwapChain* _swapChain = nullptr;

	ID3D11RenderTargetView* _renderTargetView = nullptr;
	ID3D11DepthStencilView* _depthStencilView = nullptr;

private:
	HRESULT	CreateSwapChain(GRAPHIC_DESC desc);
	HRESULT CreateRenderTargetView();
	HRESULT CreateDepthStencilView(GRAPHIC_DESC desc);

public:
	virtual void Free() override;

	Viewport _vp;
};

END
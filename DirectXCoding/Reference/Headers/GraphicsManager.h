#pragma once

#include "Base.h"

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
};

END
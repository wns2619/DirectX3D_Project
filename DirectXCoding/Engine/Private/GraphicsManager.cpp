#include "pch.h"
#include "GraphicsManager.h"

IMPLEMENT_SINGLETON(GraphicsManager)

GraphicsManager::GraphicsManager()
	: _device(nullptr), _deviceContext(nullptr)
{
}

HRESULT GraphicsManager::Initialize(GRAPHIC_DESC desc, _Inout_ ID3D11Device** device, _Inout_ ID3D11DeviceContext** deviceContext)
{
	uint32 flag = 0;

#ifdef _DEBUG
	flag = D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL	featureLevel;
	ZeroMemory(&featureLevel, sizeof(featureLevel));

	if (FAILED(::D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, flag, nullptr, 0, D3D11_SDK_VERSION, &_device, &featureLevel, &_deviceContext)))
		return E_FAIL;

	if (FAILED(CreateSwapChain(desc)))
		return E_FAIL;

	if (FAILED(CreateRenderTargetView()))
		return E_FAIL;

	if (FAILED(CreateDepthStencilView(desc)))
		return E_FAIL;

	ID3D11RenderTargetView* rtv[1] = { _renderTargetView, };

	_deviceContext->OMSetRenderTargets(1, rtv, _depthStencilView);

	D3D11_VIEWPORT	viewportDesc;
	ZeroMemory(&viewportDesc, sizeof(viewportDesc));

	viewportDesc.TopLeftX = 0;
	viewportDesc.TopLeftY = 0;
	viewportDesc.Width = static_cast<_float>(desc.iWinSizeX);
	viewportDesc.Height = static_cast<_float>(desc.iWinSizeY);
	viewportDesc.MinDepth = 0.f;
	viewportDesc.MaxDepth = 1.f;

	_deviceContext->RSSetViewports(1, &viewportDesc);

	SetViewport(static_cast<_float>(desc.iWinSizeX), static_cast<_float>(desc.iWinSizeY));

	*device = _device;
	*deviceContext = _deviceContext;

	Safe_AddRef<ID3D11Device*>(_device);
	Safe_AddRef<ID3D11DeviceContext*>(_deviceContext);

	return S_OK;

}

HRESULT GraphicsManager::ClearBackBuffer(Color color)
{
	if (nullptr == _deviceContext)
		return E_FAIL;

	_deviceContext->ClearRenderTargetView(_renderTargetView, (_float*)&color);
	
	return S_OK;
}

HRESULT GraphicsManager::ClearDepthStencil()
{
	if (nullptr == _deviceContext)
		return E_FAIL;

	_deviceContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT GraphicsManager::Present()
{
	if (nullptr == _swapChain)
		return E_FAIL;

	return _swapChain->Present(0, 0);
}

void GraphicsManager::SetViewport(float width, float height, float x, float y, float minDepth, float maxDepth)
{
	_vp.Set(width, height, x, y, minDepth, maxDepth);
}

HRESULT GraphicsManager::CreateSwapChain(GRAPHIC_DESC desc)
{
	// 이미지 데이터를 생성하는 파생클래스
	IDXGIDevice*		device = nullptr;
	if (FAILED(_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&device)))
		return E_FAIL;

	// 디스플레이 하위 시스템(하나 이상 GPU, 비디오메모리포함)
	IDXGIAdapter* adapter = nullptr;
	if(FAILED(device->GetParent(__uuidof(IDXGIAdapter), (void**)&adapter)))
		return E_FAIL;

	// 전체 화면 전환을 처리하는 메서드드를 구현하기 위함.
	IDXGIFactory* factory = nullptr;
	if (FAILED(adapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory)))
		return E_FAIL;

	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(swapDesc));

	swapDesc.BufferDesc.Width = desc.iWinSizeX;
	swapDesc.BufferDesc.Height = desc.iWinSizeY;

	swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferCount = 1;

	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.SampleDesc.Count = 1;

	swapDesc.OutputWindow = desc._hwnd;
	swapDesc.Windowed = desc._winMode;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	if (FAILED(factory->CreateSwapChain(_device, &swapDesc, &_swapChain)))
		return E_FAIL;

	Safe_Release<IDXGIFactory*>(factory);
	Safe_Release<IDXGIAdapter*>(adapter);
	Safe_Release<IDXGIDevice*>(device);

	return S_OK;
}

HRESULT GraphicsManager::CreateRenderTargetView()
{
	if (nullptr == _device)
		return E_FAIL;

	ID3D11Texture2D* bufferTexture = nullptr;

	if (FAILED(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&bufferTexture)))
		return E_FAIL;

	if (FAILED(_device->CreateRenderTargetView(bufferTexture, nullptr, &_renderTargetView)))
		return E_FAIL;

	Safe_Release<ID3D11Texture2D*>(bufferTexture);

	return S_OK;
}

HRESULT GraphicsManager::CreateDepthStencilView(GRAPHIC_DESC desc)
{
	if (nullptr == _device)
		return E_FAIL;

	ID3D11Texture2D* DepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));

	texDesc.Width = desc.iWinSizeX;
	texDesc.Height = desc.iWinSizeY;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	texDesc.SampleDesc.Quality = 0;
	texDesc.SampleDesc.Count = 1;

	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	if (FAILED(_device->CreateTexture2D(&texDesc, nullptr, &DepthStencilTexture)))
		return E_FAIL;

	if (FAILED(_device->CreateDepthStencilView(DepthStencilTexture, nullptr, &_depthStencilView)))
		return E_FAIL;

	Safe_Release<ID3D11Texture2D*>(DepthStencilTexture);

	return S_OK;

}

void GraphicsManager::Free()
{
	Safe_Release<IDXGISwapChain*>(_swapChain);
	Safe_Release<ID3D11DepthStencilView*>(_depthStencilView);
	Safe_Release<ID3D11RenderTargetView*>(_renderTargetView);
	Safe_Release<ID3D11DeviceContext*>(_deviceContext);
//
//	#if defined(DEBUG) || defined(_DEBUG)
//	ID3D11Debug* d3dDebug;
//	HRESULT hr = _device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
//	if (SUCCEEDED(hr))
//	{
//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
//		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker \r ");
//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
//
//		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
//
//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
//		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker END \r ");
//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
//	}
//	if (d3dDebug != nullptr)            d3dDebug->Release();
//#endif


	Safe_Release<ID3D11Device*>(_device);
}


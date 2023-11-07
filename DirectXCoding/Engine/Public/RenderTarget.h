#pragma once

#include "Base.h"

BEGIN(Engine)

class RenderTarget final : public Base
{
private:
	RenderTarget(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~RenderTarget() = default;

public:
	ID3D11RenderTargetView* GetResourceView() { return _pRenderTargetView; }

public:
	HRESULT Initialize(uint32 iSizeX, uint32 iSizeY, DXGI_FORMAT ePixelFormat);
	HRESULT Clear();

#ifdef _DEBUG
public:
	HRESULT ReadyDebug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(class Shader* pShader, class VIBufferRect* pBuffer);
#endif // _DEBUG


private:
	ID3D11Device* _pDevice = nullptr;
	ID3D11DeviceContext* _pDeviceContext = nullptr;

private:
	ID3D11Texture2D* _pTexture2D = nullptr;
	ID3D11RenderTargetView* _pRenderTargetView = nullptr;
	ID3D11ShaderResourceView* _pShaderResourceView = nullptr;

private:
	Color _vColor;

#ifdef _DEBUG
private:
	Matrix _mWorldMatrix;
#endif // _DEBUG


public:
	static RenderTarget* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, uint32 iSizeX, uint32 iSizeY, DXGI_FORMAT ePixelFormat);
	virtual void Free() override;
};

END
#pragma once

#include "Base.h"

BEGIN(Engine)

class RenderTarget final :public Base
{
private:
	explicit RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~RenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() const { return _pRTV; }

public:
	HRESULT Initialize(uint32 iSizeX, uint32 iSizeY, DXGI_FORMAT ePixelFormat, const Color& vColor);
	HRESULT BindSRV(class Shader* pShader, const _char* pConstantName);
	HRESULT Clear();
	
#ifdef _DEBUG
public:
	HRESULT ReadyDebug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(class Shader* pShader, class VIBufferRect* pVIBuffer);
#endif // _DEBUG


private:
	ID3D11Device* _pDevice = nullptr;
	ID3D11DeviceContext* _pDeviceContext = nullptr;

private:
	ID3D11Texture2D* _pTexture2D = nullptr;
	ID3D11RenderTargetView* _pRTV = nullptr;
	ID3D11ShaderResourceView* _pSRV = nullptr;

private:
	Color _vColor;

#ifdef _DEBUG
private:
	Matrix _mWorldMatrix = Matrix::Identity;
#endif // _DEBUG


public:
	static RenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, uint32 iSizeX, uint32 iSizeY, DXGI_FORMAT ePiexlFormat, const Color& vColor);
	virtual void Free() override;
};


END


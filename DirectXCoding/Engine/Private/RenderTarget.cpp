#include "pch.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "VIBufferRect.h"

RenderTarget::RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: _pDevice(pDevice), _pDeviceContext(pDeviceContext)
{
	Safe_AddRef<ID3D11Device*>(_pDevice);
	Safe_AddRef<ID3D11DeviceContext*>(_pDeviceContext);
}

HRESULT RenderTarget::Initialize(uint32 iSizeX, uint32 iSizeY, DXGI_FORMAT ePixelFormat, const Color& vColor)
{
	_vColor = vColor;

	D3D11_TEXTURE2D_DESC		TextureDesc =
	{
		iSizeX,
		iSizeY,
		1,
		1,
		ePixelFormat,
		1,
		0,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
		0,
		0
	};

	//TextureDesc.Width = iSizeX;
	//TextureDesc.Height = iSizeY;
	//TextureDesc.MipLevels = 1;
	//TextureDesc.ArraySize = 1;
	//TextureDesc.Format = ePixelFormat;

	//TextureDesc.SampleDesc.Quality = 0;
	//TextureDesc.SampleDesc.Count = 1;

	//TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	//TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	//TextureDesc.CPUAccessFlags = 0;
	//TextureDesc.MiscFlags = 0;

	if (FAILED(_pDevice->CreateTexture2D(&TextureDesc, nullptr, &_pTexture2D)))
		return E_FAIL;

	D3D11_RENDER_TARGET_VIEW_DESC rtsvd = {
		ePixelFormat,
		D3D11_RTV_DIMENSION_TEXTURE2D
	};

	if (FAILED(_pDevice->CreateRenderTargetView(_pTexture2D, &rtsvd, &_pRTV)))
		return E_FAIL;



	if (FAILED(_pDevice->CreateShaderResourceView(_pTexture2D, nullptr, &_pSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT RenderTarget::BindSRV(Shader* pShader, const _char* pConstantName)
{
	return pShader->BindTexture(pConstantName, _pSRV);
}

HRESULT RenderTarget::Clear()
{
	_pDeviceContext->ClearRenderTargetView(_pRTV, reinterpret_cast<_float*>(&_vColor));

	return S_OK;
}

#ifdef _DEBUG

HRESULT RenderTarget::ReadyDebug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	D3D11_VIEWPORT ViewportDesc;

	uint32 iNumViewports = 1;

	_pDeviceContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	_mWorldMatrix = Matrix::Identity;

	_mWorldMatrix._11 = fSizeX;
	_mWorldMatrix._22 = fSizeY;

	_mWorldMatrix._41 = fX - ViewportDesc.Width * 0.5f;
	_mWorldMatrix._42 = -fY + ViewportDesc.Height * 0.5f;

	return S_OK;
}

HRESULT RenderTarget::Render(class Shader* pShader, VIBufferRect* pVIBuffer)
{
	if (FAILED(pShader->BindMatrix("W", &_mWorldMatrix)))
		return E_FAIL;

	if (FAILED(pShader->BindTexture("gTexture", _pSRV)))
		return E_FAIL;

	if (FAILED(pShader->Begin(0)))
		return E_FAIL;

	if (FAILED(pVIBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

#endif // _DEBUG




RenderTarget* RenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, uint32 iSizeX, uint32 iSizeY, DXGI_FORMAT ePiexlFormat, const Color& vColor)
{
	RenderTarget* pInstance = new RenderTarget(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize(iSizeX, iSizeY, ePiexlFormat, vColor)))
	{
		MSG_BOX("Failed to Created : CRenderTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void RenderTarget::Free()
{
	//SaveDDSTextureToFile(_pDeviceContext, _pTexture2D, TEXT("../Binaries/Diffuse.dds"));
	
	Safe_Release<ID3D11ShaderResourceView*>(_pSRV);
	Safe_Release<ID3D11RenderTargetView*>(_pRTV);
	Safe_Release<ID3D11Texture2D*>(_pTexture2D);

	Safe_Release<ID3D11Device*>(_pDevice);
	Safe_Release<ID3D11DeviceContext*>(_pDeviceContext);
}

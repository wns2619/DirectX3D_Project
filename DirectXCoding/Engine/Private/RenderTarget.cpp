#include "pch.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "VIBufferRect.h"

RenderTarget::RenderTarget(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
    : _pDevice(device), _pDeviceContext(deviceContext)
{
    Safe_AddRef<ID3D11Device*>(_pDevice);
    Safe_AddRef<ID3D11DeviceContext*>(_pDeviceContext);
}

HRESULT RenderTarget::Initialize(uint32 iSizeX, uint32 iSizeY, DXGI_FORMAT ePixelFormat)
{
	D3D11_TEXTURE2D_DESC		TextureDesc = {};

	TextureDesc.Width = iSizeX;
	TextureDesc.Height = iSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(_pDevice->CreateTexture2D(&TextureDesc, nullptr, &_pTexture2D)))
		return E_FAIL;

	if (FAILED(_pDevice->CreateRenderTargetView(_pTexture2D, nullptr, &_pRenderTargetView)))
		return E_FAIL;

	if (FAILED(_pDevice->CreateShaderResourceView(_pTexture2D, nullptr, &_pShaderResourceView)))
		return E_FAIL;


    return S_OK;
}

HRESULT RenderTarget::Clear()
{
	_pDeviceContext->ClearRenderTargetView(_pRenderTargetView, reinterpret_cast<_float*>(&_vColor));

	return S_OK;
}

#ifdef _DEBUG

HRESULT RenderTarget::ReadyDebug(_float fX, _float fY, _float fSizeX, _float fSizeY)
{
	D3D11_VIEWPORT viewPortDesc;

	uint32 iNumViewports = 1;

	_pDeviceContext->RSGetViewports(&iNumViewports, &viewPortDesc);

	_mWorldMatrix._11 = fSizeX;
	_mWorldMatrix._22 = fSizeY;

	_mWorldMatrix._41 = fX - viewPortDesc.Width * 0.5f;
	_mWorldMatrix._42 = -fY + viewPortDesc.Height * 0.5f;


	return S_OK;
}

HRESULT RenderTarget::Render(Shader* pShader, VIBufferRect* pBuffer)
{
	if (FAILED(pShader->BindMatrix("W", &_mWorldMatrix)))
		return E_FAIL;

	if (FAILED(pShader->BindTexture("gTexture", _pShaderResourceView)))
		return E_FAIL;

	if (FAILED(pShader->Begin(0)))
		return E_FAIL;

	if (FAILED(pBuffer->Render()))
		return E_FAIL;

	return S_OK;
}

#endif // _DEBUG






RenderTarget* RenderTarget::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, uint32 iSizeX, uint32 iSizeY, DXGI_FORMAT ePixelFormat)
{
	RenderTarget* pInstance = new RenderTarget(device, deviceContext);

	if (FAILED(pInstance->Initialize(iSizeX, iSizeY, ePixelFormat)))
	{
		MSG_BOX("Failed to Created : RenderTarget");
		Safe_Release<RenderTarget*>(pInstance);
	}

	return pInstance;

}

void RenderTarget::Free()
{
	//SaveDDSTextureToFile(_pDeviceContext, _pTexture2D, TEXT("..\\Binaries\\Diffuse.dds"));


	Safe_Release<ID3D11ShaderResourceView*>(_pShaderResourceView);
	Safe_Release<ID3D11RenderTargetView*>(_pRenderTargetView);
	Safe_Release<ID3D11Texture2D*>(_pTexture2D);

	Safe_Release<ID3D11Device*>(_pDevice);
	Safe_Release<ID3D11DeviceContext*>(_pDeviceContext);
}

#include "pch.h"
#include "RenderTargetManager.h"
#include "RenderTarget.h"

IMPLEMENT_SINGLETON(RenderTargetManager)

RenderTargetManager::RenderTargetManager()
{
	int a = 0;
}

HRESULT RenderTargetManager::AddRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const wstring& strTargetTag, uint32 iSizeX, uint32 iSizeY, DXGI_FORMAT ePixelFormat, const Color& vColor)
{
	if (nullptr != FindRenderTarget(strTargetTag))
		return E_FAIL;

	RenderTarget* pRenderTarget = RenderTarget::Create(pDevice, pDeviceContext, iSizeX, iSizeY, ePixelFormat, vColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	_renderTargets.emplace(strTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT RenderTargetManager::AddMRT(const wstring& strMRTTag, const wstring& strTargetTag)
{
	RenderTarget* pRenderTarget = FindRenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<RenderTarget*>* pMRTList = FindMRT(strMRTTag);

	if (nullptr == pMRTList)
	{
		list<RenderTarget*>		MRTList;
		MRTList.push_back(pRenderTarget);

		_MRTs.emplace(strMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);

	Safe_AddRef(pRenderTarget);

	return	S_OK;
}

HRESULT RenderTargetManager::BindSRV(Shader* pShader, const wstring& strTargetTag, const _char* pConstantName)
{
	RenderTarget* pRenderTarget = FindRenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->BindSRV(pShader, pConstantName);
}

HRESULT RenderTargetManager::BeginMRT(ID3D11DeviceContext* pDeviceContext, const wstring& strMRTTag)
{
	list<RenderTarget*>* pMRTList = FindMRT(strMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

 	pDeviceContext->OMGetRenderTargets(1, &_pBackBufferRTV, &_pDSV);

	ID3D11RenderTargetView* pRenderTargets[8] = {};

	uint32			iNumRTVs = 0;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTargets[iNumRTVs++] = pRenderTarget->Get_RTV();
		pRenderTarget->Clear();
	}

	if(iNumRTVs >= 0 || iNumRTVs < 8)
		pDeviceContext->OMSetRenderTargets(iNumRTVs, pRenderTargets, _pDSV);


	return	S_OK;
}

HRESULT RenderTargetManager::EndMRT(ID3D11DeviceContext* pDeviceContext)
{
	pDeviceContext->OMSetRenderTargets(1, &_pBackBufferRTV, _pDSV);

	Safe_Release<ID3D11RenderTargetView*>(_pBackBufferRTV);
	Safe_Release<ID3D11DepthStencilView*>(_pDSV);

	return S_OK;
}

#ifdef _DEBUG

HRESULT RenderTargetManager::ReadyDebug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
	RenderTarget* pRenderTarget = FindRenderTarget(strTargetTag);

	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->ReadyDebug(fX, fY, fSizeX, fSizeY);
}

HRESULT RenderTargetManager::Render(const wstring& strMRTTag, Shader* pShader, VIBufferRect* pVIBuffer)
{
	list<RenderTarget*>* pMRTList = FindMRT(strMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Render(pShader, pVIBuffer);
	}

	return S_OK;
}

#endif // _DEBUG




RenderTarget* RenderTargetManager::FindRenderTarget(const wstring& strTargetTag)
{
	auto	iter = _renderTargets.find(strTargetTag);

	if (iter == _renderTargets.end())
		return nullptr;

	return iter->second;
}

list<class RenderTarget*>* RenderTargetManager::FindMRT(const wstring& strMRTTag)
{
	auto	iter = _MRTs.find(strMRTTag);

	if (iter == _MRTs.end())
		return nullptr;

	return &iter->second;
}

void RenderTargetManager::Free()
{
	for (auto& Pair : _MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
			Safe_Release<RenderTarget*>(pRenderTarget);

		Pair.second.clear();
	}
	_MRTs.clear();

	for (auto& Pair : _renderTargets)
		Safe_Release<RenderTarget*>(Pair.second);

	_renderTargets.clear();
}

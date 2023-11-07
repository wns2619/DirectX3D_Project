#include "pch.h"
#include "TargetManager.h"
#include "RenderTarget.h"

IMPLEMENT_SINGLETON(TargetManager)

TargetManager::TargetManager()
{
}

HRESULT TargetManager::AddRenderTarget(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wstring& strTargetTag, uint32 iSizeX, uint32 iSizeY, DXGI_FORMAT ePixelFormat, const Color& vColor)
{
    if (nullptr != FindRenderTarget(strTargetTag))
        return E_FAIL;

    RenderTarget* pRenderTarget = RenderTarget::Create(device, deviceContext, iSizeX, iSizeY, ePixelFormat);

    if (nullptr == pRenderTarget)
        return E_FAIL;

    _renderTargets.emplace(strTargetTag, pRenderTarget);

    return S_OK;
}

HRESULT TargetManager::AddMRT(const wstring& strMRTTag, const wstring& strTargetTag)
{
    RenderTarget* pRenderTarget = FindRenderTarget(strTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    list<RenderTarget*>* pMRTList = FindMRT(strMRTTag);

    if (nullptr == pMRTList)
    {
        list<RenderTarget*> MRTList;
        MRTList.push_back(pRenderTarget);

        _MRTs.emplace(strMRTTag, MRTList);
    }
    else
        pMRTList->push_back(pRenderTarget);

    Safe_AddRef<RenderTarget*>(pRenderTarget);

    return S_OK;
}

HRESULT TargetManager::BeginMRT(ID3D11DeviceContext* deviceContext, const wstring& strMRTTag)
{
    list<RenderTarget*>* pMRTList = FindMRT(strMRTTag);

    if (nullptr == pMRTList)
        return E_FAIL;

    deviceContext->OMGetRenderTargets(1, &_pBackBufferRenderTarget, &_pDepthStencilView);

    ID3D11RenderTargetView* pRenderTargets[8] = {};

    uint32 iNumRTVs = 0;

    for (auto& pRenderTarget : *pMRTList)
    {
        pRenderTargets[iNumRTVs++] = pRenderTarget->GetResourceView();
        pRenderTarget->Clear();
    }

    if (iNumRTVs < 0 || iNumRTVs >= 8)
        return E_FAIL;

    deviceContext->OMSetRenderTargets(iNumRTVs, pRenderTargets, _pDepthStencilView);
    
    return S_OK;
}

HRESULT TargetManager::EndMRT(ID3D11DeviceContext* deviceContext)
{
    deviceContext->OMSetRenderTargets(1, &_pBackBufferRenderTarget, _pDepthStencilView);

    Safe_Release<ID3D11RenderTargetView*>(_pBackBufferRenderTarget);
    Safe_Release<ID3D11DepthStencilView*>(_pDepthStencilView);

    return S_OK;
}

#ifdef _DEBUG

HRESULT TargetManager::ReadyDebug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
    RenderTarget* pRenderTarget = FindRenderTarget(strTargetTag);

    if (nullptr == pRenderTarget)
        return E_FAIL;

    return pRenderTarget->ReadyDebug(fX, fY, fSizeX, fSizeY);
}

HRESULT TargetManager::Render(const wstring& strMRTTag, Shader* pShader, VIBufferRect* pVIBuffer)
{
    list<RenderTarget*>* pMRTList = FindMRT(strMRTTag);
    if (nullptr == pMRTList)
        return E_FAIL;

    for (auto& pRenderTarget : *pMRTList)
        pRenderTarget->Render(pShader, pVIBuffer);

    return S_OK;
}

#endif // _DEBUG




RenderTarget* TargetManager::FindRenderTarget(const wstring& strTargetTag)
{
    auto iter = _renderTargets.find(strTargetTag);

    if (iter == _renderTargets.end())
        return nullptr;

    return iter->second;
}

list<class RenderTarget*>* TargetManager::FindMRT(const wstring& strMRTTag)
{
    auto iter = _MRTs.find(strMRTTag);

    if (iter == _MRTs.end())
        return nullptr;

    return &iter->second;
}

void TargetManager::Free()
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

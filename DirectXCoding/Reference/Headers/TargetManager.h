#pragma once

#include "Base.h"

BEGIN(Engine)

class TargetManager final : public Base
{
	DECLARE_SINGLETON(TargetManager)

private:
	TargetManager();
	virtual ~TargetManager() = default;

public:
	HRESULT AddRenderTarget(ID3D11Device* device, ID3D11DeviceContext* deviceContext,
		const wstring& strTargetTag, uint32 iSizeX, uint32 iSizeY, DXGI_FORMAT ePixelFormat, const Color& vColor);
	HRESULT AddMRT(const wstring& strMRTTag, const wstring& strTargetTag);

	HRESULT BeginMRT(ID3D11DeviceContext* deviceContext, const wstring& strMRTTag);
	HRESULT EndMRT(ID3D11DeviceContext* deviceContext);

#ifdef _DEBUG
public:
	HRESULT ReadyDebug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(const wstring& strMRTTag, class Shader* pShader, class VIBufferRect* pVIBuffer);
#endif // _DEBUG


private:
	map<const wstring, class RenderTarget*> _renderTargets;
	map<const wstring, list<class RenderTarget*>> _MRTs;

private:
	ID3D11RenderTargetView* _pBackBufferRenderTarget = nullptr;
	ID3D11DepthStencilView* _pDepthStencilView = nullptr;

private:
	class RenderTarget* FindRenderTarget(const wstring& strTargetTag);
	list<class RenderTarget*>* FindMRT(const wstring& strMRTTag);

public:
	virtual void Free() override;
};

END


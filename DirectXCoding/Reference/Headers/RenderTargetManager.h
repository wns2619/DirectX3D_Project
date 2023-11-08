#pragma once

#include "Base.h"

BEGIN(Engine)

class RenderTargetManager final : public Base
{
	DECLARE_SINGLETON(RenderTargetManager)

private:
	RenderTargetManager();
	virtual ~RenderTargetManager() = default;

public:
	HRESULT AddRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const wstring& strTargetTag,
		uint32 iSizeX, uint32 iSizeY, DXGI_FORMAT ePixelFormat, const Color& vColor);
	HRESULT AddMRT(const wstring& strMRTTag, const wstring& strTargetTag);
	HRESULT BindSRV(class Shader* pShader, const wstring& strTargetTag, const _char* pConstantName);

	// strMRTTag에 해당하는 list에 담겨있는 타겟들을 장치에 바인딩.
	HRESULT BeginMRT(ID3D11DeviceContext* pDeviceContext, const wstring& strMRTTag);
	HRESULT EndMRT(ID3D11DeviceContext* pDeviceContext);

#ifdef _DEBUG
public:
	HRESULT ReadyDebug(const wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Render(const wstring& strMRTTag, class Shader* pShader, class VIBufferRect* pVIBuffer);
#endif // _DEBUG


private:
	map<const wstring, class RenderTarget*> _renderTargets;

	// 장치에 동시에 바인딩되어야 하는 타겟들을 미리 묶어 둔다.
	map<const wstring, list<class RenderTarget*>> _MRTs;
private:
	ID3D11RenderTargetView* _pBackBufferRTV = nullptr;
	ID3D11DepthStencilView* _pDSV = nullptr;

private:
	class RenderTarget* FindRenderTarget(const wstring& strTargetTag);
	list<class RenderTarget*>* FindMRT(const wstring& strMRTTag);

public:
	virtual void Free() override;

};

END
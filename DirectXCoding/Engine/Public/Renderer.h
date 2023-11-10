#pragma once

#include "Component.h"

BEGIN(Engine)

class GameObject;

class ENGINE_DLL Renderer final : public Component
{
public:
	enum class RENDERGROUP { PRIORITY = 0, NONLIGHT, NONBLEND, BLEND, UI, RENDER_END };
	
private:

	explicit Renderer(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Renderer(const Renderer& rhs) = delete;
	virtual ~Renderer() = default;

public:
	virtual HRESULT InitializePrototype()		override;
	virtual HRESULT Initialize(void* argument)	override;

public:
	HRESULT	AddRenderGroup(RENDERGROUP renderGroup, GameObject* gameObject);
	HRESULT	DrawRenderObjects();

#ifdef _DEBUG
	HRESULT AddDebug(class Component* pDebug)
	{
		_renderDebug.push_back(pDebug);
		Safe_AddRef<Component*>(pDebug);
		return S_OK;
	}
#endif // _DEBUG


private:
	list<GameObject*> _renderObjects[static_cast<int32>(RENDERGROUP::RENDER_END)];
	class RenderTargetManager* _pTargetManager = nullptr;
	class LightManager* _pLightManager = nullptr;

private:
	class VIBufferRect* _pVIBuffer = nullptr;
	class Shader* _pShader = nullptr;

	Matrix _mWorldMatrix, _mViewMatrix, _mProjMatrix;

#ifdef _DEBUG
private:
	list<class Component*> _renderDebug;
#endif // _DEBUG


private:
	HRESULT	RenderPriority();
	HRESULT	RenderNonLight();
	HRESULT	RenderNonBlend();
	HRESULT RenderLightAcc();
	HRESULT RenderDeferred();
	HRESULT	RenderBlend();
	HRESULT	RenderUI();

#ifdef _DEBUG
private:
	HRESULT RenderDebug();
#endif // _DEBUG

	
public:
	static Renderer* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual Component* Clone(void* argument)	override;
	virtual void Free()							override;
};


END


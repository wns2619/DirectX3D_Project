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

private:
	list<GameObject*> _renderObjects[static_cast<int32>(RENDERGROUP::RENDER_END)];
	class TargetManager* _pTargetManager = nullptr;

private:
	HRESULT	RenderPriority();
	HRESULT	RenderNonLight();
	HRESULT	RenderNonBlend();
	HRESULT	RenderBlend();
	HRESULT	RenderUI();
	
public:
	static Renderer* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual Component* Clone(void* argument)	override;
	virtual void Free()							override;
};


END


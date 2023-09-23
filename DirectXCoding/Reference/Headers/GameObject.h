#pragma once

#include "Base.h"
#include "Light.h"

BEGIN(Engine)

class Component;

class ENGINE_DLL GameObject abstract : public Base
{
protected:
	explicit GameObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit GameObject(const GameObject& rhs);
	virtual ~GameObject() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT	Initialize(void* argument);
	virtual void Tick(const _float& timeDelta);
	virtual void LateTick(const _float& timeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device* _device = nullptr;
	ID3D11DeviceContext* _deviceContext = nullptr;

protected:
	map<const wstring, Component*> _Components;

protected:
	HRESULT	AddComponent(uint32 levelIndex, const wstring& prototypeTag, const wstring& ComponentTag,
		_Inout_ Component** componentout, void* argument = nullptr);
	HRESULT AddLightComponent(uint32 levelIndex, const Light::LightType type, const wstring& ComponentTag,
		_Inout_ Component** componentout, void* argument = nullptr);
	Component* FindComponent(const wstring& componentTag);
public:
	virtual GameObject* Clone(void* argument) = 0;
	virtual void Free() override;
};

END
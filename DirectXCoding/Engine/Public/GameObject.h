#pragma once

#include "Base.h"
#include "Light.h"

BEGIN(Engine)

class Component;

enum class OBJECT_TYPE { PLAYER, TERRAIN, CAMERA, BACKGROUND, OBJECT_END };

class ENGINE_DLL GameObject abstract : public Base
{
protected:
	explicit GameObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext, OBJECT_TYPE type);
	explicit GameObject(const GameObject& rhs);
	virtual ~GameObject() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT	Initialize(void* argument);
	virtual void Tick(const _float& timeDelta);
	virtual void LateTick(const _float& timeDelta);
	virtual HRESULT Render();

public:
	OBJECT_TYPE GetObjectType() { return _objectType; }

protected:
	ID3D11Device* _device = nullptr;
	ID3D11DeviceContext* _deviceContext = nullptr;
	OBJECT_TYPE _objectType = OBJECT_TYPE::OBJECT_END;

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
#pragma once

#include "Base.h"
#include "Light.h"
#include "Collider.h"

BEGIN(Engine)

const int32 _id = 0;

class Component;
class Transform;
class Model;

enum class OBJECT_TYPE { PLAYER, TERRAIN, EMPTYOBJECT, CAMERA, BACKGROUND, OBJECT_END };

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
	void SetObjectType(OBJECT_TYPE type) { _objectType = type; }
	OBJECT_TYPE GetObjectType() { return _objectType; }
	Collider* GetCollider() { return _owner; }


	// IMGUI
	string GetModelName() const;
	string GetModelNameId() const;
	uint32 GetIdNumber() { return _id; }
	_bool* GetEnabled() { return &_enabled; }
	_bool* GetObjectSelect() { return &_selectObject; }
	Transform* GetTransform() { return _transform; }
	Model* GetModelComponent() { return _model; }
	const wstring GetModelPath() const { return _modelPath; }

	void SetEnabled(_bool enabled) { _enabled = enabled; }
	void SetIdNumber(uint32 id) { _id = id; }
protected:
	ID3D11Device* _device = nullptr;
	ID3D11DeviceContext* _deviceContext = nullptr;
	OBJECT_TYPE _objectType = OBJECT_TYPE::OBJECT_END;

	Collider* _owner = nullptr;
	Transform* _transform = nullptr;
	Model* _model = nullptr;

	// ID && IMGUI
	uint32 _id = 0;
	string _modelName = "";
	wstring _modelPath = L"";

	_bool  _enabled = false;
	_bool  _selectObject = false;

protected:
	map<const wstring, Component*> _Components;

protected:
	HRESULT	AddComponent(uint32 levelIndex, const wstring& prototypeTag, const wstring& ComponentTag,
		_Inout_ Component** componentout, void* argument = nullptr);
	//HRESULT AddLightComponent(uint32 levelIndex, const Light::LightType type, const wstring& ComponentTag,
	//	_Inout_ Component** componentout, void* argument = nullptr);
	Component* FindComponent(const wstring& componentTag);
public:
	virtual GameObject* Clone(void* argument) = 0;
	virtual void Free() override;
};

END
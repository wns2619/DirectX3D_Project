#pragma once

#include "GameObject.h"

BEGIN(Engine)
class Shader;
class Renderer;
class Collider;
END

BEGIN(Client)


// 해당 파일의 정보를 읽어들이자..
class DynamicObject abstract : public GameObject
{
public:
	enum DYNAMIC_TYPE { STEEL_DOOR, BREAK_DOOR, MASK, CHAIN, DRUM, 
		GRID_DOOR, CAGE, OLD_DOOR, VALVE, BASEMENT_WOOD, KEY, GUN, 
		GUN_LIGHT, BULLET, COLLIDER, EVENT_DOOR, WATER, TYPE_END };

public:
	struct STATE_DESC
	{
		wstring _strShaderName;
		wstring _strModelComponentName;
		wstring _protoTypeTag;
	};

protected:
	explicit DynamicObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext, DYNAMIC_TYPE eType);
	explicit DynamicObject(const DynamicObject& rhs);
	virtual ~DynamicObject() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& timeDelta)		override;
	virtual void LateTick(const _float& timeDelta)	override;
	virtual HRESULT Render()						override;

public:
	virtual HRESULT ReadyCollider() = 0;
	
public:
	STATE_DESC& GetDynamicComponentsName() { return _comNames; }

public:
	_bool CheckOBBOverlap(class Collider* pOther);
	void CheckAABBOverlap(class Collider* pOther);
	void CheckSphereOverlap(class Collider* pOther);

public:
	_bool GetIsOpen() { return _bIsOpen; }
	_bool GetIsRotatate() { return _bIsRotation; }

	void SetOpen(_bool open) { _bIsOpen = open; SetRotate(false); }
	void SetRotate(_bool rotate) { _bIsRotation = rotate; }

protected:
	Renderer* _render = nullptr;
	Shader* _shader = nullptr;

	STATE_DESC _comNames;
	DYNAMIC_TYPE _eDynamicType;


	Vec3 vOverlapMin;
	Vec3 vOverlapMax;

	_bool _bIsOpen = false;
	_bool _bIsRotation = false;
	_bool _bHalt = false;

protected:
	virtual HRESULT ReadyComponents();
	virtual HRESULT BindShaderResource();

public:
	virtual GameObject* Clone(void* argument) = 0;
	virtual void Free() override;
};


END
#pragma once

#include "LandObject.h"

BEGIN(Engine)
class Shader;
class Renderer;
class BinaryNavi;
END

BEGIN(Client)


// 해당 파일의 정보를 읽어들이자..
class StaticObject : public GameObject
{
public:
	struct STATE_DESC
	{
		wstring _strShaderName;
		wstring _strModelComponentName;
		wstring _protoTypeTag;
	};

protected:
	explicit StaticObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit StaticObject(const StaticObject& rhs);
	virtual ~StaticObject() = default;

public:
	virtual HRESULT InitializePrototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void PriorityTick(const _float& timeDelta)	override;
	virtual void Tick(const _float& timeDelta)			override;
	virtual void LateTick(const _float& timeDelta)		override;
	virtual HRESULT Render()							override;
	
public:
	STATE_DESC& GetStaticComponentsName() { return _comNames; }

protected:
	Renderer* _render = nullptr;
	Shader* _shader = nullptr;

protected:
	STATE_DESC _comNames;
	BinaryNavi* _pNavigation = nullptr;

private:
	VIBufferCell* _pVIBuffer = nullptr;

protected:
	HRESULT ReadyComponents();
	HRESULT BindShaderResource();

public:
	static StaticObject* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* argument) override;
	virtual void Free() override;
};


END
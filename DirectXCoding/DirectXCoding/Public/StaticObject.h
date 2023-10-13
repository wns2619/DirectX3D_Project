#pragma once

#include "GameObject.h"

BEGIN(Engine)
class Shader;
class Renderer;
END

BEGIN(Client)


// 해당 파일의 정보를 읽어들이자..
class StaticObject final : public GameObject
{
public:
	struct STATE_DESC
	{
		wstring _strShaderName;
		wstring _strModelComponentName;
		string _strModelName;
	};

private:
	explicit StaticObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit StaticObject(const StaticObject& rhs);
	virtual ~StaticObject() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& timeDelta)		override;
	virtual void LateTick(const _float& timeDelta)	override;
	virtual HRESULT Render()						override;
	
private:
	Renderer* _render = nullptr;
	Shader* _shader = nullptr;

private:
	HRESULT ReadyComponents();
	HRESULT BindShaderResource();

public:
	static StaticObject* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* argument) override;
	virtual void Free() override;
};


END
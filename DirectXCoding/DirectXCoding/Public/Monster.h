#pragma once

#include "LandObject.h"

BEGIN(Engine)
class Renderer;
class Transform;
class BinaryNavi;
class StateMachine;
class Animator;
class Collider;
END

BEGIN(Client)

class Monster final : public LandObject
{
public:
	struct STATE_DESC
	{
		wstring _strShaderName;
		wstring _strModelComponentName;
		wstring _protoTypeTag;
	};
private:
	explicit Monster(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Monster(const Monster& rhs);
	virtual ~Monster() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& fTimeDelta)		override;
	virtual void LateTick(const _float& fTimeDelta) override;
	virtual HRESULT Render()						override;

public:
	virtual void OnCollisionEnter(class  Collider* pOther);
	virtual void OnCollisionStay(class Collider* pOther);
	virtual void OnCollisionExit(class Collider* pOther);

public:
	StateMachine* GetStateMachine() { return _pStateMachine; }

private:
	void TrigerBoxEvent(Collider* pOther);

private:
	Renderer* _render = nullptr;
	Shader* _shader = nullptr;
	StateMachine* _pStateMachine = nullptr;
	Animator* _pAnimator = nullptr;

	uint32 animationcount = 0;

	STATE_DESC _comNames;

private:
	_bool _bDeadDelay = false;
	_float _fLifeTime = 0.f;

private:
	HRESULT ReadyComponents();
	HRESULT BindShaderResuorces();

public:
	static Monster* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* argument) override;
	virtual void Free() override;
};

END


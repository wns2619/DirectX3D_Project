#pragma once

#include "StaticObject.h"

BEGIN(Engine)
class VIBufferRect;
class Collider;
END

BEGIN(Client)

class TrigerBox final : public StaticObject
{
private:
	explicit TrigerBox(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit TrigerBox(const TrigerBox& rhs);
	virtual ~TrigerBox() = default;

public:
	virtual HRESULT InitializePrototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void PriorityTick(const _float& timeDelta)	override;
	virtual void Tick(const _float& timeDelta)			override;
	virtual void LateTick(const _float& timeDelta)		override;
	virtual HRESULT Render()							override;

public:
	virtual void OnCollisionEnter(class  Collider* pOther);
	virtual void OnCollisionStay(class Collider* pOther);
	virtual void OnCollisionExit(class Collider* pOther);

public:
	const _bool& GetTriger() { return _bTrigerOn; }

	void TrigerSet(_bool triger) { _bTrigerOn = triger; }

public:
	void EventTarget();
	void LerpSoundPlayer(_float& fVolume, _float& fDistance, _float fMaxDistance, GameInstance* pGameInstance, _float fMaxSound = 0.3f);

	void TrigerOccur(const _float& timeDelta);

private:
	VIBufferRect* _pVIBufferRect = nullptr;

	_bool _bTrigerOn = false;
	GameObject* _pTargetObject = nullptr;

	uint32 _iTurnCount = 0;

	_float _fEventSound = 0.f;

private: //event
	_bool _bEventState = false;

	Vec3 _fMaxBlinkRange = Vec3(2.f,0.f,0.f);
	Vec3 _fMinBlinkRange = Vec3(0.f,0.f,0.f);
	_float _fBlinkSpeed = 3.f;
	_float _fBlinkTime = 0.f;

	_float _PlayerLightOff = 0.f;

	_bool _bToMax = true;
	_bool _bToMin = false;

private:
	virtual HRESULT ReadyComponents() override;
	virtual HRESULT ReadyCollider() { return S_OK; };

public:
	static TrigerBox* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


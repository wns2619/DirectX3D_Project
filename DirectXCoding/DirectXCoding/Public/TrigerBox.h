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

private:
	VIBufferRect* _pVIBufferRect = nullptr;

	_bool _bTrigerOn = false;
	GameObject* _pTargetObject = nullptr;

	uint32 _iTurnCount = 0;

private:
	virtual HRESULT ReadyComponents() override;
	virtual HRESULT ReadyCollider() { return S_OK; };

public:
	static TrigerBox* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END


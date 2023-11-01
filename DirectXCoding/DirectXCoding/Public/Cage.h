#pragma once

#include "DynamicObject.h"

BEGIN(Client)

class Cage final : public DynamicObject
{
private:
	explicit Cage(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Cage(const Cage& rhs);
	virtual ~Cage() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& timeDelta)		override;
	virtual void LateTick(const _float& timeDelta)	override;
	virtual HRESULT Render()						override;

public:
	virtual HRESULT ReadyCollider()					override;

public:
	virtual void OnCollisionEnter(class  Collider* pOther);
	virtual void OnCollisionStay(class Collider* pOther);
	virtual void OnCollisionExit(class Collider* pOther);

private:
	void ValveTargetSet();

private:
	GameObject* _pTargetObject = nullptr;

public:
	static Cage* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* argument) override;
	virtual void Free() override;
};

END
#pragma once

#include "DynamicObject.h"

BEGIN(Client)

class Valve final : public DynamicObject
{
private:
	explicit Valve(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Valve(const Valve& rhs);
	virtual ~Valve() = default;

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

public:
	uint32 GetRotationCount() { return _iRotationCount; }

private:
	int32 _iRotationTick = 0;
	uint32 _iRotationCount = 0;

public:
	static Valve* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* argument) override;
	virtual void Free() override;
};

END
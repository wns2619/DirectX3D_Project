#pragma once

#include "DynamicObject.h"

BEGIN(Client)

class Water final : public DynamicObject
{
private:
	explicit Water(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Water(const Water& rhs);
	virtual ~Water() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& timeDelta)		override;
	virtual void LateTick(const _float& timeDelta)	override;
	virtual HRESULT Render()						override;

public:
	virtual void OnCollisionEnter(class  Collider* pOther);
	virtual void OnCollisionStay(class Collider* pOther);
	virtual void OnCollisionExit(class Collider* pOther);

public:
	virtual HRESULT ReadyCollider()					override;

public:
	static Water* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* argument) override;
	virtual void Free() override;
};

END
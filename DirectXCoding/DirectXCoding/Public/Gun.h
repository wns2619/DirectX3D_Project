#pragma once

#include "DynamicObject.h"


BEGIN(Client)

class Gun final : public DynamicObject
{
private:
	explicit Gun(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Gun(const Gun& rhs);
	virtual ~Gun() = default;

public:
	virtual HRESULT InitializePrototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void Tick(const _float& timeDelta)			override;
	virtual void LateTick(const _float& timeDelta)		override;
	virtual HRESULT Render()							override;

public:
	virtual HRESULT ReadyCollider()						override;

public:
	virtual void OnCollisionEnter(class  Collider* pOther);
	virtual void OnCollisionStay(class Collider* pOther);
	virtual void OnCollisionExit(class Collider* pOther);



public:
	static Gun* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END


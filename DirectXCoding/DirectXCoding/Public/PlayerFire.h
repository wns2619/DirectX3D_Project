#pragma once

#include "State.h"

BEGIN(Engine)

class GameObject;

END

BEGIN(Engine)

class PlayerFire final : public State
{
private:
	explicit PlayerFire(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~PlayerFire() = default;

public:
	virtual HRESULT ReadyState(class GameObject* pOwner)	override;
	virtual STATE UpdateState(const _float& timeDelta)		override;
	virtual void LateUpdateState(const _float& timeDelta)	override;
	virtual void RenderState()								override;

public:
	virtual STATE KeyInput(const _float& timeDelta)			override;

public:
	static PlayerFire* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, GameObject* pOwner);
	virtual void Free() override;
};

END



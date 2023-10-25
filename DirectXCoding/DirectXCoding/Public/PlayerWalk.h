#pragma once

#include "State.h"

BEGIN(Engine)

class GameObject;

END

BEGIN(Engine)

class PlayerWalk final : public State
{
private:
	explicit PlayerWalk(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~PlayerWalk() = default;
	
public:
	virtual HRESULT ReadyState(class GameObject* pOwner)	override;
	virtual STATE UpdateState(const _float& timeDelta)		override;
	virtual void LateUpdateState(const _float& timeDelta)	override;
	virtual void RenderState()								override;

public:
	virtual STATE KeyInput(const _float& timeDelta)			override;

public:
	static PlayerWalk* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, GameObject* pOwner);
	virtual void Free() override;
};

END



#pragma once

#include "State.h"

BEGIN(Engine)

class GameObject;

END

BEGIN(Client)

class PlayerReload final : public State
{
private:
	explicit PlayerReload(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~PlayerReload() = default;

public:
	virtual HRESULT ReadyState(class GameObject* pOwner)	override;
	virtual STATE UpdateState(const _float& timeDelta)		override;
	virtual void LateUpdateState(const _float& timeDelta)	override;
	virtual void RenderState()								override;

public:
	virtual void ChangeSetState() override;

public:
	virtual STATE KeyInput(const _float& timeDelta)			override;

private:
	_bool _bWalk = false;
	_bool _bReload = false;

public:
	static PlayerReload* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, GameObject* pOwner);
	virtual void Free() override;
};

END



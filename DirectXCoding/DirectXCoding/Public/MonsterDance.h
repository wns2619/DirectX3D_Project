#pragma once

#include "State.h"

BEGIN(Engine)
class GameObject;

END

BEGIN(Client)

class MonsterDance final : public State
{
private:
	explicit MonsterDance(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~MonsterDance() = default;

public:
	virtual HRESULT ReadyState(class GameObject* pOwner)	override;
	virtual STATE UpdateState(const _float& timeDelta)		override;
	virtual void LateUpdateState(const _float& timeDelta)	override;
	virtual void RenderState()								override;

public:
	virtual State::STATE KeyInput(const _float& timeDelta) override
	{
		return State::STATE::WALK;
	}

public:
	virtual void ChangeSetState() override;

public:
	static MonsterDance* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, GameObject* pOwner);
	virtual void Free() override;


};

END



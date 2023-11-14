#pragma once

#include "State.h"

BEGIN(Engine)
class GameObject;
class GameInstance;
END

BEGIN(Client)

class MonsterWalk final : public State
{
private:
	explicit MonsterWalk(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~MonsterWalk() = default;

public:
	virtual HRESULT ReadyState(class GameObject* pOwner)	override;
	virtual STATE UpdateState(const _float& timeDelta)		override;
	virtual void LateUpdateState(const _float& timeDelta)	override;
	virtual void RenderState()								override;

public:
	virtual State::STATE KeyInput(const _float& timeDelta) override
	{ return State::STATE::WALK; }

public:
	virtual void ChangeSetState() override;
	void LerpSoundPlayer(_float& fVolume, _float& fDistance, _float fMaxDistance, GameInstance* pGameInstance);


public:
	static MonsterWalk* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, GameObject* pOwner);
	virtual void Free() override;


};

END



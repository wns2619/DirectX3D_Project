#pragma once

#include "State.h"

BEGIN(Engine)
class GameObject;
class GameInstance;
END

BEGIN(Client)

class MonsterSeat final : public State
{
private:
	explicit MonsterSeat(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~MonsterSeat() = default;

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

private:
	Vec4 _vOriginPos;

public:
	virtual void ChangeSetState() override;
	void LerpSoundPlayer(_float& fVolume, _float& fDistance, _float fMaxDistance, GameInstance* pGameInstance);
	void LerpSoundTime(_float& fVolume, _float& time, _float fMaxTime, GameInstance* pGameInstance);
public:
	static MonsterSeat* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, GameObject* pOwner);
	virtual void Free() override;


};

END



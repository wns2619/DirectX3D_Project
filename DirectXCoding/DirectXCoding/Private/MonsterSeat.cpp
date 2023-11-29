#include "pch.h"
#include "MonsterSeat.h"
#include "GameInstance.h"
#include "Monster.h"
#include "DynamicObject.h"
#include "Player.h"
#include "Cell.h"
#include "MonsterLight.h"
#include "PlayerBody.h"

MonsterSeat::MonsterSeat(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
}

HRESULT MonsterSeat::ReadyState(GameObject* pOwner)
{
	_pOwner = pOwner;

	return S_OK;
}

State::STATE MonsterSeat::UpdateState(const _float& timeDelta)
{
	State::STATE eState = STATE::SEAT;

	Vec4 vPos = _vOriginPos;
	vPos.y = _vOriginPos.y - 1.3f;
	
	_pOwner->GetTransform()->SetState(Transform::STATE::POSITION, vPos);

	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	Player* pPlayer = static_cast<Player*>(pGameInstance->GetLayerObject(LAYER_TAG::LAYER_PLAYER).front());

	if (nullptr != pPlayer)
	{
		const _bool& bLight = static_cast<PlayerBody*>(pPlayer->GetPlyaerPart()[Player::PART::PART_BODY])->IsObtainingLight();

		if (true == bLight)
			pGameInstance->DeleteObject(_pOwner);
	}

	RELEASE_INSTANCE(GameInstance);

	return eState;
}

void MonsterSeat::LateUpdateState(const _float& timeDelta)
{
}

void MonsterSeat::RenderState()
{
}

void MonsterSeat::ChangeSetState()
{
	_vOriginPos = _pOwner->GetTransform()->GetState(Transform::STATE::POSITION);
	_pOwner->GetTransform()->FixRotation(Vec4(0.f, 1.f, 0.f, 1.f), ::XMConvertToRadians(180.f));
	_pOwner->GetTransform()->BillBoardRotate(Vec4(1.f, 0.f, 0.f, 1.f), ::XMConvertToRadians(11.f));
}

void MonsterSeat::LerpSoundPlayer(_float& fVolume, _float& fDistance, _float fMaxDistance, GameInstance* pGameInstance)
{
	const _float fMaxVolume = 0.3f;
	const _float fMinVolume = 0.f;

	Vec4 vThisPos = _pOwner->GetTransform()->GetState(Transform::STATE::POSITION);

	uint32 iCurrentLevel = pGameInstance->GetCurrentLevelIndex();

	Transform* pPlayerTransform = static_cast<Transform*>(pGameInstance->GetComponent(iCurrentLevel, LAYER_TAG::LAYER_PLAYER, TEXT("ComponentTransform"), "Player"));

	Vec4 vPlayerPos = pPlayerTransform->GetState(Transform::STATE::POSITION);

	Vec4 vDir = vPlayerPos - vThisPos;
	fDistance = vDir.Length();

	fVolume = fMaxVolume - (fDistance / fMaxDistance) * (fMaxVolume - fMinVolume);

	if (fVolume <= 0.f)
		fVolume = 0.f;
}

void MonsterSeat::LerpSoundTime(_float& fVolume, _float& time, _float fMaxTime, GameInstance* pGameInstance)
{
	const _float fMaxVolume = 0.3f;
	const _float fMinVolume = 0.f;

	fVolume = fMaxVolume - (time / fMaxTime) * (fMaxVolume - fMinVolume);

	if (fVolume <= 0.f)
		fVolume = 0.f;
}


MonsterSeat* MonsterSeat::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, GameObject* pOwner)
{
	MonsterSeat* pInstance = new MonsterSeat(device, deviceContext);

	if (FAILED(pInstance->ReadyState(pOwner)))
	{
		MSG_BOX("Failed to Created : MonsterSeat");
		Safe_Release<MonsterSeat*>(pInstance);
	}

	return pInstance;

}

void MonsterSeat::Free()
{
	__super::Free();
}

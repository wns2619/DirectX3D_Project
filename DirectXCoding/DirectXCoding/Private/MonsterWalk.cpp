#include "pch.h"
#include "MonsterWalk.h"
#include "GameInstance.h"

MonsterWalk::MonsterWalk(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
}

HRESULT MonsterWalk::ReadyState(GameObject* pOwner)
{
	_pOwner = pOwner;

	return S_OK;
}

State::STATE MonsterWalk::UpdateState(const _float& timeDelta)
{
	State::STATE eState = STATE::IDLE;



	return eState;
}

void MonsterWalk::LateUpdateState(const _float& timeDelta)
{
}

void MonsterWalk::RenderState()
{
}

void MonsterWalk::ChangeSetState()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	RELEASE_INSTANCE(GameInstance);
}

void MonsterWalk::LerpSoundPlayer(_float& fVolume, _float& fDistance, _float fMaxDistance, GameInstance* pGameInstance)
{
	const _float fMaxVolume = 0.3f;
	const _float fMinVolume = 0.f;

	Vec4 vThisPos = _pOwner->GetTransform()->GetState(Transform::STATE::POSITION);

	uint32 iCurrentLevel = pGameInstance->GetCurrentLevelIndex();

	Transform* pPlayerTransform = dynamic_cast<Transform*>(pGameInstance->GetComponent(iCurrentLevel, LAYER_TAG::LAYER_PLAYER, TEXT("ComponentTransform"), "Player"));

	Vec4 vPlayerPos = pPlayerTransform->GetState(Transform::STATE::POSITION);

	Vec4 vDir = vPlayerPos - vThisPos;
	fDistance = vDir.Length();

	fVolume = fMaxVolume - (fDistance / fMaxDistance) * (fMaxVolume - fMinVolume);

	if (fVolume <= 0.f)
		fVolume = 0.f;
}

MonsterWalk* MonsterWalk::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, GameObject* pOwner)
{
	MonsterWalk* pInstance = new MonsterWalk(device, deviceContext);

	if (FAILED(pInstance->ReadyState(pOwner)))
	{
		MSG_BOX("Failed to Created : MonsterWalk");
		Safe_Release<MonsterWalk*>(pInstance);
	}

	return pInstance;

}

void MonsterWalk::Free()
{
	__super::Free();
}

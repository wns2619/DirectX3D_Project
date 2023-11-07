#include "pch.h"
#include "MonsterRun.h"
#include "GameInstance.h"

MonsterRun::MonsterRun(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
}

HRESULT MonsterRun::ReadyState(GameObject* pOwner)
{
	_pOwner = pOwner;

	return S_OK;
}

State::STATE MonsterRun::UpdateState(const _float& timeDelta)
{
	State::STATE eState = STATE::RUN;

	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	_float fVolume = 0.f;
	_float fDistance = 5.f;

	LerpSoundPlayer(fVolume, fDistance, 7.f, pGameInstance);

	pGameInstance->PlaySound(TEXT("Steps.wav"), SOUND_MONSTER, fVolume);

	RELEASE_INSTANCE(GameInstance);


	return eState;
}

void MonsterRun::LateUpdateState(const _float& timeDelta)
{
}

void MonsterRun::RenderState()
{
}

void MonsterRun::ChangeSetState()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	pGameInstance->StopSound(SOUND_MONSTER);

	RELEASE_INSTANCE(GameInstance);
}

void MonsterRun::LerpSoundPlayer(_float& fVolume, _float& fDistance, _float fMaxDistance, GameInstance* pGameInstance)
{
	const _float fMaxVolume = 1.f;
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

MonsterRun* MonsterRun::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, GameObject* pOwner)
{
	MonsterRun* pInstance = new MonsterRun(device, deviceContext);

	if (FAILED(pInstance->ReadyState(pOwner)))
	{
		MSG_BOX("Failed to Created : MonsterRun");
		Safe_Release<MonsterRun*>(pInstance);
	}

	return pInstance;

}

void MonsterRun::Free()
{
	__super::Free();
}

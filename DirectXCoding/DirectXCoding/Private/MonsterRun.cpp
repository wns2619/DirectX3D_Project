#include "pch.h"
#include "MonsterRun.h"
#include "GameInstance.h"
#include "Monster.h"
#include "Player.h"
#include "MonsterLight.h"
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

	Monster* pOwnerMonster = static_cast<Monster*>(_pOwner);

	if (0 < pOwnerMonster->GetLife() && false == pOwnerMonster->GetDeadDelay())
	{
		LerpSoundPlayer(fVolume, fDistance, 13.f, pGameInstance);

		if (false == pOwnerMonster->GetOnWater() && false == pOwnerMonster->GetRunOnWater() && pOwnerMonster->GetMonsterID() == 1)
			pGameInstance->PlaySound(TEXT("Steps.wav"), SOUND_MONSTER, fVolume);
		else if (true == pOwnerMonster->GetOnWater() && false == pOwnerMonster->GetRunOnWater())
			pGameInstance->PlaySound(TEXT("walkWATER.wav"), SOUND_MONSTER, fVolume);
		else if (true == pOwnerMonster->GetRunOnWater())
			pGameInstance->PlaySound(TEXT("RUNWATER.wav"), SOUND_MONSTER3, fVolume);

		if (nullptr != pOwnerMonster->GetTargetObject())
		{
			// TODO -> 열쇠면 열쇠 쪽으로 순찰
			// 플레이어를 발견했으면 플레이어 추격.
			pOwnerMonster->MoveAstar(timeDelta);
			pOwnerMonster->GetTransform()->Forward(timeDelta, pOwnerMonster->GetNavigation());

			if (pOwnerMonster->GetTargetObject()->GetObjectType() == OBJECT_TYPE::DYNAMIC)
				pOwnerMonster->GetNavigation()->StartAStar(679);
			else
				pOwnerMonster->GetNavigation()->StartAStar(static_cast<Player*>(pOwnerMonster->GetTargetObject())->GetNavigation()->GetCurrentIndex());

			list<Cell*>& bestCell = pOwnerMonster->GetBestList();
			bestCell = pOwnerMonster->GetNavigation()->GetBestCell();

			Vec4& vDestination = pOwnerMonster->GetDestination();
			vDestination = pOwnerMonster->GetTargetObject()->GetTransform()->GetState(Transform::STATE::POSITION);
			// 도착거리.
		}
	}
	else
	{
		if (true == pOwnerMonster->GetDeadDelay())
		{
			if (1 == pOwnerMonster->GetMonsterID())
			{
				_float& SurpriseTime = pOwnerMonster->GetSurpriseTime();

				SurpriseTime += timeDelta;

				if (SurpriseTime >= 0.5f)
				{
					pGameInstance->DeleteObject(pOwnerMonster);
					pGameInstance->StopSound(SOUND_MONSTER);
				}
			}
		}

		if (0 >= pOwnerMonster->GetLife())
		{
			_float& LifeTime = pOwnerMonster->GetLifeTime();

			LifeTime += timeDelta * 3.f;

			LerpSoundTime(fVolume, LifeTime, 8.f);
			if (LifeTime >= 8.f)
			{
				pGameInstance->DeleteObject(pOwnerMonster);

				if (2 == pOwnerMonster->GetMonsterID())
				{
					pGameInstance->StopSound(SOUND_MONSTER);
					pGameInstance->StopSound(SOUND_MONSTER3);
				}

				if (2 == pOwnerMonster->GetMonsterID() || 3 == pOwnerMonster->GetMonsterID())
					static_cast<MonsterLight*>(pOwnerMonster->GetMonsterPart()[Monster::MONSTER_PART::PART_LIGHT])->GetOwnLight()->GetLightDesc()->bEnable = true;
			}
			else
			{
				if (2 == pOwnerMonster->GetMonsterID())
				{
					pGameInstance->PlaySound(TEXT("walkWATER.wav"), SOUND_MONSTER, fVolume);
					pGameInstance->PlaySound(TEXT("RUNWATER.wav"), SOUND_MONSTER3, fVolume);
				}
			}
		}
	}

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

	Transform* pPlayerTransform = static_cast<Transform*>(pGameInstance->GetComponent(iCurrentLevel, LAYER_TAG::LAYER_PLAYER, TEXT("ComponentTransform"), "Player"));

	Vec4 vPlayerPos = pPlayerTransform->GetState(Transform::STATE::POSITION);

	Vec4 vDir = vPlayerPos - vThisPos;
	fDistance = vDir.Length();

	fVolume = fMaxVolume - (fDistance / fMaxDistance) * (fMaxVolume - fMinVolume);

	if (fVolume <= 0.f)
		fVolume = 0.f;
}

void MonsterRun::LerpSoundTime(_float& fVolume, _float& time, _float fMaxTime)
{
	const _float fMaxVolume = 0.3f;
	const _float fMinVolume = 0.f;

	fVolume = fMaxVolume - (time / fMaxTime) * (fMaxVolume - fMinVolume);

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

#include "pch.h"
#include "MonsterWalk.h"
#include "GameInstance.h"
#include "Monster.h"
#include "DynamicObject.h"
#include "Player.h"
#include "Cell.h"
#include "MonsterLight.h"

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

	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	wstring soundfileName = L"";


	_float fVolume = 0.f;
	_float fDistance = 5.f;

	Monster* pOwnerMonster = static_cast<Monster*>(_pOwner);

	if (true == static_cast<Monster*>(_pOwner)->GetOnWater() && false == static_cast<Monster*>(_pOwner)->IsDead())
		soundfileName = TEXT("walkWATER.wav");
	else if (pOwnerMonster->GetMonsterID() == 5 || pOwnerMonster->GetMonsterID() == 6)
		soundfileName = TEXT("MonsterCrying.wav");

	if (0 < pOwnerMonster->GetLife() && false == pOwnerMonster->GetDeadDelay())
	{
		LerpSoundPlayer(fVolume, fDistance, 13.f, pGameInstance);

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

		}

		if (pOwnerMonster->GetMonsterID() == 3)
			pGameInstance->PlaySound(soundfileName.c_str(), SOUND_MONSTER, fVolume);
		else if (pOwnerMonster->GetMonsterID() == 5)
			pGameInstance->PlaySound(soundfileName.c_str(), SOUND_MONSTER2, fVolume);
		else if (pOwnerMonster->GetMonsterID() == 6)
			pGameInstance->PlaySound(soundfileName.c_str(), SOUND_MONSTER3, fVolume);
	}
	else
	{
		if (true == pOwnerMonster->GetDeadDelay())
		{
			if (2 == pOwnerMonster->GetMonsterID())
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

			LerpSoundTime(fVolume, LifeTime, 8.f, pGameInstance);
			if (LifeTime >= 8.f)
			{
				pGameInstance->DeleteObject(pOwnerMonster);

				if (3 == pOwnerMonster->GetMonsterID())
				{
					pGameInstance->StopSound(SOUND_MONSTER);
					pGameInstance->StopSound(SOUND_MONSTER3);
				}
				else if (5 == pOwnerMonster->GetMonsterID())
					pGameInstance->StopSound(SOUND_MONSTER2);
				else if (6 == pOwnerMonster->GetMonsterID())
					pGameInstance->StopSound(SOUND_MONSTER3);

				if (3 == pOwnerMonster->GetMonsterID() || 4 == pOwnerMonster->GetMonsterID())
					static_cast<MonsterLight*>(pOwnerMonster->GetMonsterPart()[Monster::MONSTER_PART::PART_LIGHT])->GetOwnLight()->GetLightDesc()->bEnable = true;
			}
			else
			{
				if (3 == pOwnerMonster->GetMonsterID())
				{
					pGameInstance->PlaySound(TEXT("walkWATER.wav"), SOUND_MONSTER, fVolume);
					pGameInstance->PlaySound(TEXT("RUNWATER.wav"), SOUND_MONSTER3, fVolume);
				}
				else if (5 == pOwnerMonster->GetMonsterID())
					pGameInstance->PlaySound(soundfileName.c_str(), SOUND_MONSTER2, fVolume);
				else if(6 == pOwnerMonster->GetMonsterID())
					pGameInstance->PlaySound(soundfileName.c_str(), SOUND_MONSTER3, fVolume);
			}
		}
	}
	
	RELEASE_INSTANCE(GameInstance);


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

	pGameInstance->StopSound(SOUND_MONSTER);

	RELEASE_INSTANCE(GameInstance);
}

void MonsterWalk::LerpSoundPlayer(_float& fVolume, _float& fDistance, _float fMaxDistance, GameInstance* pGameInstance)
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

void MonsterWalk::LerpSoundTime(_float& fVolume, _float& time, _float fMaxTime, GameInstance* pGameInstance)
{
	const _float fMaxVolume = 0.3f;
	const _float fMinVolume = 0.f;
	
	fVolume = fMaxVolume - (time / fMaxTime) * (fMaxVolume - fMinVolume);

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

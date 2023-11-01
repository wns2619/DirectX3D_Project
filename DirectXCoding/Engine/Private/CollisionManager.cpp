#include "pch.h"
#include "CollisionManager.h"
#include "Collider.h"
#include "GameInstance.h"
#include "Layer.h"
#include "Bounding.h"

IMPLEMENT_SINGLETON(CollisionManager)

CollisionManager::CollisionManager()
{
	::ZeroMemory(_arrCheck, 0, sizeof(uint32));

	CheckGroup(LAYER_TAG::LAYER_PLAYER, LAYER_TAG::LAYER_MONSTER);
	CheckGroup(LAYER_TAG::LAYER_PLAYER, LAYER_TAG::LAYER_DYNAMIC);
	CheckGroup(LAYER_TAG::LAYER_PLAYER, LAYER_TAG::LAYER_PUZZLE);
	CheckGroup(LAYER_TAG::LAYER_MONSTER, LAYER_TAG::LAYER_DYNAMIC);

}

void CollisionManager::Tick(const _float& timeDelta)
{
}

void CollisionManager::LateTick(const _float& timeDelta)
{
	for (uint32 iRow = 0; iRow < static_cast<uint32>(LAYER_TAG::LAYER_END); ++iRow)
		for (uint32 iCol = iRow; iCol < static_cast<uint32>(LAYER_TAG::LAYER_COLLIDER); ++iCol)
			if (_arrCheck[iRow] & (1 << iCol))
				CheckCollisionByType(static_cast<LAYER_TAG>(iRow), static_cast<LAYER_TAG>(iCol));


	CheckDoorCollision();
}

_bool CollisionManager::IsCollision(Collider* pDest, Collider* pSour)
{
	_bool IsCol = false;

	IsCol = pDest->IsCollisition(pSour);

	return IsCol;
}

void CollisionManager::CheckCollisionByType(LAYER_TAG eLayerTagLeft, LAYER_TAG eLayerTagRight)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	const map<const LAYER_TAG, Layer*>* pLayer = pGameInstance->GetEntireObjectLayer();
	if (pLayer == nullptr)
	{
		RELEASE_INSTANCE(GameInstance);
		return;
	}

	if (eLayerTagLeft == LAYER_TAG::LAYER_STATIC || eLayerTagRight == LAYER_TAG::LAYER_STATIC)
	{
		RELEASE_INSTANCE(GameInstance);
		return;
	}

	uint32 iCurrentLevel = pGameInstance->GetCurrentLevelIndex();

	// 현재 레벨의 레이어의 오브젝트를 순회.
	auto pLeft = pLayer[iCurrentLevel].find(eLayerTagLeft);
	auto pRight = pLayer[iCurrentLevel].find(eLayerTagRight);


	if (pLeft == pLayer[iCurrentLevel].end() || pRight == pLayer[iCurrentLevel].end())
	{
		RELEASE_INSTANCE(GameInstance);
		return;
	}

	RELEASE_INSTANCE(GameInstance);

	map<uint64, _bool>::iterator iter;

	// Left , Right.
	vector<GameObject*>* pLeftGameObject = pLeft->second->GetGameObject();
	vector<GameObject*>* pRightGameObject = pRight->second->GetGameObject();
	

	for (auto& iterL : *pLeftGameObject)
	{
		if (nullptr == iterL->GetCollider())
			continue;

		for (auto& iterR : *pRightGameObject)
		{
			if (nullptr == iterR->GetCollider() || iterL == iterR)
				continue;

			if (iterL->GetModelName() == "Player" && iterR->GetModelName() == "Bullet")
				continue;
			

			Collider* pLeftCol = iterL->GetCollider();
			Collider* pRightCol = iterR->GetCollider();

			COLLIDER_ID ID;
			ID.LeftID = pLeftCol->GetID();
			ID.RightID = pRightCol->GetID();

			iter = _mapColInfo.find(ID.ID);

			if (_mapColInfo.end() == iter)
			{
				_mapColInfo.emplace(make_pair( ID.ID, false));
				iter = _mapColInfo.find(ID.ID);
			}

			if (IsCollision(pLeftCol, pRightCol))
			{
				// 현재 충돌중
				if (iter->second)
				{
					// 이전에도 충돌한 상태였다면. -> 계속 true
					if (iterL->IsDead() || iterR->IsDead())
					{
						pLeftCol->OnCollisionExit(pRightCol);
						pRightCol->OnCollisionExit(pLeftCol);
						iter->second = false;

						pLeftCol->GetBounding()->SetColl(false);
						pRightCol->GetBounding()->SetColl(false);
					}
					else
					{
						pLeftCol->OnCollisionStay(pRightCol);
						pRightCol->OnCollisionStay(pLeftCol);

						pLeftCol->GetBounding()->SetColl(true);
						pRightCol->GetBounding()->SetColl(true);
					}
				}
				else
				{
					// 이전에는 충돌 X
					if (!iterL->IsDead() && !iterR->IsDead())
					{
						pLeftCol->OnCollisionEnter(pRightCol);
						pRightCol->OnCollisionEnter(pLeftCol);
						iter->second = true;

						pLeftCol->GetBounding()->SetColl(true);
						pRightCol->GetBounding()->SetColl(true);
					}
					else
					{
						pLeftCol->OnCollisionExit(pRightCol);
						pRightCol->OnCollisionExit(pLeftCol);
						iter->second = false;

						pLeftCol->GetBounding()->SetColl(false);
						pRightCol->GetBounding()->SetColl(false);
					}
				}
			}
			else
			{
				if (iter->second)
				{
					pLeftCol->OnCollisionExit(pRightCol);
					pRightCol->OnCollisionExit(pLeftCol);
					iter->second = false;

					pLeftCol->GetBounding()->SetColl(false);
					pRightCol->GetBounding()->SetColl(false);
				}
			}
		}
	}

}

void CollisionManager::CheckGroup(LAYER_TAG eLeft, LAYER_TAG eRight)
{
	uint32 iRow = static_cast<uint32>(eLeft);
	uint32 iCol = static_cast<uint32>(eRight);

	if (iCol < iRow)
	{
		iRow = static_cast<uint32>(eRight);
		iCol = static_cast<uint32>(eLeft);
	}

	if (_arrCheck[iRow] & (1 << iCol))
		_arrCheck[iRow] &= ~(1 << iCol);
	else
		_arrCheck[iRow] |= (1 << iCol);
}

void CollisionManager::CheckDoorCollision()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	const map<const LAYER_TAG, Layer*>* pLayer = pGameInstance->GetEntireObjectLayer();
	if (pLayer == nullptr)
		return;

	uint32 iCurrentLevel = pGameInstance->GetCurrentLevelIndex();

	// 현재 레벨의 레이어의 오브젝트를 순회.
	auto pLeft = pLayer[iCurrentLevel].find(LAYER_TAG::LAYER_COLLIDER);
	auto pRight = pLayer[iCurrentLevel].find(LAYER_TAG::LAYER_DYNAMIC);


	if (pLeft == pLayer[iCurrentLevel].end() || pRight == pLayer[iCurrentLevel].end())
	{
		RELEASE_INSTANCE(GameInstance);
		return;
	}

	RELEASE_INSTANCE(GameInstance);

	map<uint64, _bool>::iterator iter;

	// Left , Right.
	vector<GameObject*>* pLeftGameObject = pLeft->second->GetGameObject();
	vector<GameObject*>* pRightGameObject = pRight->second->GetGameObject();


	for (auto& iterL : *pLeftGameObject)
	{
		if (nullptr == iterL->GetCollider())
			continue;


		for (auto& iterR : *pRightGameObject)
		{
			if (nullptr == iterR->GetAssistCollider() || iterL == iterR)
				continue;

			Collider* pLeftCol = iterL->GetCollider();
			Collider* pRightCol = iterR->GetAssistCollider();

			COLLIDER_ID ID;
			ID.LeftID = pLeftCol->GetID();
			ID.RightID = pRightCol->GetID();

			iter = _mapColInfo.find(ID.ID);

			if (_mapColInfo.end() == iter)
			{
				_mapColInfo.emplace(make_pair(ID.ID, false));
				iter = _mapColInfo.find(ID.ID);
			}

			if (IsCollision(pLeftCol, pRightCol))
			{
				// 현재 충돌중
				if (iter->second)
				{
					// 이전에도 충돌한 상태였다면. -> 계속 true
					if (iterL->IsDead() || iterR->IsDead())
					{
						pLeftCol->OnCollisionExit(pRightCol);
						pRightCol->OnCollisionExit(pLeftCol);
						iter->second = false;

						pLeftCol->GetBounding()->SetColl(false);
						pRightCol->GetBounding()->SetColl(false);
					}
					else
					{
						pLeftCol->OnCollisionStay(pRightCol);
						pRightCol->OnCollisionStay(pLeftCol);

						pLeftCol->GetBounding()->SetColl(true);
						pRightCol->GetBounding()->SetColl(true);
					}
				}
				else
				{
					// 이전에는 충돌 X
					if (!iterL->IsDead() && !iterR->IsDead())
					{
						pLeftCol->OnCollisionEnter(pRightCol);
						pRightCol->OnCollisionEnter(pLeftCol);
						iter->second = true;

						pLeftCol->GetBounding()->SetColl(true);
						pRightCol->GetBounding()->SetColl(true);
					}
					else
					{
						pLeftCol->OnCollisionExit(pRightCol);
						pRightCol->OnCollisionExit(pLeftCol);
						iter->second = false;

						pLeftCol->GetBounding()->SetColl(false);
						pRightCol->GetBounding()->SetColl(false);
					}
				}
			}
			else
			{
				if (iter->second)
				{
					pLeftCol->OnCollisionExit(pRightCol);
					pRightCol->OnCollisionExit(pLeftCol);
					iter->second = false;

					pLeftCol->GetBounding()->SetColl(false);
					pRightCol->GetBounding()->SetColl(false);
				}
			}
		}
	}

}

void CollisionManager::Free()
{
	__super::Free();
}

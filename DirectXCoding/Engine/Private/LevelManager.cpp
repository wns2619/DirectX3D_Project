#include "pch.h"
#include "LevelManager.h"
#include "Level.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(LevelManager)

LevelManager::LevelManager()
{
}

void LevelManager::Tick(const _float& timeDelta)
{
	if (nullptr == _CurrentLevel)
		return;

	_CurrentLevel->Tick(timeDelta);
}

void LevelManager::LateTick(const _float& timeDelta)
{
	if (nullptr == _CurrentLevel)
		return;

	_CurrentLevel->LateTick(timeDelta);
}

HRESULT LevelManager::OpenLevel(uint32 levelIndex, Level* newLevel)
{
	if (nullptr != _CurrentLevel)
	{
		GameInstance* gameInstance = GameInstance::GetInstance();
		Safe_AddRef<GameInstance*>(gameInstance);

		gameInstance->Clear(_CurrentLevelIndex);

		Safe_Release<GameInstance*>(gameInstance);
	}

	Safe_Release<Level*>(_CurrentLevel);

	_CurrentLevel = newLevel;

	_CurrentLevelIndex = levelIndex;

	return S_OK;
}

void LevelManager::Free()
{
	Safe_Release<Level*>(_CurrentLevel);
}

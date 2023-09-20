#pragma once

#include "Base.h"

BEGIN(Engine)

class Level;

class LevelManager final : public Base
{
	DECLARE_SINGLETON(LevelManager)

private:
	LevelManager();
	virtual ~LevelManager() = default;

public:
	void Tick(const _float& timeDelta);
	void LateTick(const _float& timeDelta);
public:
	HRESULT	OpenLevel(uint32 levelIndex, Level* newLevel);

	uint32 GetCurrentLevelIndex() { return _CurrentLevelIndex; }

private:
	Level* _CurrentLevel = nullptr;
	uint32 _CurrentLevelIndex = 0;

public:
	virtual void Free() override;
};

END
#pragma once

#include "Base.h"

BEGIN(Engine)

class Collider;

class CollisionManager final : public Base
{
	DECLARE_SINGLETON(CollisionManager)

private:
	CollisionManager();
	virtual ~CollisionManager() = default;

public:
	void Tick(const _float& timeDelta);
	void LateTick(const _float& timeDelta);

private:
	_bool IsCollision(Collider* pDesc, Collider* pSour);
	void CheckCollisionByType(LAYER_TAG eLayerTagLeft, LAYER_TAG eLayerTagRight);
	void CheckGroup(LAYER_TAG eLeft, LAYER_TAG eRight);
	
	void CheckDoorCollision();

private:
	map<uint64, _bool> _mapColInfo;
	uint32 _arrCheck[static_cast<uint32>(LAYER_TAG::LAYER_END)] = {};

public:
	virtual void Free() override;
};

END


#pragma once

#include "Component.h"
#include "Transform.h"
#include "VIBufferTerrain.h"

BEGIN(Engine)

class ENGINE_DLL Picking : public Base
{
	DECLARE_SINGLETON(Picking)

private:
	Picking();
	virtual ~Picking() = default;

public:
	_bool PickObject(POINT pt);
	_bool TerrainPicking(POINT pt, Vec3& pickPos, _float& distance, Transform* trans, VIBufferTerrain* buffer);
	
public:
	virtual void Free() override;
};


END


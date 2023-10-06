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
	Vec4 TerrainPicking(POINT pt, Transform* trans, VIBufferTerrain* buffer);
	
public:
	virtual void Free() override;
};


END


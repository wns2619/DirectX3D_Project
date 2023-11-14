#pragma once

#include "Base.h"

BEGIN(Engine)

class Frustum final : public Base
{
	DECLARE_SINGLETON(Frustum)

private:
	Frustum();
	virtual ~Frustum() = default;

public:
	HRESULT Initialize();
	void Tick();
	_bool IsIn_FrustumWorld(FXMVECTOR vWorldPos, _float fRadius);

private:
	class CameraHelper* _pCameraHelper = nullptr;
	Vec3 _vOriginalPoints[8];
	Vec3 _vPoints[8];

	Vec4 _vWorldPlanes[6];

private:
	void MakePlanes(const Vec3* pPoints, Vec4* pPlanes);

public:
	virtual void Free() override;
};

END

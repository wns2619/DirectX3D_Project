#include "pch.h"
#include "Frustum.h"
#include "CameraHelper.h"

IMPLEMENT_SINGLETON(Frustum)

Frustum::Frustum()
	: _pCameraHelper(CameraHelper::GetInstance())
{
	Safe_AddRef<CameraHelper*>(_pCameraHelper);
}

HRESULT Frustum::Initialize()
{
	_vOriginalPoints[0] = Vec3(-1.f, 1.f, 0.f);
	_vOriginalPoints[1] = Vec3(1.f, 1.f, 0.f);
	_vOriginalPoints[2] = Vec3(1.f, -1.f, 0.f);
	_vOriginalPoints[3] = Vec3(-1.f, -1.f, 0.f);

	_vOriginalPoints[4] = Vec3(-1.f, 1.f, 1.f);
	_vOriginalPoints[5] = Vec3(1.f, 1.f, 1.f);
	_vOriginalPoints[6] = Vec3(1.f, -1.f, 1.f);
	_vOriginalPoints[7] = Vec3(-1.f, -1.f, 1.f);

	return S_OK;
}

void Frustum::Tick()
{
	Matrix ProjMatrixInv = _pCameraHelper->GetInverseTransformCalculator(CameraHelper::TRANSFORMSTATE::D3DTS_PROJ);
	Matrix ViewMatrixInv = _pCameraHelper->GetInverseTransformCalculator(CameraHelper::TRANSFORMSTATE::D3DTS_VIEW);

	for (size_t i = 0; i < 8; i++)
	{
		_vPoints[i] = XMVector3TransformCoord(_vOriginalPoints[i], ProjMatrixInv);
		_vPoints[i] = XMVector3TransformCoord(_vPoints[i], ViewMatrixInv);
	}

	MakePlanes(_vPoints, _vWorldPlanes);
}

_bool Frustum::IsIn_FrustumWorld(FXMVECTOR vWorldPos, _float fRadius)
{
	for (size_t i = 0; i < 6; i++)
		if (fRadius < ::XMVectorGetX(XMPlaneDotCoord(_vWorldPlanes[i], vWorldPos)))
			return false;

	return true;
}

void Frustum::MakePlanes(const Vec3* pPoints, Vec4* pPlanes)
{
	pPlanes[0] = ::XMPlaneFromPoints(pPoints[1], pPoints[5], pPoints[6]);
	pPlanes[1] = ::XMPlaneFromPoints(pPoints[4], pPoints[0], pPoints[3]);
	pPlanes[2] = ::XMPlaneFromPoints(pPoints[4], pPoints[5], pPoints[1]);
	pPlanes[3] = ::XMPlaneFromPoints(pPoints[3], pPoints[2], pPoints[6]);
	pPlanes[4] = ::XMPlaneFromPoints(pPoints[5], pPoints[4], pPoints[7]);
	pPlanes[5] = ::XMPlaneFromPoints(pPoints[0], pPoints[1], pPoints[2]);
}

void Frustum::Free()
{
	Safe_Release<CameraHelper*>(_pCameraHelper);
}

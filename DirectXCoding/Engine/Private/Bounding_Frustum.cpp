#include "pch.h"
#include "Bounding_Frustum.h"
#include "BoundingAABB.h"
#include "BoundingOBB.h"
#include "Bounding_Sphere.h"
#include "DebugDraw.h"

Bounding_Frustum::Bounding_Frustum()
{
}

HRESULT Bounding_Frustum::Initialize(BOUNDING_DESC* pDesc)
{
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	BOUNDING_FRUSTUM_DESC* pBoundingFrustumDesc = static_cast<BOUNDING_FRUSTUM_DESC*>(pDesc);
	

	_pFrustum_Original = new BoundingFrustum(pBoundingFrustumDesc->vCenter, pBoundingFrustumDesc->vOrientation,
		pBoundingFrustumDesc->fRightSlope, pBoundingFrustumDesc->fLeftSlope, pBoundingFrustumDesc->fTopSlope, pBoundingFrustumDesc->fBottomSlope,
		pBoundingFrustumDesc->fNear, pBoundingFrustumDesc->fFar);

	_pFrustum = new BoundingFrustum(*_pFrustum_Original);
	return S_OK;
}

void Bounding_Frustum::Update(FXMMATRIX TransformMatrix)
{
	_pFrustum_Original->Transform(*_pFrustum, TransformMatrix);
}

_bool Bounding_Frustum::IsCollision(Collider::COLLIDER_TYPE eType, Bounding* pBounding)
{
	_bool bIntersects = false;

	switch (eType)
	{
	case Engine::Collider::AABB:
		bIntersects = _pFrustum->Intersects(*static_cast<BoundingAABB*>(pBounding)->GetBounding());
		break;
	case Engine::Collider::OBB:
		bIntersects = _pFrustum->Intersects(*static_cast<BoundingOBB*>(pBounding)->GetBounding());
		break;
	case Engine::Collider::SPHERE:
		bIntersects = _pFrustum->Intersects(*static_cast<Bounding_Sphere*>(pBounding)->GetBounding());
		break;
	default:
		break;
	}

	return bIntersects;
}

#ifdef _DEBUG

HRESULT Bounding_Frustum::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	Color vColor = _IsColl == true ? Color(1.f, 0.f, 0.f, 1.f) : Color(0.f, 1.f, 0.f, 1.f);

	DX::Draw(pBatch, *_pFrustum, ::XMLoadFloat4(&vColor));
	return S_OK;
}


#endif // _DEBUG



Bounding_Frustum* Bounding_Frustum::Create(BOUNDING_DESC* pDesc)
{
	Bounding_Frustum* pInstance = new Bounding_Frustum();

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : Bounding Frustum");
		Safe_Release<Bounding_Frustum*>(pInstance);
	}

	return pInstance;
}

void Bounding_Frustum::Free()
{
	__super::Free();

	Safe_Delete<BoundingFrustum*>(_pFrustum_Original);
	Safe_Delete<BoundingFrustum*>(_pFrustum);
}

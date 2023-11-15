#include "pch.h"
#include "BoundingAABB.h"
#include "DebugDraw.h"

#include "BoundingOBB.h"
#include "Bounding_Sphere.h"
#include "Bounding_Frustum.h"

BoundingAABB::BoundingAABB()
{
}

HRESULT BoundingAABB::Initialize(BOUNDING_DESC* pDesc)
{
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	BOUNDING_AABB_DESC* pAABBDesc = static_cast<BOUNDING_AABB_DESC*>(pDesc);

	_pAABB_Original = new BoundingBox(pAABBDesc->vCenter, pAABBDesc->vExtents);
	_pAABB = new BoundingBox(*_pAABB_Original);

	return S_OK;
}

void BoundingAABB::Update(FXMMATRIX TransformMatrix)
{
	XMMATRIX Matrix = TransformMatrix;

	Matrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[0]));
	Matrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[1]));
	Matrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[2]));

	_pAABB_Original->Transform(*_pAABB, Matrix);
}


_bool BoundingAABB::IsCollision(Collider::COLLIDER_TYPE eType, Bounding* pBounding)
{
	_bool bIntersects = false;

	switch (eType)
	{
	case Engine::Collider::AABB:
		bIntersects = _pAABB->Intersects(*dynamic_cast<BoundingAABB*>(pBounding)->GetBounding());
		break;
	case Engine::Collider::OBB:
		bIntersects = _pAABB->Intersects(*dynamic_cast<BoundingOBB*>(pBounding)->GetBounding());
		break;
	case Engine::Collider::SPHERE:
		bIntersects = _pAABB->Intersects(*dynamic_cast<Bounding_Sphere*>(pBounding)->GetBounding());
		break;
	case Engine::Collider::FRUSTUM:
		bIntersects = _pAABB->Intersects(*dynamic_cast<Bounding_Frustum*>(pBounding)->GetBounding());
		break;
	default:
		break;
	}

	return bIntersects;
}

#ifdef _DEBUG

HRESULT BoundingAABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	Color vColor = _IsColl == true ? Color(1.f, 0.f, 0.f, 1.f) : Color(0.f, 1.f, 0.f, 1.f);

	DX::Draw(pBatch, *_pAABB, ::XMLoadFloat4(&vColor));
	return S_OK;
}

#endif // _DEBUG

BoundingAABB* BoundingAABB::Create(BOUNDING_DESC* pDesc)
{
	BoundingAABB* pInstance = new BoundingAABB();

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : Bounding AABB");
		Safe_Release<BoundingAABB*>(pInstance);
	}

	return pInstance;
}

void BoundingAABB::Free()
{
	__super::Free();

	Safe_Delete<BoundingBox*>(_pAABB_Original);
	Safe_Delete<BoundingBox*>(_pAABB);
}

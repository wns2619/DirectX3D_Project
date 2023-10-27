#include "pch.h"
#include "BoundingOBB.h"
#include "DebugDraw.h"
#include "BoundingAABB.h"
#include "Bounding_Sphere.h"

BoundingOBB::BoundingOBB()
{
}

HRESULT BoundingOBB::Initialize(BOUNDING_DESC* pDesc)
{
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	BOUNDING_OBB_DESC* pOBBDesc = static_cast<BOUNDING_OBB_DESC*>(pDesc);

	_pOBB_Original = new BoundingOrientedBox(pOBBDesc->vCenter, pOBBDesc->vExtents, pOBBDesc->vRotation);
	_pOBB = new BoundingOrientedBox(*_pOBB_Original);

	return S_OK;
}

void BoundingOBB::Update(FXMMATRIX TransformMatrix)
{
	_pOBB_Original->Transform(*_pOBB, TransformMatrix);
}


_bool BoundingOBB::IsCollision(Collider::COLLIDER_TYPE eType, Bounding* pBounding)
{
	_bool testfalse = false;

	switch (eType)
	{
	case Engine::Collider::AABB:
		testfalse = _pOBB->Intersects(*dynamic_cast<BoundingAABB*>(pBounding)->GetBounding());
		break;
	case Engine::Collider::OBB:
		testfalse = _pOBB->Intersects(*dynamic_cast<BoundingOBB*>(pBounding)->GetBounding());
		break;
	case Engine::Collider::SPHERE:
		testfalse = _pOBB->Intersects(*dynamic_cast<Bounding_Sphere*>(pBounding)->GetBounding());
		break;
	default:
		break;
	}

	return testfalse;
}

#ifdef _DEBUG

HRESULT BoundingOBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	Color vColor = _IsColl == true ? Color(1.f, 0.f, 0.f, 1.f) : Color(0.f, 1.f, 0.f, 1.f);

	DX::Draw(pBatch, *_pOBB, ::XMLoadFloat4(&vColor));

	return S_OK;
}

#endif // _DEBUG

BoundingOBB* BoundingOBB::Create(BOUNDING_DESC* pDesc)
{
	BoundingOBB* pInstance = new BoundingOBB();

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : Bounding AABB");
		Safe_Release<BoundingOBB*>(pInstance);
	}

	return pInstance;
}

void BoundingOBB::Free()
{
	__super::Free();

	Safe_Delete<BoundingOrientedBox*>(_pOBB_Original);
	Safe_Delete<BoundingOrientedBox*>(_pOBB);
}

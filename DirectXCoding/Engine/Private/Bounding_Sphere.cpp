#include "pch.h"
#include "Bounding_Sphere.h"
#include "DebugDraw.h"

#include "BoundingAABB.h"
#include "BoundingOBB.h"

Bounding_Sphere::Bounding_Sphere()
{
}

HRESULT Bounding_Sphere::Initialize(BOUNDING_DESC* pDesc)
{
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	BOUNDING_SPHERE_DESC* pSphereDesc = static_cast<BOUNDING_SPHERE_DESC*>(pDesc);

	_pSphere_Original = new BoundingSphere(pSphereDesc->vCenter, pSphereDesc->fRadius);
	_pSphere = new BoundingSphere(*_pSphere_Original);

	return S_OK;
}

void Bounding_Sphere::Update(FXMMATRIX TransformMatrix)
{
	_pSphere_Original->Transform(*_pSphere, TransformMatrix);
}

_bool Bounding_Sphere::IsCollision(Collider::COLLIDER_TYPE eType, Bounding* pBounding)
{
	_IsColl = false;

	switch (eType)
	{
	case Engine::Collider::AABB:
		_IsColl = _pSphere->Intersects(*dynamic_cast<BoundingAABB*>(pBounding)->GetBounding());
		break;
	case Engine::Collider::OBB:
		_IsColl = _pSphere->Intersects(*dynamic_cast<BoundingOBB*>(pBounding)->GetBounding());
		break;
	case Engine::Collider::SPHERE:
		_IsColl = _pSphere->Intersects(*dynamic_cast<Bounding_Sphere*>(pBounding)->GetBounding());
		break;
	default:
		break;
	}

	return _IsColl;
}

#ifdef _DEBUG
HRESULT Bounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *_pSphere);

	return S_OK;
}
#endif // _DEBUG



Bounding_Sphere* Bounding_Sphere::Create(BOUNDING_DESC* pDesc)
{
	Bounding_Sphere* pInstance = new Bounding_Sphere();

	if (FAILED(pInstance->Initialize(pDesc)))
	{
		MSG_BOX("Failed to Created : Bounding Sphere");
		Safe_Release<Bounding_Sphere*>(pInstance);
	}

    return pInstance;
}

void Bounding_Sphere::Free()
{
	__super::Free();

	Safe_Delete<BoundingSphere*>(_pSphere_Original);
	Safe_Delete<BoundingSphere*>(_pSphere);
}

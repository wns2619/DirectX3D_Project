#include "pch.h"
#include "BoundingOBB.h"
#include "DebugDraw.h"

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

#ifdef _DEBUG
HRESULT BoundingOBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *_pOBB);

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

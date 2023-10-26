#include "pch.h"
#include "BoundingAABB.h"
#include "DebugDraw.h"

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
	_pAABB_Original->Transform(*_pAABB, TransformMatrix);
}

#ifdef _DEBUG
HRESULT BoundingAABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
	DX::Draw(pBatch, *_pAABB);

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

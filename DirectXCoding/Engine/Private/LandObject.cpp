#include "pch.h"
#include "LandObject.h"

#include "Transform.h"
#include "VIBufferCell.h"

LandObject::LandObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext, OBJECT_TYPE etype)
	: GameObject(device, deviceContext, etype)
{
}

LandObject::LandObject(const LandObject& rhs)
	: GameObject(rhs)
{
}

HRESULT LandObject::InitializePrototype()
{
	return S_OK;
}

HRESULT LandObject::Initialize(void* argument)
{
	if (nullptr == argument)
		return E_FAIL;

	LANDOBJET_DESC* pDesc = static_cast<LANDOBJET_DESC*>(argument);

	_pCellTransform = pDesc->pCellTransform;
	_pCellBuffer = pDesc->pCellBuffer;

	Safe_AddRef<Transform*>(_pCellTransform);
	Safe_AddRef<VIBufferCell*>(_pCellBuffer);

	return S_OK;
}

void LandObject::PriorityTick(const _float& timeDelta)
{
}

void LandObject::Tick(const _float& timeDelta)
{
}

void LandObject::LateTick(const _float& timeDelta)
{
}

HRESULT LandObject::Render()
{
	return S_OK;
}

XMVECTOR LandObject::SetUp_OnCell(FXMVECTOR vWorldPos)
{
	return _pCellBuffer->SetUp_OnCell(_pCellTransform, vWorldPos);
}

void LandObject::Free()
{
	__super::Free();

	Safe_Release<Transform*>(_pCellTransform);
	Safe_Release<VIBufferCell*>(_pCellBuffer);
}

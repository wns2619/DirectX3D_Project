#include "pch.h"
#include "MonsterPartObject.h"

#include "GameInstance.h"

MonsterPartObject::MonsterPartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: GameObject(pDevice, pDeviceContext, OBJECT_TYPE::MONSTER)
{

}

MonsterPartObject::MonsterPartObject(const MonsterPartObject& rhs)
	: GameObject(rhs)
{

}

Matrix* MonsterPartObject::GetSocketBoneMatrix(const _char* pBoneName)
{
	if (nullptr == _binaryModel)
		return nullptr;

	return _binaryModel->GetBoneMatrix(pBoneName);
}

Matrix MonsterPartObject::GetSocketPivotMatrix()
{
	if (nullptr == _binaryModel)
		return Matrix();

	return _binaryModel->GetPivotMatrix();
}

HRESULT MonsterPartObject::InitializePrototype()
{
	return S_OK;
}

HRESULT MonsterPartObject::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		PART_DESC* pPartDesc = static_cast<PART_DESC*>(pArg);
		_pParentTransform = pPartDesc->pParentTransform;
		Safe_AddRef<Transform*>(_pParentTransform);
	}

	return S_OK;
}

void MonsterPartObject::Tick(const _float& fTimeDelta)
{
}

void MonsterPartObject::LateTick(const _float& fTimeDelta)
{
}

HRESULT MonsterPartObject::Render()
{
	return S_OK;
}

HRESULT MonsterPartObject::Compute_RenderMatrix(FXMMATRIX ChildeMatrix)
{
	_mWorldMatrix = ChildeMatrix * _pParentTransform->GetWorldMatrixCaculator();

	return S_OK;
}

MonsterPartObject* MonsterPartObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	return nullptr;
}

void MonsterPartObject::Free()
{
	__super::Free();

	Safe_Release<Transform*>(_pParentTransform);
	Safe_Release<Shader*>(_pShader);

	for (uint32 i = 0; i < 2; ++i)
		Safe_Release<Texture*>(_pMeshTexture[i]);

	Safe_Release<Texture*>(_pTexture);

}

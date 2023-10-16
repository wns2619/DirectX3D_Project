#include "pch.h"
#include "PartObject.h"

#include "GameInstance.h"

PartObject::PartObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : GameObject(pDevice, pContext, OBJECT_TYPE::PLAYER)
{
}

PartObject::PartObject(const PartObject& rhs)
    : GameObject(rhs)
{
}

Matrix* PartObject::Get_SocketBonePtr(const char* pBoneName)
{
    if (nullptr == _binaryModel)
        return nullptr;

    return _binaryModel->GetBoneMatrix(pBoneName);
    // 소켓을 붙일 뼈의 매트릭스 정보.
}

Matrix PartObject::Get_SocketPivotMatrix()
{
    if (nullptr == _binaryModel)
        return Matrix();

    return _binaryModel->GetPivotMatrix();
}

HRESULT PartObject::InitializePrototype()
{
    return S_OK;
}

HRESULT PartObject::Initialize(void* pArg)
{
    if (nullptr != pArg)
    {
        PART_DESC* pPartDesc = static_cast<PART_DESC*>(pArg);
        _pParentTransform = pPartDesc->pParentTransform;
        Safe_AddRef<Transform*>(_pParentTransform);
    }

    return S_OK;
}

void PartObject::Tick(const _float& fTimeDelta)
{
}

void PartObject::LateTick(const _float& fTimeDelta)
{
}

HRESULT PartObject::Render()
{
    return S_OK;
}

HRESULT PartObject::Compute_RenderMatrix(FXMMATRIX ChildMatrix)
{
    _WorldMatrix = ChildMatrix * _pParentTransform->GetWorldMatrixCaculator();

    return S_OK;
}

PartObject* PartObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return nullptr;
}

GameObject* PartObject::Clone(void* pArg)
{
    return nullptr;
}

void PartObject::Free()
{
    __super::Free();

    Safe_Release<Transform*>(_pParentTransform);
}

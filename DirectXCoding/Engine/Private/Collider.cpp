#include "pch.h"
#include "Collider.h"
#include "CameraHelper.h"
#include "BoundingAABB.h"
#include "BoundingOBB.h"
#include "Bounding_Sphere.h"
#include "GameObject.h"
#include "Bounding_Frustum.h"

uint32 Collider::_giNextID = 0;

Collider::Collider(ID3D11Device* device, ID3D11DeviceContext* deviceContext, COLLIDER_USAGE eUsage)
    : Component(device, deviceContext, COMPONENT_TYPE::COLLIDER), _eColliderUsage(eUsage)
{

}

Collider::Collider(const Collider& rhs)
    : Component(rhs)
    , _eColliderType(rhs._eColliderType)
    , _eColliderUsage(rhs._eColliderUsage)
#ifdef _DEBUG
    , _pBatch(rhs._pBatch)
    , _pEffect(rhs._pEffect)
    , _pInputLayOut(rhs._pInputLayOut)
    , _pDSState(rhs._pDSState)
#endif // _DEBUG
{
#ifdef _DEBUG
    Safe_AddRef<ID3D11DepthStencilState*>(_pDSState);
    Safe_AddRef<ID3D11InputLayout*>(_pInputLayOut);
#endif // _DEBUG
}

HRESULT Collider::InitializePrototype(COLLIDER_TYPE eType)
{
    _eColliderType = eType;

#ifdef _DEBUG
    /* µð¹ö±ë ÁØºñ */
    _pBatch = new PrimitiveBatch<VertexPositionColor>(_deviceContext);
    //
    _pEffect = new BasicEffect(_device);
    _pEffect->SetVertexColorEnabled(true);

    const void* pShaderByteCode = nullptr;
    size_t iShaderByteCodeLength = 0;

    _pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

    if (FAILED(_device->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeLength, &_pInputLayOut)))
        return E_FAIL;

    D3D11_DEPTH_STENCIL_DESC DSStateDesc = {};
    DSStateDesc.DepthEnable = true;
    DSStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DSStateDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    
    DSStateDesc.StencilEnable = false;

    if (FAILED(_device->CreateDepthStencilState(&DSStateDesc, &_pDSState)))
        return E_FAIL;

#endif // _DEBUG


    return S_OK;
}

HRESULT Collider::Initialize(void* argument)
{
    Bounding::BOUNDING_DESC* pBoundingDesc = static_cast<Bounding::BOUNDING_DESC*>(argument);

    _iID = _giNextID++;

    _pOwner = pBoundingDesc->pOwner;

    switch (_eColliderType)
    {
    case Engine::Collider::AABB:
        _pBounding = BoundingAABB::Create(pBoundingDesc);
        break;
    case Engine::Collider::OBB:
        _pBounding = BoundingOBB::Create(pBoundingDesc);
        break;
    case Engine::Collider::SPHERE:
        _pBounding = Bounding_Sphere::Create(pBoundingDesc);
        break;
    case Engine::Collider::FRUSTUM:
        _pBounding = Bounding_Frustum::Create(pBoundingDesc);
        break;
    default:
        break;
    }

    if (nullptr == _pBounding)
        return E_FAIL;

    return S_OK;
}

#ifdef _DEBUG

HRESULT Collider::Render()
{
    _deviceContext->OMSetDepthStencilState(_pDSState, 0);

    CameraHelper* pCameraHelper = GET_INSTANCE(CameraHelper);

   _pEffect->SetWorld(::XMMatrixIdentity());
   _pEffect->SetView(pCameraHelper->GetTransformCalculator(CameraHelper::TRANSFORMSTATE::D3DTS_VIEW));
   _pEffect->SetProjection(pCameraHelper->GetTransformCalculator(CameraHelper::TRANSFORMSTATE::D3DTS_PROJ));

   RELEASE_INSTANCE(CameraHelper);

   _deviceContext->IASetInputLayout(_pInputLayOut);
   _pEffect->Apply(_deviceContext);

   _pBatch->Begin();

   _pBounding->Render(_pBatch);

   _pBatch->End();

    return S_OK;
}

#endif // _DEBUG

_bool Collider::IsCollisition(Collider* pTargetCol)
{
    return _pBounding->IsCollision(pTargetCol->_eColliderType, pTargetCol->_pBounding);
}

void Collider::OnCollisionEnter(Collider* other)
{
    ++_iCol;

    if (COLLIDER_USAGE::MAIN_COLLIDER == _eColliderUsage)
        _pOwner->OnCollisionEnter(other);
    else
        _pOwner->OnAssistCollisionEnter(other);

}

void Collider::OnCollisionStay(Collider* other)
{
    if (COLLIDER_USAGE::MAIN_COLLIDER == _eColliderUsage)
        _pOwner->OnCollisionStay(other);
    else
        _pOwner->OnAssistCollisionStay(other);

}

void Collider::OnCollisionExit(Collider* other)
{
    --_iCol;
    
    if (COLLIDER_USAGE::MAIN_COLLIDER == _eColliderUsage)
        _pOwner->OnCollisionExit(other);
    else
        _pOwner->OnAssistCollisionExit(other);

}

Collider* Collider::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, COLLIDER_TYPE eType, COLLIDER_USAGE eUsage)
{
    Collider* pInstance = new Collider(device, deviceContext, eUsage);

    if (FAILED(pInstance->InitializePrototype(eType)))
    {
        MSG_BOX("Failed To Created : Collider");
        Safe_Release<Collider*>(pInstance);
    }

    return pInstance;
}

Component* Collider::Clone(void* pArg)
{
    Collider* pInstance = new Collider(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : Collider");
        Safe_Release<Collider*>(pInstance);
    }

    return pInstance;
}

void Collider::Free()
{
    __super::Free();

#ifdef _DEBUG
    Safe_Release<ID3D11InputLayout*>(_pInputLayOut);
    Safe_Release<ID3D11DepthStencilState*>(_pDSState);
    if (false == _isCloned)
    {
        Safe_Delete<DirectX::PrimitiveBatch<VertexPositionColor>*>(_pBatch);
        Safe_Delete<BasicEffect*>(_pEffect);
    }
#endif // _DEBUG

    Safe_Release<Bounding*>(_pBounding);

}

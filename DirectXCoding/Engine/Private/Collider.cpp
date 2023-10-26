#include "pch.h"
#include "Collider.h"
#include "CameraHelper.h"
#include "BoundingAABB.h"
#include "BoundingOBB.h"
#include "Bounding_Sphere.h"

Collider::Collider(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
    : Component(device, deviceContext, COMPONENT_TYPE::COLLIDER)
{

}

Collider::Collider(const Collider& rhs)
    : Component(rhs)
    , _eColliderType(rhs._eColliderType)
#ifdef _DEBUG
    , _pBatch(rhs._pBatch)
    , _pEffect(rhs._pEffect)
#endif // _DEBUG
{

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
#endif // _DEBUG


    return S_OK;
}

HRESULT Collider::Initialize(void* argument)
{
    Bounding::BOUNDING_DESC* pBoundingDesc = static_cast<Bounding::BOUNDING_DESC*>(argument);

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
    CameraHelper* pCameraHelper = GET_INSTANCE(CameraHelper);

   _pEffect->SetWorld(::XMMatrixIdentity());
   _pEffect->SetView(pCameraHelper->GetTransformCalculator(CameraHelper::TRANSFORMSTATE::D3DTS_VIEW));
   _pEffect->SetProjection(pCameraHelper->GetTransformCalculator(CameraHelper::TRANSFORMSTATE::D3DTS_PROJ));

   RELEASE_INSTANCE(CameraHelper);

   _pEffect->Apply(_deviceContext);

   _pBatch->Begin();

   _pBounding->Render(_pBatch);

   _pBatch->End();

    return S_OK;
}

#endif // _DEBUG

_bool Collider::Intersects(Ray& ray, OUT _float& distance)
{
    return _bool();
}

_bool Collider::Intersects(Collider* other)
{
    return _bool();
}

void Collider::OnCollisionEnter(Collider* other)
{
}

void Collider::OnCollisionStay(Collider* other)
{
}

void Collider::OnCollisionExit(Collider* other)
{
}

Collider* Collider::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, COLLIDER_TYPE eType)
{
    Collider* pInstance = new Collider(device, deviceContext);

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
    if (false == _isCloned)
    {
        Safe_Delete<DirectX::PrimitiveBatch<VertexPositionColor>*>(_pBatch);
        Safe_Delete<BasicEffect*>(_pEffect);
    }
#endif // _DEBUG

    Safe_Release<Bounding*>(_pBounding);

}

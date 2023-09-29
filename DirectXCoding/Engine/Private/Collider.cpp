#include "pch.h"
#include "Collider.h"

Collider::Collider(ID3D11Device* device, ID3D11DeviceContext* deviceContext, COLLIDER_TYPE type)
    : Component(device, deviceContext, COMPONENT_TYPE::COLLIDER), _colliderType(type)
{

}

Collider::Collider(const Collider& rhs)
    : Component(rhs), _colliderType(rhs._colliderType)
{
}

HRESULT Collider::InitializePrototype()
{
    return S_OK;
}

HRESULT Collider::Initialize(void* argument)
{
    return S_OK;
}

HRESULT Collider::Tick(const _float& timeDelta)
{
    return S_OK;
}

HRESULT Collider::Render()
{
    return S_OK;
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

void Collider::Free()
{
    __super::Free();
}

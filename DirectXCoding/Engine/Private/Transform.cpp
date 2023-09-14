#include "pch.h"
#include "Transform.h"

Transform::Transform(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
    : Component(device, deviceContext)
{
}

Transform::Transform(const Transform& rhs)
    : Component(rhs)
{
}

void Transform::SetState(STATE state, FXMVECTOR vectorState)
{
    XMMATRIX stateMatrix;

    stateMatrix = ::XMLoadFloat4x4(&_worldMatrix);

    stateMatrix.r[static_cast<uint32>(state)] = vectorState;

    ::XMStoreFloat4x4(&_worldMatrix, stateMatrix);
}

HRESULT Transform::InitializePrototype()
{
    ::XMStoreFloat4x4(&_worldMatrix, ::XMMatrixIdentity());

    return S_OK;
}

HRESULT Transform::Initialize(void* argument)
{
    if (nullptr != argument)
    {
        TRANSFORM_DESC desc;

        ::memmove(&desc, argument, sizeof(desc));

        _transformDesc.speedPerSec = desc.speedPerSec;
        _transformDesc.rotationRadianPerSec = desc.rotationRadianPerSec;
    }

    return S_OK;
}

void Transform::Forward(const _float& timeDelta)
{
    XMVECTOR look = GetState(STATE::LOOK);

    XMVECTOR position = GetState(STATE::POSITION);

    position += ::XMVector3Normalize(look) * _transformDesc.speedPerSec * timeDelta;

    SetState(STATE::POSITION, position);
}

void Transform::Backward(const _float& timeDelta)
{
    XMVECTOR look = GetState(STATE::LOOK);
    
    XMVECTOR position = GetState(STATE::POSITION);

    position -= ::XMVector3Normalize(look) * _transformDesc.speedPerSec * timeDelta;

    SetState(STATE::POSITION, position);
}

void Transform::Left(const _float& timeDelta)
{
    XMVECTOR right = GetState(STATE::RIGHT);

    XMVECTOR position = GetState(STATE::POSITION);

    position -= ::XMVector3Normalize(right) * _transformDesc.speedPerSec * timeDelta;

    SetState(STATE::RIGHT, position);
}

void Transform::Right(const _float& timeDelta)
{
    XMVECTOR right = GetState(STATE::RIGHT);
    
    XMVECTOR position = GetState(STATE::POSITION);

    position += ::XMVector3Normalize(right) * _transformDesc.speedPerSec * timeDelta;

    SetState(STATE::RIGHT, position);
}

void Transform::FixRotation(XMVECTOR axis, const _float radian)
{
     
}

void Transform::Turn(XMVECTOR axis, const _float& timeDelta)
{

}

Transform* Transform::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    Transform* transform = new Transform(device, deviceContext);

    if (FAILED(transform->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : Transform");
        Safe_Release<Transform*>(transform);
        return nullptr;
    }

    return transform;
}

Component* Transform::Clone(void* argument)
{
    Transform* transform = new Transform(*this);

    if (FAILED(transform->Initialize(argument)))
    {
        MSG_BOX("Failed to Cloned : Transform");
        Safe_Release<Transform*>(transform);
        return nullptr;
    }

    return transform;
}

void Transform::Free()
{
    __super::Free();
}

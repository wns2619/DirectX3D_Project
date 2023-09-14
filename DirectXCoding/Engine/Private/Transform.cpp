#include "pch.h"
#include "Transform.h"
#include "Shader.h"

Transform::Transform(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
    : Component(device, deviceContext)
{
}

Transform::Transform(const Transform& rhs)
    : Component(rhs)
{
}

Vec3 Transform::GetScaled()
{
    return Vec3(::XMVectorGetX(XMVector3Length(GetState(STATE::RIGHT))),
        ::XMVectorGetX(XMVector3Length(GetState(STATE::UP))),
        ::XMVectorGetX(XMVector3Length(GetState(STATE::LOOK))));
}

void Transform::SetState(STATE state, FXMVECTOR vectorState)
{
    XMMATRIX stateMatrix;

    stateMatrix = ::XMLoadFloat4x4(&_worldMatrix);

    stateMatrix.r[static_cast<uint32>(state)] = vectorState;

    ::XMStoreFloat4x4(&_worldMatrix, stateMatrix);
}

void Transform::SetScaling(const Vec3& vectorScale)
{
    XMVECTOR right = GetState(STATE::RIGHT);
    XMVECTOR up = GetState(STATE::UP);
    XMVECTOR look = GetState(STATE::LOOK);

    SetState(STATE::RIGHT, ::XMVector3Normalize(right) * vectorScale.x);
    SetState(STATE::UP, ::XMVector3Normalize(up) * vectorScale.y);
    SetState(STATE::LOOK, ::XMVector3Normalize(look) * vectorScale.z);
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

HRESULT Transform::BindShaderResources(Shader* shader, const _char* constantName)
{
    return shader->BindMatrix(constantName, &_worldMatrix);
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
    Vec3 scale = GetScaled();

    XMVECTOR right = ::XMVectorSet(1.f, 0.f, 0.f, 0.f) * scale.x;
    XMVECTOR up = ::XMVectorSet(0.f, 1.f, 0.f, 0.f) * scale.y;
    XMVECTOR look = ::XMVectorSet(0.f, 0.f, 1.f, 0.f) * scale.z;

    Matrix rotationMatrix = ::XMMatrixRotationAxis(axis, radian);

    right = ::XMVector4Transform(right, rotationMatrix);
    up = ::XMVector4Transform(up, rotationMatrix);
    look = ::XMVector4Transform(look, rotationMatrix);

    SetState(STATE::RIGHT, right);
    SetState(STATE::UP, up);
    SetState(STATE::LOOK, look);

}

void Transform::Turn(XMVECTOR axis, const _float& timeDelta)
{
    XMVECTOR right = GetState(STATE::RIGHT);
    XMVECTOR up = GetState(STATE::UP);
    XMVECTOR look = GetState(STATE::LOOK);

    Matrix rotationMatrix = ::XMMatrixRotationAxis(axis, _transformDesc.rotationRadianPerSec * timeDelta);

    right = ::XMVector4Transform(right, rotationMatrix);
    up = ::XMVector4Transform(up, rotationMatrix);
    look = ::XMVector4Transform(look, rotationMatrix);

    SetState(STATE::RIGHT, right);
    SetState(STATE::UP, up);
    SetState(STATE::LOOK, look);
}

void Transform::LookAt(FXMVECTOR point)
{
    Vec3 scaled = GetScaled();

    XMVECTOR position = GetState(STATE::POSITION);
    XMVECTOR look = ::XMVector3Normalize(point - position) * scaled.z;
    XMVECTOR right = ::XMVector3Normalize(::XMVector3Cross(::XMVectorSet(0.f, 1.f, 0.f, 0.f), look)) * scaled.x;
    XMVECTOR up = ::XMVector3Normalize(::XMVector3Cross(look, right)) * scaled.y;

    SetState(STATE::RIGHT, right);
    SetState(STATE::UP, up);
    SetState(STATE::LOOK, look);
}

void Transform::Chase(FXMVECTOR point, _float const& timeDelta, _float distance)
{
    XMVECTOR position = GetState(STATE::POSITION);

    XMVECTOR direction = point - position;

    if (::XMVectorGetX(::XMVector3Length(direction)) > distance)
        position += ::XMVector3Normalize(direction) * _transformDesc.speedPerSec * timeDelta;

    SetState(STATE::POSITION, position);
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

#include "pch.h"
#include "Transform.h"
#include "Shader.h"
#include "Navigation.h"

Transform::Transform(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
    : Component(device, deviceContext, COMPONENT_TYPE::TRANSFORM), _worldMatrix(::XMMatrixIdentity())
{
    ::ZeroMemory(&_transformDesc, sizeof(_transformDesc));
}

Transform::Transform(const Transform& rhs)
    : Component(rhs), _worldMatrix(rhs._worldMatrix), _transformDesc(rhs._transformDesc)
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

void Transform::Forward(const _float& timeDelta, class Navigation* pNavigation)
{
    XMVECTOR look = GetState(STATE::LOOK);

    XMVECTOR position = GetState(STATE::POSITION);

    position += ::XMVector3Normalize(look) * _transformDesc.speedPerSec * timeDelta;

    // True 네비게이션에게 진짜 움직여도 되는 지 묻는다.

    if(nullptr == pNavigation || true == pNavigation->IsMove(position))
        SetState(STATE::POSITION, position);
}

void Transform::Backward(const _float& timeDelta, class Navigation* pNavigation)
{
    XMVECTOR look = GetState(STATE::LOOK);
    
    XMVECTOR position = GetState(STATE::POSITION);

    position -= ::XMVector3Normalize(look) * _transformDesc.speedPerSec * timeDelta;

    if (nullptr == pNavigation || true == pNavigation->IsMove(position))
        SetState(STATE::POSITION, position);
}

void Transform::Left(const _float& timeDelta, class Navigation* pNavigation)
{
    XMVECTOR right = GetState(STATE::RIGHT);

    XMVECTOR position = GetState(STATE::POSITION);

    position -= ::XMVector3Normalize(right) * _transformDesc.speedPerSec * timeDelta;


    if (nullptr == pNavigation || true == pNavigation->IsMove(position))
        SetState(STATE::POSITION, position);
}

void Transform::Right(const _float& timeDelta, class Navigation* pNavigation)
{
    XMVECTOR right = GetState(STATE::RIGHT);
    
    XMVECTOR position = GetState(STATE::POSITION);

    position += ::XMVector3Normalize(right) * _transformDesc.speedPerSec * timeDelta;

    if (nullptr == pNavigation || true == pNavigation->IsMove(position))
        SetState(STATE::POSITION, position);
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

void Transform::FixRotation(_float x, _float y, _float z)
{
    Vec3 scale = GetScaled();


    XMVECTOR right = ::XMVectorSet(1.f, 0.f, 0.f, 0.f) * scale.x;
    XMVECTOR up = ::XMVectorSet(0.f, 1.f, 0.f, 0.f) * scale.y;
    XMVECTOR look = ::XMVectorSet(0.f, 0.f, 1.f, 0.f) * scale.z;

    Matrix rotationMatrix = ::XMMatrixIdentity();

    _rotation.m128_f32[0] = x;
    _rotation.m128_f32[1] = y;
    _rotation.m128_f32[2] = z;

    rotationMatrix *= ::XMMatrixRotationX(::XMConvertToRadians(_rotation.m128_f32[0]));
    rotationMatrix *= ::XMMatrixRotationY(::XMConvertToRadians(_rotation.m128_f32[1]));
    rotationMatrix *= ::XMMatrixRotationZ(::XMConvertToRadians(_rotation.m128_f32[2]));

    right = ::XMVector4Transform(right, rotationMatrix);
    up = ::XMVector4Transform(up, rotationMatrix);
    look = ::XMVector4Transform(look, rotationMatrix);

    SetState(STATE::RIGHT, right);
    SetState(STATE::UP, up);
    SetState(STATE::LOOK, look);
}

void Transform::Turn(XMVECTOR axis, const _float& timeDelta)
{

    //Vec4		vRight = GetState(STATE::RIGHT);
    //Vec4		vUp = GetState(STATE::UP);
    //Vec4		vLook = GetState(STATE::LOOK);

    //Matrix		RotationMatrix = XMMatrixRotationAxis(axis, _transformDesc.rotationRadianPerSec * timeDelta);
    //Vec4		vQuaternionData = XMQuaternionRotationMatrix(RotationMatrix);
    //Matrix		QuaternionMatrix = XMMatrixRotationQuaternion(vQuaternionData);

    //vRight = XMVector4Transform(vRight, QuaternionMatrix);
    //vUp = XMVector4Transform(vUp, QuaternionMatrix);
    //vLook = XMVector4Transform(vLook, QuaternionMatrix);

    //SetState(STATE::RIGHT, vRight);
    //SetState(STATE::UP, vUp);
    //SetState(STATE::LOOK, vLook);
    // 
    XMVECTOR right = GetState(STATE::RIGHT);
    XMVECTOR up = GetState(STATE::UP);
    XMVECTOR look = GetState(STATE::LOOK);

    XMVECTOR rotationMatrix = ::XMQuaternionRotationAxis(axis, _transformDesc.rotationRadianPerSec * timeDelta);
    XMVECTOR rotationQuaternion = ::XMQuaternionIdentity();

    rotationQuaternion = ::XMQuaternionMultiply(rotationQuaternion, rotationMatrix);

    right = ::XMVector3Rotate(right, rotationQuaternion);
    up = ::XMVector3Rotate(up, rotationQuaternion);
    look = ::XMVector3Rotate(look, rotationQuaternion);

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

void Transform::Move(XMVECTOR moveVector)
{
    _position = ::XMVectorAdd(moveVector, _position);
}

void Transform::Move(Direction direction)
{
    if (direction == Direction::FORWARD)
        _position = ::XMVectorAdd(_directionalVectors.forward * speed, _position);
    else if (direction == Direction::BACKWARD)
        _position = ::XMVectorAdd(_directionalVectors.backward * speed, _position);
    else if (direction == Direction::LEFT)
        _position = ::XMVectorAdd(_directionalVectors.left * speed, _position);
    else if (direction == Direction::RIGHT)
        _position = ::XMVectorAdd(_directionalVectors.right * speed, _position);
}

void Transform::UpdateDirVectors()
{
    XMMATRIX rotationMatrix = ::XMMatrixRotationY(::XMVectorGetY(_rotation));
    _directionalVectors.forward = ::XMVector4Normalize(::XMVector3TransformCoord(::XMVectorSet(0.f, 0.f, 1.f, 0.f), rotationMatrix));
    _directionalVectors.left = ::XMVector4Normalize(::XMVector3TransformCoord(::XMVectorSet(-1.f, 0.f, 0.f, 0.f), rotationMatrix));
    _directionalVectors.right = ::XMVector4Normalize(::XMVector3TransformCoord(::XMVectorSet(1.f, 0.f, 1.f, 0.f), rotationMatrix));
    _directionalVectors.backward = ::XMVector4Normalize(::XMVector3TransformCoord(::XMVectorSet(0.f, 0.f, -1.f, 0.f), rotationMatrix));
}

Vec3 Transform::CustomGetPositionV3() const
{
    Vec3 positionV3;
    ::XMStoreFloat3(&positionV3, _position);

    return positionV3;
}

XMMATRIX Transform::CustomGetWorldMatrix() const
{
    return XMMATRIX(
        ::XMMatrixScalingFromVector(_scale) *
        ::XMMatrixRotationRollPitchYawFromVector(_localrotation) *
        ::XMMatrixRotationRollPitchYawFromVector(_rotation) *
        ::XMMatrixTranslationFromVector(_position)
    );
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

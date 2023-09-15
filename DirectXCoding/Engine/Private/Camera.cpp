#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "CameraHelper.h"

Camera::Camera(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: GameObject(device, deviceContext)
{
}

Camera::Camera(const Camera& rhs)
	: GameObject(rhs),
	_viewMatrix(rhs._viewMatrix),
	_projMatrix(rhs._projMatrix),
	_cameraDesc(rhs._cameraDesc)
{
}

HRESULT Camera::InitializePrototype()
{
	return S_OK;
}

HRESULT Camera::Initialize(void* argument)
{
	if (nullptr != argument)
	{
		CAMERA_DESC desc;

		::memmove(&desc, argument, sizeof(desc));

		_cameraDesc._aspect = desc._aspect;
		_cameraDesc._far	= desc._far;
		_cameraDesc._fov	= desc._fov;
		_cameraDesc._near	= desc._near;
	}

	CameraHelper::GetInstance()->SetViewProjMatrix(&_viewMatrix, &_projMatrix, *_transform, _cameraDesc);

	//::XMStoreFloat4x4(&_viewMatrix, ::XMMatrixLookAtLH(_transform->GetState(Transform::STATE::POSITION),
	//	_transform->GetState(Transform::STATE::LOOK), _transform->GetState(Transform::STATE::UP)));

	//::XMStoreFloat4x4(&_projMatrix,
	//	::XMMatrixPerspectiveFovLH(_cameraDesc._fov, _cameraDesc._aspect, _cameraDesc._near, _cameraDesc._far));

	return S_OK;
}

void Camera::Tick(const _float& timeDelta)
{



	//::XMStoreFloat4x4(&_viewMatrix, ::XMMatrixLookAtLH(_transform->GetState(Transform::STATE::POSITION),
	//	_transform->GetState(Transform::STATE::LOOK), _transform->GetState(Transform::STATE::UP)));

	//::XMStoreFloat4x4(&_projMatrix,
	//	::XMMatrixPerspectiveFovLH(_cameraDesc._fov, _cameraDesc._aspect, _cameraDesc._near, _cameraDesc._far));
}

void Camera::LateTick(const _float& timeDelta)
{
	CameraHelper::GetInstance()->SetViewProjMatrix(&_viewMatrix, &_projMatrix, *_transform, _cameraDesc);
}

void Camera::Free()
{
	__super::Free();
	
	Safe_Release<Transform*>(_transform);
}

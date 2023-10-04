#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "CameraHelper.h"

Camera::Camera(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: GameObject(device, deviceContext, OBJECT_TYPE::CAMERA)
	, _cameraHelper(CameraHelper::GetInstance())
{
	::ZeroMemory(&_cameraDesc, sizeof(Camera::CAMERA_DESC));
	Safe_AddRef<CameraHelper*>(_cameraHelper);
}

Camera::Camera(const Camera& rhs)
	: GameObject(rhs),
	_cameraDesc(rhs._cameraDesc)
	, _cameraHelper(rhs._cameraHelper)
{
	Safe_AddRef<CameraHelper*>(_cameraHelper);
}

HRESULT Camera::InitializePrototype()
{
	return S_OK;
}

HRESULT Camera::Initialize(void* argument)
{
	if (nullptr != argument)
	{
		CAMERA_DESC* desc = static_cast<CAMERA_DESC*>(argument);
		_cameraDesc._eye = desc->_eye;
		_cameraDesc._at = desc->_at;
		_cameraDesc._fovy = desc->_fovy;
		_cameraDesc._aspect = desc->_aspect;
		_cameraDesc._near = desc->_near;
		_cameraDesc._far = desc->_far;
	}

	_transform = Transform::Create(_device, _deviceContext);
	if (nullptr == _transform)
		return E_FAIL;
	
	if (FAILED(_transform->Initialize(argument)))
		return E_FAIL;

	_transform->SetState(Transform::STATE::POSITION, ::XMLoadFloat4(&_cameraDesc._eye));
	_transform->LookAt(::XMLoadFloat4(&_cameraDesc._at));

	return S_OK;
}

void Camera::Tick(const _float& timeDelta)
{
	if (nullptr == _cameraHelper)
		return;

	XMMATRIX world = _transform->CustomGetWorldMatrix();
	XMMATRIX worldInv = ::XMMatrixInverse(nullptr, world);

	_cameraHelper->SetTransform(CameraHelper::TRANSFORMSTATE::D3DTS_VIEW, worldInv);
	_cameraHelper->SetTransform(CameraHelper::TRANSFORMSTATE::D3DTS_PROJ,
		::XMMatrixPerspectiveFovLH(_cameraDesc._fovy, _cameraDesc._aspect, _cameraDesc._near, _cameraDesc._far));
}

void Camera::LateTick(const _float& timeDelta)
{

}

void Camera::Free()
{
	__super::Free();
	
	Safe_Release<Transform*>(_transform);
	Safe_Release<CameraHelper*>(_cameraHelper);
}

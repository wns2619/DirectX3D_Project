#include "pch.h"
#include "CameraHelper.h"
#include "Transform.h"

IMPLEMENT_SINGLETON(CameraHelper)

CameraHelper::CameraHelper()
{
}

HRESULT CameraHelper::SetViewProjMatrix(Matrix* viewMatrix, Matrix* projMatrix, Transform& transform, Camera::CAMERA_DESC desc)
{
	if (viewMatrix == nullptr)
		return E_FAIL;

	if (projMatrix == nullptr)
		return E_FAIL;
	
	::XMStoreFloat4x4(viewMatrix, ::XMMatrixLookAtLH(transform.GetState(Transform::STATE::POSITION),
		transform.GetState(Transform::STATE::LOOK), transform.GetState(Transform::STATE::UP)));

	::XMStoreFloat4x4(projMatrix,
		::XMMatrixPerspectiveFovLH(desc._fov, desc._aspect, desc._near, desc._far));

	::XMStoreFloat4x4(&_viewMatrix, *viewMatrix);
	::XMStoreFloat4x4(&_projMatrix, *projMatrix);
}

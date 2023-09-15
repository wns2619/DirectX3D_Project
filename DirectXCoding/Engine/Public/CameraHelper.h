#pragma once

#include "Defines.h"
#include "Camera.h"

BEGIN(Engine)

class Transform;

class CameraHelper : public Base
{
	DECLARE_SINGLETON(CameraHelper)

private:
	CameraHelper();
	virtual ~CameraHelper() = default;

public:
	HRESULT SetViewProjMatrix(Matrix* viewMatrix, Matrix* projMatrix, Transform& transform, Camera::CAMERA_DESC desc);

	Matrix* GetViewMatrix() { return &_viewMatrix; }
	Matrix* GetProjMatrix() { return &_projMatrix; }
private:
	Matrix _viewMatrix = ::XMMatrixIdentity(), _projMatrix = ::XMMatrixIdentity();
};

END


#include "pch.h"
#include "CameraHelper.h"
#include "Shader.h"

IMPLEMENT_SINGLETON(CameraHelper)

CameraHelper::CameraHelper()
{
	for (size_t i = 0; i < static_cast<uint32>(TRANSFORMSTATE::D3DTS_END); ++i)
	{
		::XMStoreFloat4x4(&_transformMatrices[i], ::XMMatrixIdentity());
		::XMStoreFloat4x4(&_transformInverseMatrices[i], ::XMMatrixIdentity());
	}
}

void CameraHelper::SetTransform(TRANSFORMSTATE state, FXMMATRIX trasnsformMatrix)
{
	if (state >= TRANSFORMSTATE::D3DTS_END || TRANSFORMSTATE::D3DTS_VIEW > state)
		return;

	::XMStoreFloat4x4(&_transformMatrices[static_cast<uint32>(state)], trasnsformMatrix);
}

Matrix CameraHelper::GetTransform(TRANSFORMSTATE state) const
{
	if (state >= TRANSFORMSTATE::D3DTS_END || TRANSFORMSTATE::D3DTS_VIEW > state)
		return Matrix();

	return _transformMatrices[static_cast<uint32>(state)];
}

FXMMATRIX CameraHelper::GetTransformCalculator(TRANSFORMSTATE state) const
{
	if (state >= TRANSFORMSTATE::D3DTS_END || TRANSFORMSTATE::D3DTS_VIEW > state)
		return ::XMMatrixIdentity();

	return ::XMLoadFloat4x4(&_transformMatrices[static_cast<uint32>(state)]);
}

Matrix CameraHelper::GetInverseTransform(TRANSFORMSTATE state) const
{
	if (state >= TRANSFORMSTATE::D3DTS_END || TRANSFORMSTATE::D3DTS_VIEW > state)
		return Matrix();

	return _transformInverseMatrices[static_cast<uint32>(state)];
}

FXMMATRIX CameraHelper::GetInverseTransformCalculator(TRANSFORMSTATE state) const
{
	if (state >= TRANSFORMSTATE::D3DTS_END || TRANSFORMSTATE::D3DTS_VIEW > state)
		return ::XMMatrixIdentity();

	return ::XMLoadFloat4x4(&_transformInverseMatrices[static_cast<uint32>(state)]);
}

Vec4 CameraHelper::GetCameraPosition(TRANSFORMSTATE state) const
{
	if (state >= TRANSFORMSTATE::D3DTS_END || TRANSFORMSTATE::D3DTS_VIEW > state)
		return Vec4();

	return _camPosition;
}

XMVECTOR CameraHelper::GetCameraCaculator(TRANSFORMSTATE state) const
{
	return ::XMLoadFloat4(&_camPosition);
}

HRESULT CameraHelper::BindTransformToShader(Shader* shader, const char* constantName, CameraHelper::TRANSFORMSTATE state)
{
	return shader->BindMatrix(constantName, &_transformMatrices[static_cast<uint32>(state)]);
}

void CameraHelper::Tick()
{
	for (size_t i = 0; i < static_cast<uint32>(TRANSFORMSTATE::D3DTS_END); i++)
		::XMStoreFloat4x4(&_transformInverseMatrices[i], ::XMMatrixInverse(nullptr, ::XMLoadFloat4x4(&_transformMatrices[i])));

	::memmove(&_camPosition, &_transformInverseMatrices[static_cast<uint32>(TRANSFORMSTATE::D3DTS_VIEW)].m[3], sizeof(Vec4));

}

void CameraHelper::Free()
{
	__super::Free();
}


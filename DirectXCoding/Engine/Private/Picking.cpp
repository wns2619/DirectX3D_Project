#include "pch.h"
#include "Picking.h"
#include "GameInstance.h"

Picking::Picking(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Component(device, deviceContext)
{
}

Picking::Picking(const Picking& rhs)
	: Component(rhs)
{
}

void Picking::Pick(int32 screenX, int32 screenY, _float winsizeX, _float winSizeY)
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	Matrix projectionMatrix = gameInstance->GetTransformCalculator(CameraHelper::TRANSFORMSTATE::D3DTS_PROJ);
	
	// ����Ʈ���� -> ���� 
	_float viewX = (+2.0f * screenX / winsizeX - 1.f) / projectionMatrix(0, 0);
	_float viewY = (-2.0f * screenY / winSizeY + 1.f) / projectionMatrix(1, 1);
	
	// ������ǥ���� -> �� �����̽� 
	Matrix viewMatrix = gameInstance->GetTransformCalculator(CameraHelper::TRANSFORMSTATE::D3DTS_VIEW);
	Matrix viewMatrixInv = viewMatrix.Invert();

	// view space���� Ray����
	Vec4 rayOrigin, rayDir;
	rayOrigin = Vec4(0.f, 0.f, 0.f, 1.f);
	rayDir = Vec4(viewX, viewY, 1.f, 0.f);

	// world space���� Ray �ٽ� ����
	Vec3 WorldRayOrigin = ::XMVector3TransformCoord(rayOrigin, viewMatrixInv);
	Vec3 worldRayDir = ::XMVector3TransformNormal(rayDir, viewMatrixInv);
	worldRayDir.Normalize();

	// WorldSpace���� ����
	Ray ray = Ray(WorldRayOrigin, worldRayDir);
	



	RELEASE_INSTANCE(GameInstance);
}

Picking* Picking::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	return nullptr;
}

Component* Picking::Clone(void* argument)
{
	return nullptr;
}

void Picking::Free()
{
}

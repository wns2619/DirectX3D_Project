#include "pch.h"
#include "Picking.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(Picking)

Picking::Picking()
{
}

_bool Picking::PickObject(POINT pt)
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	Matrix projectionMatrix = gameInstance->GetTransformCalculator(CameraHelper::TRANSFORMSTATE::D3DTS_PROJ);
	
	Viewport& vp = gameInstance->GetViewPort();

	// 뷰포트에서 -> 투영 
	_float viewX = (+2.0f * pt.x / vp.GetWidth() - 1.f) / projectionMatrix(0, 0);
	_float viewY = (-2.0f * pt.y / vp.GetHeight() + 1.f) / projectionMatrix(1, 1);
	
	// 투영좌표에서 -> 뷰 스페이스 
	Matrix viewMatrix = gameInstance->GetTransformCalculator(CameraHelper::TRANSFORMSTATE::D3DTS_VIEW);
	Matrix viewMatrixInv = viewMatrix.Invert();

	// view space에서 Ray정의
	Vec4 rayOrigin, rayDir;
	rayOrigin = Vec4(0.f, 0.f, 0.f, 1.f);
	rayDir = Vec4(viewX, viewY, 1.f, 0.f);

	// world space에서 Ray 다시 정의
	Vec3 WorldRayOrigin = ::XMVector3TransformCoord(rayOrigin, viewMatrixInv);
	Vec3 worldRayDir = ::XMVector3TransformNormal(rayDir, viewMatrixInv);
	worldRayDir.Normalize();

	// WorldSpace에서 연산
	Ray ray = Ray(WorldRayOrigin, worldRayDir);
	

	RELEASE_INSTANCE(GameInstance);


	return false;
}

_bool Picking::TerrainPicking(POINT pt, Vec3& pickPos, _float& distance, Transform* trans, VIBufferTerrain* buffer)
{
	Matrix W = trans->GetWorldMatrix();
	Matrix V = CameraHelper::GetInstance()->GetTransform(CameraHelper::TRANSFORMSTATE::D3DTS_VIEW);
	Matrix P = CameraHelper::GetInstance()->GetTransform(CameraHelper::TRANSFORMSTATE::D3DTS_PROJ);

	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	SimpleMath::Viewport vp(RECT{ 0,0,1280,720 });

	Vec3 Mousepos = Vec3(pt.x, pt.y, 0.f);

	Vec3 worldMouse = vp.Unproject(Mousepos, P, V, W);
	

	SimpleMath::Ray ray;
	ray.position = static_cast<Vec3>(gameInstance->GetCameraPosition());
	ray.direction = worldMouse - ray.position;
	ray.direction.Normalize();

	ray.position = XMVector3TransformCoord(ray.position, trans->GetInverseWorldMatrix()); // 요건 월드항등상태라 불필요한데 혹시 모르니 한거
	ray.direction = XMVector3TransformNormal(ray.direction, trans->GetInverseWorldMatrix());
	ray.direction.Normalize();


	SimpleMath::Vector3* pBufferPos = buffer->GetVerticesPosition();
	VIBufferTerrain::TERRAIN_DESC tTerrainDesc = buffer->GetTerranDesc();

	_ulong		dwVtxIdx[3]{};

	//_float	fDist = 0.f;

	for (_ulong i = 0; i < tTerrainDesc.numVerticesZ - 1; ++i)
	{
		for (_ulong j = 0; j < tTerrainDesc.numVerticesX - 1; ++j)
		{
			_ulong	dwIndex = i * tTerrainDesc.numVerticesX + j;

			// 오른쪽 위
			dwVtxIdx[0] = dwIndex + tTerrainDesc.numVerticesX;
			dwVtxIdx[1] = dwIndex + tTerrainDesc.numVerticesX + 1;
			dwVtxIdx[2] = dwIndex + 1;

			if (true == ray.Intersects(pBufferPos[dwVtxIdx[1]], pBufferPos[dwVtxIdx[0]], pBufferPos[dwVtxIdx[2]], distance))
			{
				RELEASE_INSTANCE(GameInstance);

				pickPos = ray.position + ray.direction * distance;

				return true;
			}

			// 왼쪽 아래
			dwVtxIdx[0] = dwIndex + tTerrainDesc.numVerticesX;
			dwVtxIdx[1] = dwIndex + 1;
			dwVtxIdx[2] = dwIndex;

			if (true == ray.Intersects(pBufferPos[dwVtxIdx[1]], pBufferPos[dwVtxIdx[0]], pBufferPos[dwVtxIdx[2]], distance))
			{
				RELEASE_INSTANCE(GameInstance);

				pickPos = ray.position + ray.direction * distance;

				return true;
			}
		}
	}

	RELEASE_INSTANCE(GameInstance);
	return false;
}

void Picking::Free()
{
	__super::Free();
}

#include "pch.h"
#include "Cell.h"
#include "VIBufferCell.h"

Cell::Cell(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: _device(device), _deviceContext(deviceContext)
{
	Safe_AddRef<ID3D11Device*>(_device);
	Safe_AddRef<ID3D11DeviceContext*>(_deviceContext);
}

HRESULT Cell::Initialize(const Vec3* pPoints, uint32 iIndex)
{
	_iIndex = iIndex;

	memcpy(_vPoints_Original, pPoints, sizeof(Vec3) * POINT_END);

	Vec3		vLines[LINE_END];

	XMStoreFloat3(&vLines[LINE_AB], XMLoadFloat3(&_vPoints_Original[POINT_B]) - XMLoadFloat3(&_vPoints_Original[POINT_A]));
	XMStoreFloat3(&vLines[LINE_BC], XMLoadFloat3(&_vPoints_Original[POINT_C]) - XMLoadFloat3(&_vPoints_Original[POINT_B]));
	XMStoreFloat3(&vLines[LINE_CA], XMLoadFloat3(&_vPoints_Original[POINT_A]) - XMLoadFloat3(&_vPoints_Original[POINT_C]));

	for (size_t i = 0; i < LINE_END; i++)
	{
		_vNormals[i] = Vec3(vLines[i].z * -1.f, 0.f, vLines[i].x);

		XMStoreFloat3(&_vNormals[i], XMVector3Normalize(XMLoadFloat3(&_vNormals[i])));
	}

#ifdef _DEBUG
	_viBuffer = VIBufferCell::Create(_device, _deviceContext, _vPoints_Original);
	if (nullptr == _viBuffer)
		return E_FAIL;
#endif // _DEBUG


	return S_OK;
}

void Cell::Update(FXMMATRIX worldMatrix)
{
	for (uint32 i = 0; i < POINT_END; ++i)
		_vPoints_InWorld[i] = ::XMVector3TransformCoord(::XMLoadFloat3(&_vPoints_Original[i]), worldMatrix);

	// 정점의 로컬위치에서 월드행렬을 곱한 값을 업데이트마다 갱신해줌.
}

_bool Cell::ComparePoints(const Vec3* pSourcePoint, const Vec3* pDestPoint)
{
	// 같은 성분을 공유하고 있는 지 판단하는 함수.

	if (::XMVector3Equal(::XMLoadFloat3(&_vPoints_Original[POINT_A]), ::XMLoadFloat3(pSourcePoint)))
	{
		// A선분과 벡터의 위치가 같다면 AB 선분인지 AC 선분인지 판단.
		if(::XMVector3Equal(::XMLoadFloat3(&_vPoints_Original[POINT_B]), ::XMLoadFloat3(pDestPoint)))
			return true;
		if (::XMVector3Equal(::XMLoadFloat3(&_vPoints_Original[POINT_C]), ::XMLoadFloat3(pDestPoint)))
			return true;
	}

	if (::XMVector3Equal(::XMLoadFloat3(&_vPoints_Original[POINT_B]), ::XMLoadFloat3(pSourcePoint)))
	{
		// B선분과 벡터의 위치가 같다면 BA 선분인지 BC 선분인지 판단.
		if (::XMVector3Equal(::XMLoadFloat3(&_vPoints_Original[POINT_A]), ::XMLoadFloat3(pDestPoint)))
			return true;
		if (::XMVector3Equal(::XMLoadFloat3(&_vPoints_Original[POINT_C]), ::XMLoadFloat3(pDestPoint)))
			return true;
	}

	if (::XMVector3Equal(::XMLoadFloat3(&_vPoints_Original[POINT_C]), ::XMLoadFloat3(pSourcePoint)))
	{
		// C선분과 벡터의 위치가 같다면 CA 선분인지 CB 선분인지 판단.
		if (::XMVector3Equal(::XMLoadFloat3(&_vPoints_Original[POINT_A]), ::XMLoadFloat3(pDestPoint)))
			return true;
		if (::XMVector3Equal(::XMLoadFloat3(&_vPoints_Original[POINT_B]), ::XMLoadFloat3(pDestPoint)))
			return true;
	}


	// 비교 했을 때 아무 선분이랑도 같지않다면 이웃이 아니다.
	return false;
}

_bool Cell::IsOut(FXMVECTOR vPoint, FXMMATRIX worldMatrix, int32* pNeighborIndex)
{
	// 내가 현재 있는 셀에서 나갔는 지 판단.

	for (uint32 i = 0; i < LINE_END; ++i)
	{
		// 월드의 위치에서의 선분과 계산.
		Vec4 vSour = ::XMVector3Normalize(vPoint - ::XMLoadFloat3(&_vPoints_Original[i]));
		// 노말을 로컬에서 만들었으니 월드행렬을 곱해서 월드에서의 노멀로 바꾼다.
		Vec4 vDest = ::XMVector3Normalize(::XMVector3TransformNormal(::XMLoadFloat3(&_vNormals[i]), worldMatrix));

		// 벡터가 양수면 나간거고, 음수면 안 나간거다.
		if (0 < ::XMVectorGetX(::XMVector3Dot(vSour, vDest)))
		{
			*pNeighborIndex = _neightborIndices[i];
			// 현재 셀에서 빠져나갔다면 내가 이동한 셀의 위치를 업데이트 해주고 true 반환한다.
			return true;
		}
	}

	return false;
}

Vec3 Cell::IsSilde(XMVECTOR& vPoint, FXMVECTOR vLook, FXMMATRIX worldMatrix)
{

	for (uint32 i = 0; i < LINE_END; ++i)
	{
		Vec4 vSour = ::XMVector3Normalize(vPoint - ::XMLoadFloat3(&_vPoints_InWorld[i]));
		Vec4 vDest = ::XMVector3Normalize(::XMVector3TransformNormal(::XMLoadFloat3(&_vNormals[i]), worldMatrix));
		// 월드상의 노멀벡터.
		// 이 선분의 노멀과 내가 이동하는 방향의 벡터를 계산하면.

		if (0 < ::XMVectorGetX(::XMVector3Dot(vSour, vDest)))
		{
			return _vNormals[i];
		}
	}

	return Vec3();
}

#ifdef _DEBUG

HRESULT Cell::Render()
{
	_viBuffer->Render();

	return S_OK;
}

#endif // DEBUG


Cell* Cell::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const Vec3* pPoints, uint32 iIndex)
{
	Cell* pInstance = new Cell(device, deviceContext);

	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX("Failed to Create : Cell");
		Safe_Release<Cell*>(pInstance);
	}

	return pInstance;
}

void Cell::Free()
{
	__super::Free();

#if _DEBUG
	Safe_Release<VIBufferCell*>(_viBuffer);
#endif // _DEBUG


	Safe_Release<ID3D11Device*>(_device);
	Safe_Release<ID3D11DeviceContext*>(_deviceContext);
}

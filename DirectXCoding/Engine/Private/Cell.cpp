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
		_vPoints_InWorld[i] = ::XMVector2TransformCoord(::XMLoadFloat3(&_vPoints_Original[i]), worldMatrix);

	// 정점의 로컬위치에서 월드행렬을 곱한 값을 업데이트마다 갱신해줌.
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

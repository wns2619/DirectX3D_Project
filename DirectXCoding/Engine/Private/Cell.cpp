#include "pch.h"
#include "Cell.h"

Cell::Cell(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: _device(device), _deviceContext(deviceContext)
{
	Safe_AddRef<ID3D11Device*>(_device);
	Safe_AddRef<ID3D11DeviceContext*>(_deviceContext);
}

HRESULT Cell::Initialize(const Vec3* pPoints, uint32 iIndex)
{
	_iIndex = iIndex;

	memcpy(_vPoints, pPoints, sizeof(Vec3) * POINT_END);

	Vec3		vLines[LINE_END];

	XMStoreFloat3(&vLines[LINE_AB], XMLoadFloat3(&_vPoints[POINT_B]) - XMLoadFloat3(&_vPoints[POINT_A]));
	XMStoreFloat3(&vLines[LINE_BC], XMLoadFloat3(&_vPoints[POINT_C]) - XMLoadFloat3(&_vPoints[POINT_B]));
	XMStoreFloat3(&vLines[LINE_CA], XMLoadFloat3(&_vPoints[POINT_A]) - XMLoadFloat3(&_vPoints[POINT_C]));

	for (size_t i = 0; i < LINE_END; i++)
	{
		_vNormals[i] = Vec3(vLines[i].z * -1.f, 0.f, vLines[i].x);

		XMStoreFloat3(&_vNormals[i], XMVector3Normalize(XMLoadFloat3(&_vNormals[i])));
	}

	return S_OK;
}

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

	Safe_Release<ID3D11Device*>(_device);
	Safe_Release<ID3D11DeviceContext*>(_deviceContext);
}

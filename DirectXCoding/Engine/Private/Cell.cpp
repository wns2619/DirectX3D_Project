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

	// ������ ������ġ���� ��������� ���� ���� ������Ʈ���� ��������.
}

_bool Cell::ComparePoints(const Vec3* pSourcePoint, const Vec3* pDestPoint)
{
	// ���� ������ �����ϰ� �ִ� �� �Ǵ��ϴ� �Լ�.

	if (::XMVector3Equal(::XMLoadFloat3(&_vPoints_Original[POINT_A]), ::XMLoadFloat3(pSourcePoint)))
	{
		// A���а� ������ ��ġ�� ���ٸ� AB �������� AC �������� �Ǵ�.
		if(::XMVector3Equal(::XMLoadFloat3(&_vPoints_Original[POINT_B]), ::XMLoadFloat3(pDestPoint)))
			return true;
		if (::XMVector3Equal(::XMLoadFloat3(&_vPoints_Original[POINT_C]), ::XMLoadFloat3(pDestPoint)))
			return true;
	}

	if (::XMVector3Equal(::XMLoadFloat3(&_vPoints_Original[POINT_B]), ::XMLoadFloat3(pSourcePoint)))
	{
		// B���а� ������ ��ġ�� ���ٸ� BA �������� BC �������� �Ǵ�.
		if (::XMVector3Equal(::XMLoadFloat3(&_vPoints_Original[POINT_A]), ::XMLoadFloat3(pDestPoint)))
			return true;
		if (::XMVector3Equal(::XMLoadFloat3(&_vPoints_Original[POINT_C]), ::XMLoadFloat3(pDestPoint)))
			return true;
	}

	if (::XMVector3Equal(::XMLoadFloat3(&_vPoints_Original[POINT_C]), ::XMLoadFloat3(pSourcePoint)))
	{
		// C���а� ������ ��ġ�� ���ٸ� CA �������� CB �������� �Ǵ�.
		if (::XMVector3Equal(::XMLoadFloat3(&_vPoints_Original[POINT_A]), ::XMLoadFloat3(pDestPoint)))
			return true;
		if (::XMVector3Equal(::XMLoadFloat3(&_vPoints_Original[POINT_B]), ::XMLoadFloat3(pDestPoint)))
			return true;
	}


	// �� ���� �� �ƹ� �����̶��� �����ʴٸ� �̿��� �ƴϴ�.
	return false;
}

_bool Cell::IsOut(FXMVECTOR vPoint, FXMMATRIX worldMatrix, int32* pNeighborIndex)
{
	// ���� ���� �ִ� ������ ������ �� �Ǵ�.

	for (uint32 i = 0; i < LINE_END; ++i)
	{
		// ������ ��ġ������ ���а� ���.
		Vec4 vSour = ::XMVector3Normalize(vPoint - ::XMLoadFloat3(&_vPoints_Original[i]));
		// �븻�� ���ÿ��� ��������� ��������� ���ؼ� ���忡���� ��ַ� �ٲ۴�.
		Vec4 vDest = ::XMVector3Normalize(::XMVector3TransformNormal(::XMLoadFloat3(&_vNormals[i]), worldMatrix));

		// ���Ͱ� ����� �����Ű�, ������ �� �����Ŵ�.
		if (0 < ::XMVectorGetX(::XMVector3Dot(vSour, vDest)))
		{
			*pNeighborIndex = _neightborIndices[i];
			// ���� ������ ���������ٸ� ���� �̵��� ���� ��ġ�� ������Ʈ ���ְ� true ��ȯ�Ѵ�.
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
		// ������� ��ֺ���.
		// �� ������ ��ְ� ���� �̵��ϴ� ������ ���͸� ����ϸ�.

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

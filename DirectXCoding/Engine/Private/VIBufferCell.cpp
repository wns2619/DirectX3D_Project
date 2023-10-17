#include "pch.h"
#include "VIBufferCell.h"

VIBufferCell::VIBufferCell(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: VIBuffer(device, deviceContext)
{

}

VIBufferCell::VIBufferCell(const VIBufferCell& rhs)
	: VIBuffer(rhs)
{

}

HRESULT VIBufferCell::InitializePrototype(const Vec3* pPoints)
{
	_BufferDesc._stride = sizeof(VertexPos);
	_BufferDesc._numvertices = 3;
	_BufferDesc._indexStride = 2;
	_BufferDesc._numIndices = 4;
	_BufferDesc._indexFormat = _BufferDesc._indexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	_BufferDesc._topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
	_BufferDesc._numVBs = 1;

#pragma region vertexBuffer

	ZeroMemory(&_BufferDesc._bufferDesc, sizeof(D3D11_BUFFER_DESC));
	{
		_BufferDesc._bufferDesc.ByteWidth = _BufferDesc._stride * _BufferDesc._numvertices;
		_BufferDesc._bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		_BufferDesc._bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		_BufferDesc._bufferDesc.CPUAccessFlags = 0;
		_BufferDesc._bufferDesc.MiscFlags = 0;
		_BufferDesc._bufferDesc.StructureByteStride = _BufferDesc._stride;
	}

	VertexPos* vertices = new VertexPos[_BufferDesc._numvertices];
	ZeroMemory(vertices, sizeof(VertexPos) * _BufferDesc._numvertices);

	::memcpy(vertices, pPoints, sizeof(Vec3) * _BufferDesc._numvertices);

	ZeroMemory(&_BufferDesc._subResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	_BufferDesc._subResourceData.pSysMem = vertices;

	if (FAILED(__super::CreateBuffer(&_vertexBuffer)))
		return E_FAIL;

	Safe_Delete_Array<VertexPos*>(vertices);

#pragma endregion

#pragma region indexBuffer

	ZeroMemory(&_BufferDesc._bufferDesc, sizeof(D3D11_BUFFER_DESC));
	{
		_BufferDesc._bufferDesc.ByteWidth = _BufferDesc._indexStride * _BufferDesc._numIndices;
		_BufferDesc._bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		_BufferDesc._bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		_BufferDesc._bufferDesc.CPUAccessFlags = 0;
		_BufferDesc._bufferDesc.MiscFlags = 0;
		_BufferDesc._bufferDesc.StructureByteStride = 0;
	}

	_ushort* Indices = new _ushort[_BufferDesc._numIndices];
	ZeroMemory(Indices, sizeof(_ushort) * _BufferDesc._numIndices);

	Indices[0] = 0;
	Indices[1] = 1;
	Indices[2] = 2;
	Indices[3] = 0;

	ZeroMemory(&_BufferDesc._subResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	_BufferDesc._subResourceData.pSysMem = Indices;

	if (FAILED(__super::CreateBuffer(&_indexBuffer)))
		return E_FAIL;

	Safe_Delete_Array<_ushort*>(Indices);
#pragma endregion

	return S_OK;
}

HRESULT VIBufferCell::Initialize(void* pArg)
{
	return S_OK;
}

VIBufferCell* VIBufferCell::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const Vec3* pPoints)
{
	VIBufferCell* pInstance = new VIBufferCell(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype(pPoints)))
	{
		MSG_BOX("Failed to Created : VIBuffer Cell");
		Safe_Release<VIBufferCell*>(pInstance);
	}

	return pInstance;
}

Component* VIBufferCell::Clone(void* pArg)
{
	VIBufferCell* pInstance = new VIBufferCell(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Faild to Cloned : VIBuffer Cell");
		Safe_Release<VIBufferCell*>(pInstance);
	}

	return pInstance;
}

void VIBufferCell::Free()
{
	__super::Free();
}

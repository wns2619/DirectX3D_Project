#include "pch.h"
#include "VIBuffer_RectInstance.h"

VIBuffer_RectInstance::VIBuffer_RectInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: VIBufferInstancing(pDevice, pDeviceContext)
{
}

VIBuffer_RectInstance::VIBuffer_RectInstance(const VIBuffer_RectInstance& rhs)
	: VIBufferInstancing(rhs)
{
}

HRESULT VIBuffer_RectInstance::InitializePrototype(uint32 iNumInstance)
{
	_iNumInstance = iNumInstance;
	_iNumIndicesPerInstance = 6;

	_BufferDesc._stride = sizeof(VertexTextureData);
	_BufferDesc._numvertices = 4;

	_BufferDesc._indexStride = 2;
	_BufferDesc._numIndices = 6 * _iNumInstance;
	_BufferDesc._indexFormat = _BufferDesc._indexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	_BufferDesc._topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	_BufferDesc._numVBs = 2;

	::ZeroMemory(&_BufferDesc._bufferDesc, sizeof(_BufferDesc._bufferDesc));
	{
		_BufferDesc._bufferDesc.ByteWidth = _BufferDesc._stride * _BufferDesc._numvertices;
		_BufferDesc._bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		_BufferDesc._bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		_BufferDesc._bufferDesc.CPUAccessFlags = 0;
		_BufferDesc._bufferDesc.MiscFlags = 0;
		_BufferDesc._bufferDesc.StructureByteStride = _BufferDesc._stride;
	}

	VertexTextureData* pVertices = new VertexTextureData[_BufferDesc._numvertices];
	::ZeroMemory(pVertices, sizeof(VertexTextureData) * _BufferDesc._numvertices);

	pVertices[0].position = Vec3(-0.5f, 0.5f, 0.f);
	pVertices[0].uv = Vec2(0.f, 0.f);

	pVertices[1].position = Vec3(0.5f, 0.5f, 0.f);
	pVertices[1].uv = Vec2(1.f, 0.f);

	pVertices[2].position = Vec3(0.5, -0.5f, 0.f);
	pVertices[2].uv = Vec2(1.f, 1.f);

	pVertices[3].position = Vec3(-0.5f, -0.5f, 0.f);
	pVertices[3].uv = Vec2(0.f, 1.f);

	::ZeroMemory(&_BufferDesc._subResourceData, sizeof(_BufferDesc._subResourceData));
	_BufferDesc._subResourceData.pSysMem = pVertices;

	if (FAILED(__super::CreateBuffer(&_vertexBuffer)))
		return E_FAIL;

	Safe_Delete_Array<VertexTextureData*>(pVertices);

	::ZeroMemory(&_BufferDesc._bufferDesc, sizeof(_BufferDesc._bufferDesc));
	{
		_BufferDesc._bufferDesc.ByteWidth = _BufferDesc._indexStride * _BufferDesc._numIndices;
		_BufferDesc._bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		_BufferDesc._bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		_BufferDesc._bufferDesc.CPUAccessFlags = 0;
		_BufferDesc._bufferDesc.MiscFlags = 0;
		_BufferDesc._bufferDesc.StructureByteStride = 0;
	}

	_ushort* pIndices = new _ushort[_BufferDesc._numIndices];
	::ZeroMemory(pIndices, sizeof(_ushort) * _BufferDesc._numIndices);

	uint32 iNumIndices = 0;

	for (uint32 i = 0; i < _iNumInstance; ++i)
	{
		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 1;
		pIndices[iNumIndices++] = 2;

		pIndices[iNumIndices++] = 0;
		pIndices[iNumIndices++] = 2;
		pIndices[iNumIndices++] = 3;
	}

	::ZeroMemory(&_BufferDesc._subResourceData, sizeof(_BufferDesc._subResourceData));
	_BufferDesc._subResourceData.pSysMem = pIndices;

	if (FAILED(__super::CreateBuffer(&_indexBuffer)))
		return E_FAIL;

	Safe_Delete_Array<_ushort*>(pIndices);

	//if (FAILED(__super::InitializePrototype()))
	//	return E_FAIL;

	return S_OK;
}

HRESULT VIBuffer_RectInstance::Initialize(void* argument)
{
	return S_OK;
}

HRESULT VIBuffer_RectInstance::Update(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource = {};

	_deviceContext->Map(_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (uint32 i = 0; i < _iNumInstance; ++i)
		static_cast<VTXINSTANCE*>(SubResource.pData)[i].vTranslation.y -= 1.f * fTimeDelta;

	_deviceContext->Unmap(_pVBInstance, 0);

	return S_OK;
}

VIBuffer_RectInstance* VIBuffer_RectInstance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, uint32 iNumInstance)
{
	VIBuffer_RectInstance* pInstance = new VIBuffer_RectInstance(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype(iNumInstance)))
	{
		MSG_BOX("Failed to Created : VIBuffer Rect Instance");
		Safe_Release<VIBuffer_RectInstance*>(pInstance);
	}

	return pInstance;
}

Component* VIBuffer_RectInstance::Clone(void* pArg)
{
	VIBuffer_RectInstance* pInstance = new VIBuffer_RectInstance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : VIBuffer Rect Instance");
		Safe_Release<VIBuffer_RectInstance*>(pInstance);
	}

	return pInstance;
}

void VIBuffer_RectInstance::Free()
{
	__super::Free();
}

#include "pch.h"
#include "VIBuffer_PointInstance.h"

VIBuffer_PointInstance::VIBuffer_PointInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: VIBufferInstancing(pDevice, pDeviceContext)
{
}

VIBuffer_PointInstance::VIBuffer_PointInstance(const VIBuffer_PointInstance& rhs)
	: VIBufferInstancing(rhs)
{
}

HRESULT VIBuffer_PointInstance::InitializePrototype(const INSTANCE_DESC& InstanceDesc)
{
	_iNumInstance = InstanceDesc.iNumInstance;
	_iNumIndicesPerInstance = 1;

	_BufferDesc._stride = sizeof(VTXPOINT);
	_BufferDesc._numvertices = 1;

	_BufferDesc._indexStride = 2;
	_BufferDesc._numIndices = _iNumIndicesPerInstance * _iNumInstance;
	_BufferDesc._indexFormat = _BufferDesc._indexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	_BufferDesc._topology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
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

	VTXPOINT* pVertices = new VTXPOINT;
	::ZeroMemory(pVertices, sizeof(VTXPOINT));
	{
		pVertices->vPosition = Vec3(0.f, 0.f, 0.f);
		pVertices->vPSize = Vec2(1.f, 1.f);
	}

	::ZeroMemory(&_BufferDesc._subResourceData, sizeof(_BufferDesc._subResourceData));
	_BufferDesc._subResourceData.pSysMem = pVertices;

	if (FAILED(__super::CreateBuffer(&_vertexBuffer)))
		return E_FAIL;

	Safe_Delete_Array<VTXPOINT*>(pVertices);

	::ZeroMemory(&_BufferDesc._bufferDesc, sizeof(_BufferDesc._bufferDesc));
	{
		_BufferDesc._bufferDesc.ByteWidth = _BufferDesc._indexStride * _BufferDesc._indexStride;
		_BufferDesc._bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		_BufferDesc._bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		_BufferDesc._bufferDesc.CPUAccessFlags = 0;
		_BufferDesc._bufferDesc.MiscFlags = 0;
		_BufferDesc._bufferDesc.StructureByteStride = 0;
	}

	_ushort* pIndices = new _ushort[_BufferDesc._numIndices];
	::ZeroMemory(pIndices, sizeof(_ushort) * _BufferDesc._numIndices);

	::ZeroMemory(&_BufferDesc._subResourceData, sizeof(_BufferDesc._subResourceData));
	_BufferDesc._subResourceData.pSysMem = pIndices;

	if (FAILED(__super::CreateBuffer(&_indexBuffer)))
		return E_FAIL;

	Safe_Delete_Array<_ushort*>(pIndices);

	if (FAILED(__super::InitializePrototype(InstanceDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT VIBuffer_PointInstance::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT VIBuffer_PointInstance::Update(_float timeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource = {};

	_deviceContext->Map(_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	for (size_t i = 0; i < _iNumInstance; i++)
	{
		_pTimeAccs[i] += timeDelta;

		//reinterpret_cast<VTXINSTANCE*>(SubResource.pData)[i].vTranslation.y += _pSpeeds[i] * timeDelta;

		if (_pTimeAccs[i] >= _pLifeTimes[i])
		{
			//reinterpret_cast<VTXINSTANCE*>(SubResource.pData)[i].vTranslation.y = _pInstanceVertices[i].vTranslation.y;
			//_pTimeAccs[i] = 0.f;
		}
	}

	_deviceContext->Unmap(_pVBInstance, 0);

	return S_OK;
}

VIBuffer_PointInstance* VIBuffer_PointInstance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const INSTANCE_DESC& InstanceDesc)
{
	VIBuffer_PointInstance* pInstance = new VIBuffer_PointInstance(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype(InstanceDesc)))
	{
		MSG_BOX("Failed to Created : VIBufferPoint Instance");
		Safe_Release<VIBuffer_PointInstance*>(pInstance);
	}
	
	return pInstance;
}

Component* VIBuffer_PointInstance::Clone(void* pArg)
{
	VIBuffer_PointInstance* pInstance = new VIBuffer_PointInstance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : VIBufferPoint Instance");
		Safe_Release<VIBuffer_PointInstance*>(pInstance);
	}

	return pInstance;
}

void VIBuffer_PointInstance::Free()
{
	__super::Free();
}

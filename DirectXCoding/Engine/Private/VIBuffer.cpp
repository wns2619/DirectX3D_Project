#include "pch.h"
#include "VIBuffer.h"
#include "GameInstance.h"
#include "Transform.h"

VIBuffer::VIBuffer(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Component(device, deviceContext)
	, _vertexBuffer(nullptr)
	, _indexBuffer(nullptr)
	, _topology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
{
	ZeroMemory(&_bufferDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&_subResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	ZeroMemory(&_indexFormat, sizeof(DXGI_FORMAT));
}

VIBuffer::VIBuffer(const VIBuffer& rhs)
	: Component(rhs)
	, _vertexBuffer(rhs._vertexBuffer)
	, _indexBuffer(rhs._indexBuffer)
	, _stride(rhs._stride)
	, _numvertices(rhs._numvertices)
	, _indexStride(rhs._indexStride)
	, _numIndices(rhs._numIndices)
	, _indexFormat(rhs._indexFormat)
	, _topology(rhs._topology)
	, _numVBs(rhs._numVBs)
	, _subResourceData(rhs._subResourceData)
	, _bufferDesc(rhs._bufferDesc)
{
	Safe_AddRef<ID3D11Buffer*>(_vertexBuffer);
	Safe_AddRef<ID3D11Buffer*>(_indexBuffer);
}

HRESULT VIBuffer::InitializePrototype()
{

	return S_OK;
}

HRESULT VIBuffer::Initialize(void* argument)
{
	return S_OK;
}

HRESULT VIBuffer::Render()
{
	ID3D11Buffer* vertexBuffer[] = {
		_vertexBuffer,
	};

	uint32 Strides[] = {
		_stride,
	};

	uint32 offSet[] = {
		0,
	};

	_deviceContext->IASetPrimitiveTopology(_topology);

	_deviceContext->IASetVertexBuffers(0, _numVBs, vertexBuffer, Strides, offSet);
	_deviceContext->IASetIndexBuffer(_indexBuffer, _indexFormat, 0);

	// 1
	// 2 shader code 

	// 콘스탄트 버퍼 셋

	// 가장 마지막에 그린다.
	_deviceContext->DrawIndexed(_numIndices, 0, 0);

	return S_OK;
}

HRESULT VIBuffer::CreateBuffer(_Inout_ ID3D11Buffer** bufferOut)
{
	if (nullptr == _device)
		return E_FAIL;

	return _device->CreateBuffer(&_bufferDesc, &_subResourceData, bufferOut);
}

void VIBuffer::Free()
{
	__super::Free();

	Safe_Release<ID3D11Buffer*>(_vertexBuffer);
	Safe_Release<ID3D11Buffer*>(_indexBuffer);
}

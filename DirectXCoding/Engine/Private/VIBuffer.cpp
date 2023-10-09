#include "pch.h"
#include "VIBuffer.h"

VIBuffer::VIBuffer(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Component(device, deviceContext, COMPONENT_TYPE::BUFFER)
	, _vertexBuffer(nullptr)
	, _indexBuffer(nullptr)
{

}

VIBuffer::VIBuffer(const VIBuffer& rhs)
	: Component(rhs)
	, _vertexBuffer(rhs._vertexBuffer)
	, _indexBuffer(rhs._indexBuffer)
	, _BufferDesc(rhs._BufferDesc)
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
		_BufferDesc._stride,
	};

	uint32 offSet[] = {
		0,
	};



	_deviceContext->IASetVertexBuffers(0, _BufferDesc._numVBs, vertexBuffer, Strides, offSet);
	_deviceContext->IASetIndexBuffer(_indexBuffer, _BufferDesc._indexFormat, 0);
	_deviceContext->IASetPrimitiveTopology(_BufferDesc._topology);
	// 1
	// 2 shader code 

	// 콘스탄트 버퍼 셋

	// 가장 마지막에 그린다.
	_deviceContext->DrawIndexed(_BufferDesc._numIndices, 0, 0);

	return S_OK;
}

HRESULT VIBuffer::CreateBuffer(_Inout_ ID3D11Buffer** bufferOut)
{
	if (nullptr == _device)
		return E_FAIL;

	return _device->CreateBuffer(&_BufferDesc._bufferDesc, &_BufferDesc._subResourceData, bufferOut);
}

void VIBuffer::Free()
{
	__super::Free();

	Safe_Release<ID3D11Buffer*>(_vertexBuffer);
	Safe_Release<ID3D11Buffer*>(_indexBuffer);
}

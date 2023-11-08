#include "pch.h"
#include "VIBufferRect.h"

VIBufferRect::VIBufferRect(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: VIBuffer(device, deviceContext)
{
}

VIBufferRect::VIBufferRect(const VIBufferRect& rhs)
	: VIBuffer(rhs)
{

}

HRESULT VIBufferRect::InitializePrototype()
{
	_BufferDesc._stride = sizeof(VertexTextureData);
	_BufferDesc._numvertices = 4;
	_BufferDesc._indexStride = 2;
	_BufferDesc._numIndices = 6;
	_BufferDesc._indexFormat = _BufferDesc._indexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	_BufferDesc._topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
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

	VertexTextureData* vertices = new VertexTextureData[_BufferDesc._numvertices];
	ZeroMemory(vertices, sizeof(VertexTextureData) * _BufferDesc._numvertices);

	vertices[0].position = Vec3(-0.5f, 0.5f, 0.f);
	vertices[0].uv		 = Vec2(0.f, 0.f);

	vertices[1].position = Vec3(0.5f, 0.5f, 0.f);
	vertices[1].uv		 = Vec2(1.f, 0.f);


	vertices[2].position = Vec3(0.5f, -0.5f, 0.f);
	vertices[2].uv		 = Vec2(1.f, 1.f);

	vertices[3].position = Vec3(-0.5f, -0.5f, 0.f);
	vertices[3].uv		 = Vec2(0.f, 1.f);

	ZeroMemory(&_BufferDesc._subResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	_BufferDesc._subResourceData.pSysMem = vertices;

	if (FAILED(__super::CreateBuffer(&_vertexBuffer)))
		return E_FAIL;


	Safe_Delete_Array<VertexTextureData*>(vertices);

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
	Indices[4] = 2;
	Indices[5] = 3;


	ZeroMemory(&_BufferDesc._subResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	_BufferDesc._subResourceData.pSysMem = Indices;

	if (FAILED(__super::CreateBuffer(&_indexBuffer)))
		return E_FAIL;

	Safe_Delete_Array<_ushort*>(Indices);
#pragma endregion

	return S_OK;
}

HRESULT VIBufferRect::Initialize(void* argument)
{
	return S_OK;
}

VIBufferRect* VIBufferRect::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	VIBufferRect* bufferRect = new VIBufferRect(device, deviceContext);

	if (FAILED(bufferRect->InitializePrototype()))
	{
		MSG_BOX("Create bufferRect Failed");
		Safe_Release<VIBufferRect*>(bufferRect);
		return nullptr;
	}

	return bufferRect;
}

Component* VIBufferRect::Clone(void* argument)
{
	VIBufferRect* bufferClone = new VIBufferRect(*this);

	if (FAILED(bufferClone->Initialize(argument)))
	{
		MSG_BOX("Create CloneBufferRect Failed");
		Safe_Release<VIBufferRect*>(bufferClone);
		return nullptr;
	}

	return bufferClone;
}

void VIBufferRect::Free()
{
	__super::Free();

}

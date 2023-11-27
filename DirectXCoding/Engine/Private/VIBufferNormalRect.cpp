#include "pch.h"
#include "VIBufferNormalRect.h"

VIBufferNormalRect::VIBufferNormalRect(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: VIBuffer(device, deviceContext)
{
}

VIBufferNormalRect::VIBufferNormalRect(const VIBufferNormalRect& rhs)
	: VIBuffer(rhs)
{

}

HRESULT VIBufferNormalRect::InitializePrototype()
{
	_BufferDesc._stride = sizeof(VertexTextureNormalData);
	_BufferDesc._numvertices = 4;
	_BufferDesc._indexStride = 2;
	_BufferDesc._numIndices = 6;
	_BufferDesc._indexFormat = _BufferDesc._indexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	_BufferDesc._topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	_BufferDesc._numVBs = 1;

#pragma region vertexBuffer

	VertexTextureNormalData* vertices = new VertexTextureNormalData[_BufferDesc._numvertices];
	ZeroMemory(vertices, sizeof(VertexTextureNormalData) * _BufferDesc._numvertices);

	vertices[0].position = Vec3(-0.5f, 0.5f, 0.f);
	vertices[0].uv = Vec2(0.f, 0.f);

	vertices[1].position = Vec3(0.5f, 0.5f, 0.f);
	vertices[1].uv = Vec2(1.f, 0.f);


	vertices[2].position = Vec3(0.5f, -0.5f, 0.f);
	vertices[2].uv = Vec2(1.f, 1.f);

	vertices[3].position = Vec3(-0.5f, -0.5f, 0.f);
	vertices[3].uv = Vec2(0.f, 1.f);

#pragma endregion

#pragma region indexBuffer


	_ushort* Indices = new _ushort[_BufferDesc._numIndices];
	ZeroMemory(Indices, sizeof(_ushort) * _BufferDesc._numIndices);

	Indices[0] = 0;
	Indices[1] = 1;
	Indices[2] = 2;

	Vec4 vSourDir, vDestDir, vNormal;

	vSourDir = Vec4(vertices[Indices[1]].position - vertices[Indices[0]].position);
	vDestDir = Vec4(vertices[Indices[2]].position - vertices[Indices[1]].position);

	vNormal = ::XMVector3Normalize(::XMVector3Cross(vSourDir, vDestDir));

	vertices[Indices[0]].normal = vertices[Indices[0]].normal + vNormal;
	vertices[Indices[1]].normal = vertices[Indices[1]].normal + vNormal;
	vertices[Indices[2]].normal = vertices[Indices[2]].normal + vNormal;

	Indices[3] = 0;
	Indices[4] = 2;
	Indices[5] = 3;

	vSourDir = Vec4(vertices[Indices[2]].position - vertices[Indices[0]].position);
	vDestDir = Vec4(vertices[Indices[3]].position - vertices[Indices[2]].position);

	vNormal = ::XMVector3Normalize(::XMVector3Cross(vSourDir, vDestDir));

	vertices[Indices[0]].normal = vertices[Indices[0]].normal + vNormal;
	vertices[Indices[2]].normal = vertices[Indices[2]].normal + vNormal;
	vertices[Indices[3]].normal = vertices[Indices[3]].normal + vNormal;


	ZeroMemory(&_BufferDesc._bufferDesc, sizeof(D3D11_BUFFER_DESC));
	{
		_BufferDesc._bufferDesc.ByteWidth = _BufferDesc._stride * _BufferDesc._numvertices;
		_BufferDesc._bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		_BufferDesc._bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		_BufferDesc._bufferDesc.CPUAccessFlags = 0;
		_BufferDesc._bufferDesc.MiscFlags = 0;
		_BufferDesc._bufferDesc.StructureByteStride = _BufferDesc._stride;
	}

	ZeroMemory(&_BufferDesc._subResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	_BufferDesc._subResourceData.pSysMem = vertices;

	if (FAILED(__super::CreateBuffer(&_vertexBuffer)))
		return E_FAIL;


	ZeroMemory(&_BufferDesc._bufferDesc, sizeof(D3D11_BUFFER_DESC));
	{
		_BufferDesc._bufferDesc.ByteWidth = _BufferDesc._indexStride * _BufferDesc._numIndices;
		_BufferDesc._bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		_BufferDesc._bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		_BufferDesc._bufferDesc.CPUAccessFlags = 0;
		_BufferDesc._bufferDesc.MiscFlags = 0;
		_BufferDesc._bufferDesc.StructureByteStride = 0;
	}

	ZeroMemory(&_BufferDesc._subResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	_BufferDesc._subResourceData.pSysMem = Indices;

	if (FAILED(__super::CreateBuffer(&_indexBuffer)))
		return E_FAIL;


	Safe_Delete_Array<VertexTextureNormalData*>(vertices);
	Safe_Delete_Array<_ushort*>(Indices);
#pragma endregion

	return S_OK;
}

HRESULT VIBufferNormalRect::Initialize(void* argument)
{
	return S_OK;
}

VIBufferNormalRect* VIBufferNormalRect::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	VIBufferNormalRect* bufferRect = new VIBufferNormalRect(device, deviceContext);

	if (FAILED(bufferRect->InitializePrototype()))
	{
		MSG_BOX("Create bufferRect Failed");
		Safe_Release<VIBufferNormalRect*>(bufferRect);
		return nullptr;
	}

	return bufferRect;
}

Component* VIBufferNormalRect::Clone(void* argument)
{
	VIBufferNormalRect* bufferClone = new VIBufferNormalRect(*this);

	if (FAILED(bufferClone->Initialize(argument)))
	{
		MSG_BOX("Create CloneBufferRect Failed");
		Safe_Release<VIBufferNormalRect*>(bufferClone);
		return nullptr;
	}

	return bufferClone;
}

void VIBufferNormalRect::Free()
{
	__super::Free();

}

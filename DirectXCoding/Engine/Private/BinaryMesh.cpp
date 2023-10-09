#include "pch.h"
#include "BinaryMesh.h"

BinaryMesh::BinaryMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: VIBuffer(pDevice, pContext)
{
}

BinaryMesh::BinaryMesh(const BinaryMesh& rhs)
	: VIBuffer(rhs)
	, _MeshBufferDesc(rhs._MeshBufferDesc)
	, _materialIndex(rhs._materialIndex)
	, _numBones(rhs._numBones)
{

}

HRESULT BinaryMesh::InitializePrototype(BinaryModel::MODEL_TYPE type, VIBuffer::BUFFER_DESC viBufferInfo, 
	MESH_BUFFER_DESC binaryMeshInfo, VTXMESH* vertices,
	_ulong* Indices, uint32 MeshIndex, FXMMATRIX pivotMat)
{
	_BufferDesc._stride = BinaryModel::MODEL_TYPE::NONE == type ? sizeof(VTXMESH) : sizeof(VTXANIMMESH);

	_materialIndex = MeshIndex;
	_BufferDesc._numvertices = viBufferInfo._numvertices;
	_BufferDesc._indexStride = viBufferInfo._indexStride;
	_BufferDesc._numIndices = viBufferInfo._numIndices;
	_BufferDesc._indexFormat = viBufferInfo._indexFormat;
	_BufferDesc._topology = viBufferInfo._topology;
	_BufferDesc._numVBs = viBufferInfo._numVBs;


	::ZeroMemory(&_BufferDesc._bufferDesc, sizeof(_BufferDesc._bufferDesc));
	{
		_BufferDesc._bufferDesc.ByteWidth = binaryMeshInfo._vertexBufferDesc.ByteWidth;
		_BufferDesc._bufferDesc.Usage = binaryMeshInfo._vertexBufferDesc.Usage;
		_BufferDesc._bufferDesc.BindFlags = binaryMeshInfo._vertexBufferDesc.BindFlags;
		_BufferDesc._bufferDesc.CPUAccessFlags = binaryMeshInfo._vertexBufferDesc.CPUAccessFlags;
		_BufferDesc._bufferDesc.MiscFlags = binaryMeshInfo._vertexBufferDesc.MiscFlags;
		_BufferDesc._bufferDesc.StructureByteStride = binaryMeshInfo._vertexBufferDesc.StructureByteStride;


		ZeroMemory(&_BufferDesc._subResourceData, sizeof _BufferDesc._subResourceData);
		_BufferDesc._subResourceData.pSysMem = vertices;

		if (FAILED(__super::CreateBuffer(&_vertexBuffer)))
			return E_FAIL;
	}

	::ZeroMemory(&_BufferDesc._bufferDesc, sizeof(_BufferDesc._bufferDesc));
	{
		_BufferDesc._bufferDesc.ByteWidth = binaryMeshInfo._indexBufferDesc.ByteWidth;
		_BufferDesc._bufferDesc.Usage = binaryMeshInfo._indexBufferDesc.Usage;
		_BufferDesc._bufferDesc.BindFlags = binaryMeshInfo._indexBufferDesc.BindFlags;
		_BufferDesc._bufferDesc.CPUAccessFlags = binaryMeshInfo._indexBufferDesc.CPUAccessFlags;
		_BufferDesc._bufferDesc.MiscFlags = binaryMeshInfo._indexBufferDesc.MiscFlags;
		_BufferDesc._bufferDesc.StructureByteStride = binaryMeshInfo._indexBufferDesc.StructureByteStride;

		ZeroMemory(&_BufferDesc._subResourceData, sizeof(_BufferDesc._subResourceData));
		_BufferDesc._subResourceData.pSysMem = Indices;

		if (FAILED(__super::CreateBuffer(&_indexBuffer)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT BinaryMesh::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

HRESULT BinaryMesh::BindBoneMatrices(Shader* shader, const vector<class Bone*>& bones, const char* constantName)
{
	return E_NOTIMPL;
}

BinaryMesh* BinaryMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	BinaryModel::MODEL_TYPE type, VIBuffer::BUFFER_DESC viBufferInfo, 
	MESH_BUFFER_DESC binaryMeshInfo, uint32 MeshIndex, VTXMESH* vertices,
	_ulong* Indices, FXMMATRIX pivotMat)
{
	BinaryMesh* pInstance = new BinaryMesh(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype(type, viBufferInfo, binaryMeshInfo, vertices, Indices, MeshIndex, pivotMat)))
	{
		MSG_BOX("Create to Failed Binary Mesh");
		Safe_Release<BinaryMesh*>(pInstance);
	}

	return pInstance;
}

Component* BinaryMesh::Clone(void* pArg)
{
	BinaryMesh* pInstance = new BinaryMesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Create to Failed Binary Mesh");
		Safe_Release<BinaryMesh*>(pInstance);
	}

	return pInstance;
}

void BinaryMesh::Free()
{

}

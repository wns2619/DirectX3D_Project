#include "pch.h"
#include "BinaryMesh.h"
#include "Shader.h"
#include "BinaryBone.h"

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


HRESULT BinaryMesh::InitializePrototype(BinaryModel::MODEL_TYPE type, string& meshName, VIBuffer::BUFFER_DESC viBufferInfo, 
	MESH_BUFFER_DESC binaryMeshInfo, void* vertices,
	_ulong* Indices, uint32 MeshIndex, vector<uint32>& boneIndex, vector<Matrix>& offsetMatrix, uint32 NumberBone, FXMMATRIX pivotMat)
{
	_BufferDesc._stride = BinaryModel::MODEL_TYPE::NONE == type ? sizeof(VTXMESH) : sizeof(VTXANIMMESH);
	strcpy_s(_szName, meshName.c_str());
	
	_bones = boneIndex;
	_offsetMatrices = offsetMatrix;
	_numBones = NumberBone;

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
		_BufferDesc._subResourceData.pSysMem = static_cast<VTXANIMMESH*>(vertices);

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

HRESULT BinaryMesh::InitializePrototype(BinaryModel::MODEL_TYPE type, string& meshName, VIBuffer::BUFFER_DESC viBufferInfo, MESH_BUFFER_DESC binaryMeshInfo, void* vertices, _ulong* Indices, uint32 MeshIndex, FXMMATRIX pivotMat)
{
	_BufferDesc._stride = BinaryModel::MODEL_TYPE::NONE == type ? sizeof(VTXMESH) : sizeof(VTXANIMMESH);
	strcpy_s(_szName, meshName.c_str());

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
		_BufferDesc._subResourceData.pSysMem = static_cast<VTXMESH*>(vertices);

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

HRESULT BinaryMesh::BindBoneMatrices(Shader* shader, const vector<class BinaryBone*>& bones, const char* constantName)
{
	Matrix boneMatrices[256];
	::ZeroMemory(boneMatrices, sizeof(Matrix) * 256);

	uint32 index = 0;

	for (auto& boneIndex : _bones)
	{
		::XMStoreFloat4x4(&boneMatrices[index++],
			::XMLoadFloat4x4(&_offsetMatrices[index]) * bones[boneIndex]->GetCombinedTransformCaculator());
	}

	// 쉐이더에 넘길 본 매트릭스에 오프셋 매트릭스와 * 부모행렬이 곱해진 최종 변환행렬의 곱을 저장한다.

	return shader->BindMatrices(constantName, boneMatrices, _numBones);
}

BinaryMesh* BinaryMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext,
	BinaryModel::MODEL_TYPE type, string& meshName, VIBuffer::BUFFER_DESC viBufferInfo,
	MESH_BUFFER_DESC binaryMeshInfo, uint32 MeshIndex, void* vertices,
	_ulong* Indices, vector<uint32>& boneIndex, vector<Matrix>& offsetMatrix, uint32 NumberBone, FXMMATRIX pivotMat)
{
	BinaryMesh* pInstance = new BinaryMesh(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype(type, meshName, viBufferInfo, binaryMeshInfo, vertices, Indices, MeshIndex, boneIndex, offsetMatrix, NumberBone)))
	{
		MSG_BOX("Create to Failed Binary Mesh");
		Safe_Release<BinaryMesh*>(pInstance);
	}

	return pInstance;
}

BinaryMesh* BinaryMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, BinaryModel::MODEL_TYPE type, string& meshName, VIBuffer::BUFFER_DESC viBufferInfo, MESH_BUFFER_DESC binaryMeshInfo, uint32 MeshIndex, void* vertices, _ulong* Indices , FXMMATRIX pivotMat)
{
	BinaryMesh* pInstance = new BinaryMesh(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype(type, meshName, viBufferInfo, binaryMeshInfo, vertices, Indices, MeshIndex, pivotMat)))
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
	__super::Free();

}

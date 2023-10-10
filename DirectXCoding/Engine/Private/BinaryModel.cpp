#include "pch.h"
#include "BinaryModel.h"
#include "Mesh.h"
#include "BinaryMesh.h"
#include "Bone.h"
#include "Texture.h"
#include "FileUtils.h"
#include "Utils.h"

BinaryModel::BinaryModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: Component(pDevice, pContext, COMPONENT_TYPE::MODEL)
{
}

BinaryModel::BinaryModel(const BinaryModel& rhs)
	: Component(rhs)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, _numMaterial(rhs._numMaterial)
	, _materials(rhs._materials)
	, _bones(rhs._bones)
	, _ModelType(rhs._ModelType)
{
	for (auto& boneiter : _bones)
		Safe_AddRef<Bone*>(boneiter);

	for (auto& material : _materials)
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_AddRef<Texture*>(material._texture[i]);

	for (auto& mesh : m_Meshes)
		Safe_AddRef<BinaryMesh*>(mesh);
}

int32 BinaryModel::GetBoneIndex(const char* boneName) const
{
	uint32 boneIndex = 0;

	auto iter = find_if(_bones.begin(), _bones.end(), [&](Bone* bone)
		{
			if (false == ::strcmp(bone->GetBoneName(), boneName))
				return true;

			++boneIndex;

			return false;
		});

	if (iter == _bones.end())
		return -1;

	return boneIndex;
}

HRESULT BinaryModel::InitializePrototype(MODEL_TYPE type, const string& pBinaryModelFilePath, FXMMATRIX pivotMat)
{
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(Utils::ToWString(pBinaryModelFilePath), FileMode::Read);

	uint32 iType;
	file->Read<uint32>(iType);
	_ModelType = static_cast<MODEL_TYPE>(iType);
	//// 모델 타입 저장.

	Matrix pivotMatrix;
	file->Read<Matrix>(pivotMatrix);
	_pivotMatrix = pivotMatrix;
	//// 모텔 사전 행렬 저장

	uint32 NumMeshes;
	file->Read<uint32>(NumMeshes);
	m_iNumMeshes = NumMeshes;
	//// 메쉬 개수 -> 나중에 이 메쉬만큼 돌아야함.

	for (uint32 i = 0; i < NumMeshes; ++i)
	{
		// 메쉬를 구성하고있는 버퍼를 어떤 형식으로 읽을건지 메인 BufferDesc
		VIBuffer::BUFFER_DESC viBufferDesc;
		file->Read<VIBuffer::BUFFER_DESC>(viBufferDesc);

		// 그리고 그 형식으로 이루어진 Mesh Desc
		BinaryMesh::MESH_BUFFER_DESC meshBufferDesc;
		file->Read<BinaryMesh::MESH_BUFFER_DESC>(meshBufferDesc);

		// 메쉬 인덱스 
		uint32 meshIndex;
		file->Read<uint32>(meshIndex);

		// 그 정보로 구성된 버텍스 
		uint32 Numvertices;
		file->Read<uint32>(Numvertices);

		VTXMESH* pVertices = new VTXMESH[Numvertices];
		ZeroMemory(pVertices, sizeof(VTXMESH));

		for (uint32 i = 0; i < Numvertices; i++)
		{
			file->Read<Vec3>(pVertices[i].position);
			file->Read<Vec3>(pVertices[i].normal);
			file->Read<Vec2>(pVertices[i].texcoord);
			file->Read<Vec3>(pVertices[i].tangent);
			file->Read<Vec3>(pVertices[i].bitangent);
		}
		// 그 정보로 구성된 버텍스로 이루어진 인덱스 


		uint32 IndicesCount = 0;
		file->Read<uint32>(IndicesCount);

		_ulong* pIndices = new _ulong[IndicesCount * 3];
		ZeroMemory(pIndices, sizeof(pIndices));

		uint32 numIndices = 0;


		for (uint32 i = 0; i < IndicesCount; ++i)
		{
			file->Read<_ulong>(pIndices[numIndices++]);
			file->Read<_ulong>(pIndices[numIndices++]);
			file->Read<_ulong>(pIndices[numIndices++]);
		}

		BinaryMesh* newBinaryMesh = 
			BinaryMesh::Create(_device, _deviceContext, type, viBufferDesc, meshBufferDesc, meshIndex, pVertices, pIndices, pivotMatrix);

		//if (nullptr == newBinaryMesh)
		//	return E_FAIL;

		m_Meshes.push_back(newBinaryMesh);

		Safe_Delete_Array<VTXMESH*>(pVertices);
		Safe_Delete_Array<_ulong*>(pIndices);
	}

	//// 마테리얼 개수 
	//uint32 NumMaterialCount;
 //	file->Read<uint32>(NumMaterialCount);
	//_numMaterial = NumMaterialCount;
	//

	//for (uint32 i = 0; i < NumMaterialCount; ++i)
	//{
	//	uint32 textureCount;
	//	file->Read<uint32>(textureCount);

	//	MESH_MATERIAL meshMaterials;
	//	::ZeroMemory(&meshMaterials, sizeof(meshMaterials));

	//	for (uint32 j = 0; j < textureCount; ++j)
	//	{

	//		uint32 textureType;
	//		file->Read<uint32>(textureType);

	//		string texturePath;
	//		file->Read(texturePath);

	//		Texture* texture = Texture::Create(_device, _deviceContext, Utils::ToWString(texturePath));
	//		if (texture == nullptr)
	//			return E_FAIL;

	//		meshMaterials._texture[textureType] = texture;
	//	}
	//	_materials.push_back(meshMaterials);
	//}

	return S_OK;
}

HRESULT BinaryModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT BinaryModel::BindBoneMatrices(Shader* shader, uint32 meshIndex, const char* constantName)
{
	return m_Meshes[meshIndex]->BindBoneMatrices(shader, _bones, constantName);
}

HRESULT BinaryModel::BindMaterialTexture(Shader* shader, const char* constantName, uint32 meshIndex, aiTextureType type)
{
	if (meshIndex >= m_iNumMeshes)
		return E_FAIL;

	uint32 iMaterialIndex = m_Meshes[meshIndex]->GetMaterialIndex();

	if (iMaterialIndex >= _numMaterial)
		return E_FAIL;


	return _materials[iMaterialIndex]._texture[type]->BindShaderResource(shader, constantName, 0);
}

HRESULT BinaryModel::PlayAnimation(const _float& timeDelta)
{
	// 뼈들의 트랜스폼 매트릭스를 애니메이션 상태에 맞도록 바꿔준다 deletatime.
	for (auto& bone : _bones)
		bone->UpdateCombinedTransformMatrix(_bones);

	return S_OK;
}

HRESULT BinaryModel::Render(uint32 meshIndex)
{
	m_Meshes[meshIndex]->Render();

	return S_OK;
}

BinaryModel* BinaryModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE type, const string& pBinaryModelFilePath, FXMMATRIX pivotMat)
{
	BinaryModel* pInstance = new BinaryModel(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype(type, pBinaryModelFilePath, pivotMat)))
	{
		MSG_BOX("Failed to Created : Model");
		Safe_Release<BinaryModel*>(pInstance);
	}

	return pInstance;
}

Component* BinaryModel::Clone(void* pArg)
{
	BinaryModel* pInstance = new BinaryModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : Model");
		Safe_Release<BinaryModel*>(pInstance);
	}

	return pInstance;
}

void BinaryModel::Free()
{
	__super::Free();

	for (auto& material : _materials)
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_Release<Texture*>(material._texture[i]);

	_materials.clear();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);

	m_Meshes.clear();

	for (auto& bone : _bones)
		Safe_Release<Bone*>(bone);

	_bones.clear();

}

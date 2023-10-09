#include "pch.h"
#include "BinaryModel.h"
#include "Mesh.h"
#include "Bone.h"
#include "Texture.h"

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
}

HRESULT BinaryModel::InitializePrototype(Model_TYPE type, const string& pBinaryModelFilePath, FXMMATRIX pivotMat)
{
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

BinaryModel* BinaryModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Model_TYPE type, const string& pBinaryModelFilePath, FXMMATRIX pivotMat)
{
	return nullptr;
}

Component* BinaryModel::Clone(void* pArg)
{
	return nullptr;
}

void BinaryModel::Free()
{
	__super::Free();


	for (auto& material : _materials)
		for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			Safe_Release<Texture*>(material._texture[i]);

	_materials.clear();

	for (auto& pMesh : m_Meshes)
	{
		Safe_Release(pMesh);
	}

	m_Meshes.clear();

	for (auto& bone : _bones)
		Safe_Release<Bone*>(bone);

	_bones.clear();


}

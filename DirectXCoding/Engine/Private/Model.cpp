#include "pch.h"
#include "Model.h"
#include "Mesh.h"
#include "Bone.h"
#include "Texture.h"
#include "Converter.h"
#include "Utils.h"
#include "FileUtils.h"
#include <filesystem>
#include "Animation.h"

Model::Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Component(pDevice, pContext, COMPONENT_TYPE::MODEL)
{
  
}

Model::Model(const Model& rhs)
    : Component(rhs)
    , m_iNumMeshes(rhs.m_iNumMeshes)
    , m_Meshes(rhs.m_Meshes)
    , _numMaterial(rhs._numMaterial)
    , _materials(rhs._materials)
    , _modelPath(rhs._modelPath)
    , _ModelType(rhs._ModelType)
    , _numAnimations(rhs._numAnimations)
    , _pivotMatrix(rhs._pivotMatrix)
{

    for (auto& pAnimation : rhs._animations)
        _animations.push_back(pAnimation->Clone());

    for (auto& boneiter : rhs._bones)
        _bones.push_back(boneiter->Clone());

    for (auto& material : _materials)
        for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
            Safe_AddRef<Texture*>(material._texture[i]);

    for (auto& mesh : m_Meshes)
        Safe_AddRef<Mesh*>(mesh);
}

int32 Model::GetBoneIndex(const char* boneName) const
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

Matrix* Model::GetBoneMatrix(const _char* pBoneName) const
{
    auto iter = find_if(_bones.begin(), _bones.end(), [&](Bone* pBone)
        {
            if (false == ::strcmp(pBone->GetBoneName(), pBoneName))
                return true;

            return false;
        });

    return (*iter)->GetCombientTransformMatrixPoint();
}

HRESULT Model::InitializePrototype(MODEL_TYPE type, const string& pModelFilePath, FXMMATRIX pivotMat)
{
    _modelPath = pModelFilePath;
    _ModelType = type;

     uint32 flag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

    if (MODEL_TYPE::NONE == type)
        flag |= aiProcess_PreTransformVertices;

    m_pAIScene = m_Importer.ReadFile(pModelFilePath.c_str(), flag);
    if (nullptr == m_pAIScene)
        return E_FAIL;

    ::XMStoreFloat4x4(&_pivotMatrix, pivotMat);
    
    /* DirectX로 그려낼 수 있도록 데이터들을 정리한다. */

	/* 모델의 전체 뼈를 로드하자 */
	/* aiNode : 뼈들의 상속관계. 부모가 누구인지에대한. 부모자식의 관계를 알고 있다면. 
	뼈의 최종적인 상태행렬. */
    if (FAILED(ReadyBones(m_pAIScene->mRootNode, -1)))
        return E_FAIL;

    /* 모델의 각 파츠 정점 정보를 로드하나다. */
    /* 메시에게 메시당 영향을 주는 뼈들의 인덱스를 모아두었다. */
    /* 렌더링할 때 메시 별로 렌더링을 수행을하고. 메시 당 사용하는 뼏르을 모아서 셰이더에 전달해줘야할 필요가 있었다. */
    /* 메시를 구성하는 정점들에게 어떤 뼈들의 영향을 얼마나 받는지에 대한 정보를 추가했다. */
    /* 혹여 특정 메시가 뼈의 갯수가 정의되어있지 않은 경우, 강제로 뼈의 갯수를 1개로 늘렸다.
    추가한 뼈는 메시랑 이름이 같은 뼈를 추가해놓는방식. */
    if (FAILED(ReadyMeshes(type)))
        return E_FAIL;

    /* 모델의 픽셀별로 정의된 재질 정보를 저장해준 텍스쳐를 읽어오자. */
    if (FAILED(ReadyMaterial(pModelFilePath)))
        return E_FAIL;

    /* 애니메이션의 갯수(걷기, 대기, 공격, 등등등 )*/
    /* 애니메이션을 로드한다. -> 이 애니메이션은 어떤 뼈(Channel)들을 갱신하는가? -> 이 뼈는 이 애니메이션 내에서 어떤 시간대에 어떤 상태(KeyFrame)를 취하는지를 뼈마다 로드해놓는다. */
    if (FAILED(ReadyAnimations()))
        return E_FAIL;

    return S_OK;
}

HRESULT Model::Initialize(void* pArg)
{


    return S_OK;
}

HRESULT Model::SetUp_Animation(_bool isLoop, uint32 animationIndex)
{
    if (animationIndex >= _numAnimations ||
        animationIndex == _currenAnimIndex)
        return S_OK;

    _animations[_currenAnimIndex]->Reset();

    _currenAnimIndex = animationIndex;

    _animations[_currenAnimIndex]->SetLoop(isLoop);

    return S_OK;
}

HRESULT Model::BindBoneMatrices(Shader* shader, uint32 meshIndex, const char* constantName)
{
    return m_Meshes[meshIndex]->BindBoneMatrices(shader, _bones, constantName, _pivotMatrix);
}

HRESULT Model::BindMaterialTexture(Shader* shader, const char* constantName, uint32 meshIndex, aiTextureType type)
{
    if (meshIndex >= m_iNumMeshes)
        return E_FAIL;

    uint32 iMaterialIndex = m_Meshes[meshIndex]->GetMaterialIndex();

    if (iMaterialIndex >= _numMaterial)
        return E_FAIL;

   
    return _materials[iMaterialIndex]._texture[type]->BindShaderResource(shader, constantName, 0);
}

HRESULT Model::PlayAnimation(const _float& timeDelta)
{
    if (0 == _numAnimations)
        return S_OK;
    /* 뼈들의 TransformationMatrix를 애니메이션 상태(Animation -> Channel -> 시간에 맞는 KeyFrame으로 맞도록 바꿔준다. */

    /* 이 애니메이션에서 사용되는 뼈들의 TransformationMatrix행렬을 갱신하고. */
    /* TransformationMatrix : 부모기준으로 표현된 이 뼈만의 행렬. */
    _animations[_currenAnimIndex]->UpdateTransformationMatrix(_bones, timeDelta);


    for (auto& bone : _bones)
        bone->UpdateCombinedTransformMatrix(_bones);

    return S_OK;
}

HRESULT Model::Render(uint32 meshIndex)
{
    m_Meshes[meshIndex]->Render();

    return S_OK;
}

HRESULT Model::ReadyMeshes(MODEL_TYPE type)
{
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    m_Meshes.reserve(m_iNumMeshes);

    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        Mesh* mesh = Mesh::Create(_device, _deviceContext, type, this, m_pAIScene->mMeshes[i], ::XMLoadFloat4x4(&_pivotMatrix));
        if (nullptr == mesh)
            return E_FAIL;

        m_Meshes.push_back(mesh);
    }

    return S_OK;
}

HRESULT Model::ReadyMaterial(const string& modelFilePath)
{
    _numMaterial = m_pAIScene->mNumMaterials;

    string name = "";
    struct dirent* entry;

    size_t found = modelFilePath.find_last_of("/\\");

    string pathName = "";

    if (found != string::npos)
        pathName = modelFilePath.substr(0, found);

    DIR* dir = ::opendir(pathName.c_str());

    if (dir != NULL)
    {
        for (size_t i = 0; i < _numMaterial; i++)
        {
            aiMaterial* materials = m_pAIScene->mMaterials[i];
            // 마테리얼의 정보를 불러온다.

            MESH_MATERIAL MeshMaterial;
            ::ZeroMemory(&MeshMaterial, sizeof(MeshMaterial));

            for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
            {
                aiString		strTexturePath;

                if (FAILED(materials->GetTexture(static_cast<aiTextureType>(j), 0, &strTexturePath)))
                    continue;


                string textureFileName = strTexturePath.C_Str();
                string fullPath = pathName + "\\" + textureFileName;

                wstring convert = wstring(fullPath.begin(), fullPath.end());


                MeshMaterial._texture[j] = Texture::Create(_device, _deviceContext, convert);
                if (nullptr == MeshMaterial._texture[j])
                {
                    ::closedir(dir);
                    return E_FAIL;
                }

                ++MeshMaterial.textureCount;
            }

            _materials.push_back(MeshMaterial);
        }

        ::closedir(dir);
    }


    return S_OK;
}

HRESULT Model::ReadyBones(const aiNode* node, int32 parentNodeIndex)
{
    // 맨 처음 root Node를 생성하고 인덱스를 넣는다.
    Bone* bone = Bone::Create(node, parentNodeIndex);
    if (nullptr == bone)
        return E_FAIL;

    // 생성된 뼈를 push
    _bones.push_back(bone);

    // 부모의 뼈가 있다면 push된 사이즈에서 - 1 인덱스의 번호를 갖는다.
    // ex -> 0 1 4 5 2 . . . 
    // -> 0번 뼈는 -1 인덱스(부모가 없을 때), 1번뼈가 부모로라면 4, 5번은 size() - 1로 1번 인덱스를 부모로 갖는다.
    int32 parentindex = _bones.size() - 1;

    // 재귀적으로 호출.
    for (size_t i = 0; i < node->mNumChildren; i++)
        ReadyBones(node->mChildren[i], parentindex);

    return S_OK;
}

HRESULT Model::ReadyAnimations()
{
    _numAnimations = m_pAIScene->mNumAnimations;

    for (uint32 i = 0; i < _numAnimations; ++i)
    {
        Animation* pAnimation = Animation::Create(this, m_pAIScene->mAnimations[i]);
        if (nullptr == pAnimation)
            return E_FAIL;

        _animations.push_back(pAnimation);
    }

    return S_OK;
}

Model* Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL_TYPE type, const string& pModelFilePath, FXMMATRIX pivotMat)
{
    Model* pInstance = new Model(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype(type, pModelFilePath, pivotMat)))
    {
        MSG_BOX("Failed to Created : Model");
        Safe_Release<Model*>(pInstance);
    }

    return pInstance;
}

Component* Model::Clone(void* pArg)
{
    Model* pInstance = new Model(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : Model");
        Safe_Release<Model*>(pInstance);
    }

    return pInstance;
}

void Model::Free()
{
    __super::Free();

    for (auto& pAnimation : _animations)
        Safe_Release<Animation*>(pAnimation);

    _animations.clear();


    for (auto& material : _materials)
        for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
            Safe_Release<Texture*>(material._texture[i]);

    _materials.clear();

    for (auto& pMesh : m_Meshes)
    {
        Safe_Release(pMesh);
    }

    m_Meshes.clear();

    m_Importer.FreeScene();

    for (auto& bone : _bones)
        Safe_Release<Bone*>(bone);

    _bones.clear();
}

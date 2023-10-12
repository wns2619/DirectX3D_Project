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
    
    /* DirectX�� �׷��� �� �ֵ��� �����͵��� �����Ѵ�. */

	/* ���� ��ü ���� �ε����� */
	/* aiNode : ������ ��Ӱ���. �θ� ��������������. �θ��ڽ��� ���踦 �˰� �ִٸ�. 
	���� �������� �������. */
    if (FAILED(ReadyBones(m_pAIScene->mRootNode, -1)))
        return E_FAIL;

    /* ���� �� ���� ���� ������ �ε��ϳ���. */
    /* �޽ÿ��� �޽ô� ������ �ִ� ������ �ε����� ��Ƶξ���. */
    /* �������� �� �޽� ���� �������� �������ϰ�. �޽� �� ����ϴ� ������ ��Ƽ� ���̴��� ����������� �ʿ䰡 �־���. */
    /* �޽ø� �����ϴ� �����鿡�� � ������ ������ �󸶳� �޴����� ���� ������ �߰��ߴ�. */
    /* Ȥ�� Ư�� �޽ð� ���� ������ ���ǵǾ����� ���� ���, ������ ���� ������ 1���� �÷ȴ�.
    �߰��� ���� �޽ö� �̸��� ���� ���� �߰��س��¹��. */
    if (FAILED(ReadyMeshes(type)))
        return E_FAIL;

    /* ���� �ȼ����� ���ǵ� ���� ������ �������� �ؽ��ĸ� �о����. */
    if (FAILED(ReadyMaterial(pModelFilePath)))
        return E_FAIL;

    /* �ִϸ��̼��� ����(�ȱ�, ���, ����, ���� )*/
    /* �ִϸ��̼��� �ε��Ѵ�. -> �� �ִϸ��̼��� � ��(Channel)���� �����ϴ°�? -> �� ���� �� �ִϸ��̼� ������ � �ð��뿡 � ����(KeyFrame)�� ���ϴ����� ������ �ε��س��´�. */
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
    /* ������ TransformationMatrix�� �ִϸ��̼� ����(Animation -> Channel -> �ð��� �´� KeyFrame���� �µ��� �ٲ��ش�. */

    /* �� �ִϸ��̼ǿ��� ���Ǵ� ������ TransformationMatrix����� �����ϰ�. */
    /* TransformationMatrix : �θ�������� ǥ���� �� ������ ���. */
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
            // ���׸����� ������ �ҷ��´�.

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
    // �� ó�� root Node�� �����ϰ� �ε����� �ִ´�.
    Bone* bone = Bone::Create(node, parentNodeIndex);
    if (nullptr == bone)
        return E_FAIL;

    // ������ ���� push
    _bones.push_back(bone);

    // �θ��� ���� �ִٸ� push�� ������� - 1 �ε����� ��ȣ�� ���´�.
    // ex -> 0 1 4 5 2 . . . 
    // -> 0�� ���� -1 �ε���(�θ� ���� ��), 1������ �θ�ζ�� 4, 5���� size() - 1�� 1�� �ε����� �θ�� ���´�.
    int32 parentindex = _bones.size() - 1;

    // ��������� ȣ��.
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

#include "pch.h"
#include "Model.h"
#include "Mesh.h"
#include "Texture.h"

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
{
    for (auto& material : _materials)
        for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
            Safe_AddRef<Texture*>(material._texture[i]);

    for (auto& mesh : m_Meshes)
        Safe_AddRef<Mesh*>(mesh);
}

HRESULT Model::InitializePrototype(const string& pModelFilePath, FXMMATRIX pivotMat)
{
    _modelPath = pModelFilePath;


    m_pAIScene = m_Importer.ReadFile(pModelFilePath.c_str(), aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);
    if (nullptr == m_pAIScene)
        return E_FAIL;

    ::XMStoreFloat4x4(&_pivotMatrix, pivotMat);


    if (FAILED(ReadyMeshes()))
        return E_FAIL;

    if (FAILED(ReadyMaterial(pModelFilePath)))
        return E_FAIL;

    return S_OK;
}

HRESULT Model::Initialize(void* pArg)
{
    return S_OK;
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

HRESULT Model::Render(uint32 meshIndex)
{
    m_Meshes[meshIndex]->Render();

    return S_OK;
}

HRESULT Model::ReadyMeshes()
{
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    m_Meshes.reserve(m_iNumMeshes);

    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        Mesh* mesh = Mesh::Create(_device, _deviceContext, m_pAIScene->mMeshes[i], ::XMLoadFloat4x4(&_pivotMatrix));
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
                string fullPath = pathName + "/" + textureFileName;

                wstring convert = wstring(fullPath.begin(), fullPath.end());


                MeshMaterial._texture[j] = Texture::Create(_device, _deviceContext, convert);
                if (nullptr == MeshMaterial._texture[j])
                {
                    ::closedir(dir);
                    return E_FAIL;
                }
            }

            _materials.push_back(MeshMaterial);
        }

        ::closedir(dir);
    }


    return S_OK;
}

Model* Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const string& pModelFilePath, FXMMATRIX pivotMat)
{
    Model* pInstance = new Model(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype(pModelFilePath, pivotMat)))
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


    for (auto& material : _materials)
        for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
            Safe_Release<Texture*>(material._texture[i]);

    _materials.clear();

    for (auto& pMesh : m_Meshes)
    {
        Safe_Release(pMesh);
    }
    m_Importer.FreeScene();
}

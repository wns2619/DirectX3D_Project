#include "pch.h"
#include "Model.h"
#include "Mesh.h"

Model::Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Component(pDevice, pContext)
{

}

Model::Model(const Model& rhs)
    : Component(rhs)
    , m_iNumMeshes(rhs.m_iNumMeshes)
    , m_Meshes(rhs.m_Meshes)
{
    for (auto& mesh : m_Meshes)
        Safe_AddRef<Mesh*>(mesh);
}

HRESULT Model::InitializePrototype(const char* pModelFilePath)
{
    m_pAIScene = m_Importer.ReadFile(pModelFilePath, aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast);
    if (nullptr == m_pAIScene)
        return E_FAIL;

    if (FAILED(ReadyMeshes()))
        return E_FAIL;

    return S_OK;
}

HRESULT Model::Initialize(void* pArg)
{
    return S_OK;
}

HRESULT Model::Render()
{
    for (auto& pMesh : m_Meshes)
    {
        if (nullptr != pMesh)
            pMesh->Render();
    }

    return S_OK;
}

HRESULT Model::ReadyMeshes()
{
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    m_Meshes.reserve(m_iNumMeshes);

    for (size_t i = 0; i < m_iNumMeshes; i++)
    {
        Mesh* mesh = Mesh::Create(_device, _deviceContext, m_pAIScene->mMeshes[i]);
        if (nullptr == mesh)
            return E_FAIL;

        m_Meshes.push_back(mesh);
    }

    return S_OK;
}

Model* Model::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pModelFilePath)
{
    Model* pInstance = new Model(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype(pModelFilePath)))
    {
        MSG_BOX("Failed to Created : Model");
        Safe_Release(pInstance);
    }

    return pInstance;
}

Component* Model::Clone(void* pArg)
{
    Model* pInstance = new Model(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : Model");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void Model::Free()
{
    __super::Free();

    for (auto& pMesh : m_Meshes)
    {
        Safe_Release(pMesh);
    }
    m_Importer.FreeScene();
}

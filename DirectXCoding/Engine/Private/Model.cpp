#include "pch.h"
#include "Model.h"
#include "Mesh.h"
#include "Bone.h"
#include "Texture.h"
#include "Converter.h"
#include "Utils.h"
#include "FileUtils.h"
#include <filesystem>

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
    , m_pAIScene(rhs.m_pAIScene)
    , _bones(rhs._bones)
{
    for (auto& boneiter : _bones)
        Safe_AddRef<Bone*>(boneiter);

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

HRESULT Model::InitializePrototype(MODEL_TYPE type, const string& pModelFilePath, FXMMATRIX pivotMat)
{
    _modelPath = pModelFilePath;

    uint32 flag = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

    if (MODEL_TYPE::NONE == type)
        flag |= aiProcess_PreTransformVertices;

    m_pAIScene = m_Importer.ReadFile(pModelFilePath.c_str(), flag);
    if (nullptr == m_pAIScene)
        return E_FAIL;

    ::XMStoreFloat4x4(&_pivotMatrix, pivotMat);



    // 재귀적으로 함수 호출한다. 처음에 루트를 시작할 node.
    if (FAILED(ReadyBones(m_pAIScene->mRootNode, -1)))
        return E_FAIL;

    if (FAILED(ReadyMeshes(type)))
        return E_FAIL;

    if (FAILED(ReadyMaterial(pModelFilePath)))
        return E_FAIL;



    return S_OK;
}

HRESULT Model::Initialize(void* pArg)
{


    return S_OK;
}

HRESULT Model::BindBoneMatrices(Shader* shader, uint32 meshIndex, const char* constantName)
{
    return m_Meshes[meshIndex]->BindBoneMatrices(shader, _bones, constantName);
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
    // 뼈들의 트랜스폼 매트릭스를 애니메이션 상태에 맞도록 바꿔준다 deletatime.


    for (auto& bone : _bones)
        bone->UpdateCombinedTransformMatrix(_bones);

    return S_OK;
}

HRESULT Model::Render(uint32 meshIndex)
{
    m_Meshes[meshIndex]->Render();

    return S_OK;
}

void Model::ReadMaterial(wstring fileName)
{
    wstring fullPath = Utils::ToWString(_texturePath) + fileName + L".xml";
    auto parentPath = filesystem::path(fullPath).parent_path();

    tinyxml2::XMLDocument* document = new tinyxml2::XMLDocument();
    tinyxml2::XMLError error = document->LoadFile(Utils::ToString(fullPath).c_str());
    assert(error == tinyxml2::XML_SUCCESS);

    tinyxml2::XMLElement* root = document->FirstChildElement();
    tinyxml2::XMLElement* materialNode = root->FirstChildElement();

    while (materialNode)
    {
        shared_ptr<asMaterial> material = make_shared<asMaterial>();

        tinyxml2::XMLElement* node = nullptr;

        node = materialNode->FirstChildElement();
        material->name = node->GetText();

        // Diffuse Texture
        node = node->NextSiblingElement();

        if (node->GetText())
        {
            wstring DiffusetextureStr = Utils::ToWString(node->GetText());
            if (DiffusetextureStr.length() > 0)
            {
                // TODO
                
                //auto texture = 
                // 읽어왔으면 텍스쳐를 찾아서 마테리얼의 디퓨즈에 넣는다.
                // 모델 자체가 Material을 들고 있으니까 여기다가 푸쉬해서 넣어주면 될 듯.
            }
        }

        // Specular Texture
        node = node->NextSiblingElement();
        if (node->GetText())
        {
            wstring SpecularTextureStr = Utils::ToWString(node->GetText());
            if (SpecularTextureStr.length() > 0)
            {
                // specular texture add 

            }
        }

        // NormalTexture 
        node = node->NextSiblingElement();
        if (node->GetText())
        {
            wstring normalTextureStr = Utils::ToWString(node->GetText());
            if (normalTextureStr.length() > 0)
            {
                // TODO
                // normalTexture Add

            }
        }
    }

}

void Model::ReadModel(wstring fileName)
{
    wstring fullPath = Utils::ToWString(_modelPath) + fileName + L".mesh";

    shared_ptr<FileUtils> file = make_shared<FileUtils>();
    file->Open(fullPath, FileMode::Read);

    // Bones
    {
        const uint32 count = file->Read<uint32>();

        for (uint32 i = 0; i < count; ++i)
        {
            shared_ptr<ModelBone> bone = make_shared<ModelBone>();
            bone->index = file->Read<int32>();
            bone->name = Utils::ToWString(file->Read<string>());
            bone->parentIndex = file->Read<int32>();
            bone->transform = file->Read<Matrix>();

            // _bones.push_back(bone);
        }
    }

    // Mesh
    {
        const uint32 count = file->Read<uint32>();

        for (uint32 i = 0; i < count; ++i)
        {
           // shared_ptr<Mesh> mesh = make_shared<Mesh>();

           //mesh->name = Utils::ToWString(file->Read<string>());
           //mesh->boneindex = file->Read<int32>();
           //
           //mesh->mateiralname = Utils::ToWString(file->Read<string>());

            // VertexData
            {
                const uint32 count = file->Read<uint32>();
                vector<VertexType> vertices;
                vertices.resize(count);

                void* data = vertices.data();
                file->Read(&data, sizeof(VertexType) * count);
                // mesh add geometry;
            }

            // indexData
            {
                const uint32 count = file->Read<uint32>();

                vector<uint32> indices;
                indices.resize(count);

                void* data = indices.data();
                file->Read(&data, sizeof(uint32) * count);
                // mesh add indices
            }

            // mesh create buffer

            // mesh pushback
        }
    }

}

void Model::LoadFBXFile(const wstring& fileName)
{
    _converter = new Converter();

    _converter->ReadAssetFile(fileName);
    
    _converter->ImportModelData();
    _converter->ImportMaterialData();
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

    if(_converter != nullptr)
        Safe_Delete<Converter*>(_converter);
}

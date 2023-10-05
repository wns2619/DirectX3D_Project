#include "pch.h"
#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"

Mesh::Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : VIBuffer(pDevice, pContext)
{
}

Mesh::Mesh(const Mesh& rhs)
    : VIBuffer(rhs)

{

}

HRESULT Mesh::InitializePrototype(Model::MODEL_TYPE type, const Model* model, const aiMesh* pAIMesh, FXMMATRIX pivotMat)
{
    _stride = Model::MODEL_TYPE::NONE == type ? sizeof(VTXMESH) : sizeof(VTXANIMMESH);
    
    // ���׸����� �ε��� ���ڸ� �����Ѵ�. �� �ε����� ���׸��� ������ �̿�
    _materialIndex = pAIMesh->mMaterialIndex;
    _numvertices = pAIMesh->mNumVertices;
    _indexStride = 4; /* �ε��� �ϳ��� ũ��. 2 or 4 */
    _numIndices = pAIMesh->mNumFaces * 3;
    _indexFormat = _indexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    _topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    _numVBs = 1;

#pragma region VERTEX_BUFFER

    /* �������ۿ� �ε��� ���۸� ���峮. */
    ZeroMemory(&_bufferDesc, sizeof _bufferDesc);

    // m_BufferDesc.ByteWidth = �����ϳ��� ũ��(Byte) * ������ ����;
    _bufferDesc.ByteWidth = _stride * _numvertices;
    _bufferDesc.Usage = D3D11_USAGE_DEFAULT; /* �������۷� �Ҵ��Ѵ�. (Lock, unLock ȣ�� �Ұ�)*/
    _bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    _bufferDesc.CPUAccessFlags = 0;
    _bufferDesc.MiscFlags = 0;
    _bufferDesc.StructureByteStride = _stride;

    HRESULT hr = Model::MODEL_TYPE::NONE == type ? ReadyVertexBufferNoneAnim(pAIMesh, pivotMat) : ReadyVertexBufferAnim(pAIMesh, model);

#pragma endregion

#pragma region INDEX_BUFFER

    /* �������ۿ� �ε��� ���۸� ���峮. */
    ZeroMemory(&_bufferDesc, sizeof _bufferDesc);
    _bufferDesc.ByteWidth = _indexStride * _numIndices;
    _bufferDesc.Usage = D3D11_USAGE_DEFAULT; /* �������۷� �Ҵ��Ѵ�. (Lock, unLock ȣ�� �Ұ�)*/
    _bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    _bufferDesc.CPUAccessFlags = 0;
    _bufferDesc.MiscFlags = 0;
    _bufferDesc.StructureByteStride = 0;

    _ulong* pIndices = new _ulong[_numIndices];
    ZeroMemory(pIndices, sizeof(_ulong) * _numIndices);

    uint32		iNumIndices = 0;

    for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
    {
        pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
        pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
        pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
    }

    ZeroMemory(&_subResourceData, sizeof _subResourceData);
    _subResourceData.pSysMem = pIndices;

    if (FAILED(__super::CreateBuffer(&_indexBuffer)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);
#pragma endregion

    return S_OK;
}

HRESULT Mesh::Initialize(void* pArg)
{
    return S_OK  ;
}

HRESULT Mesh::BindBoneMatrices(Shader* shader, const vector<class Bone*>& bones, const char* constantName)
{
    Matrix boneMatrices[256];
    ::ZeroMemory(boneMatrices, sizeof(Matrix) * 256);

    uint32 index = 0;

    for (auto& boneIndex : _bones)
    {
        ::XMStoreFloat4x4(&boneMatrices[index++],
            ::XMLoadFloat4x4(&_offsetMatrices[index]) * bones[index]->GetCombinedTransformCaculator());
    }
       
    // ���̴��� �ѱ� �� ��Ʈ������ ������ ��Ʈ������ * �θ������ ������ ���� ��ȯ����� ���� �����Ѵ�.

    return shader->BindMatrices(constantName, boneMatrices, _numBones);
}

HRESULT Mesh::ReadyVertexBufferNoneAnim(const aiMesh* mesh, FXMMATRIX pivotMat)
{
    VTXMESH* pVertices = new VTXMESH[_numvertices];
    ZeroMemory(pVertices, sizeof(VTXMESH) * _numvertices);

    for (size_t i = 0; i < _numvertices; i++)
    {
        memcpy(&pVertices[i].position, &mesh->mVertices[i], sizeof(Vec3));
        XMStoreFloat3(&pVertices[i].position, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].position), pivotMat));

        memcpy(&pVertices[i].normal, &mesh->mNormals[i], sizeof(Vec3));
        XMStoreFloat3(&pVertices[i].normal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].normal), pivotMat));

        memcpy(&pVertices[i].texcoord, &mesh->mTextureCoords[0][i], sizeof(Vec2));
        memcpy(&pVertices[i].tangent, &mesh->mTangents[i], sizeof(Vec3));
        memcpy(&pVertices[i].bitangent, &mesh->mBitangents[i], sizeof(Vec3));
    }

    ZeroMemory(&_subResourceData, sizeof _subResourceData);
    _subResourceData.pSysMem = pVertices;

    if (FAILED(__super::CreateBuffer(&_vertexBuffer)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    return S_OK;
}

HRESULT Mesh::ReadyVertexBufferAnim(const aiMesh* mesh, const Model* model)
{
    VTXANIMMESH* pVertices = new VTXANIMMESH[_numvertices];
    ZeroMemory(pVertices, sizeof(VTXANIMMESH) * _numvertices);

    for (size_t i = 0; i < _numvertices; i++)
    {
        memcpy(&pVertices[i].position, &mesh->mVertices[i], sizeof(Vec3));
        memcpy(&pVertices[i].normal, &mesh->mNormals[i], sizeof(Vec3));
        memcpy(&pVertices[i].texcoord, &mesh->mTextureCoords[0][i], sizeof(Vec2));
        memcpy(&pVertices[i].tangent, &mesh->mTangents[i], sizeof(Vec3));
        memcpy(&pVertices[i].bitangent, &mesh->mBitangents[i], sizeof(Vec3));
    }
    
    // �޽��� �̷�� �ִ� ������ ������ �ִ� ���� ����.
    _numBones = mesh->mNumBones;
    
    for (size_t i = 0; i < _numBones; i++)
    {
        // i ��° ���� ����.
        aiBone* bone = mesh->mBones[i];

        // �������� ���Ǵ� ������� ���� �ٸ� ������ �޽ÿ� �����ϱ� ���ؼ��� ������ �ʿ��ϴ�.
        Matrix offSetMatrix;
        ::memcpy(&offSetMatrix, &bone->mOffsetMatrix, sizeof(Matrix));
        ::XMStoreFloat4x4(&offSetMatrix, ::XMMatrixTranspose(::XMLoadFloat4x4(&offSetMatrix)));
        // ��ġ �Ǿ ����Ǿ������Ƿ� �ٽ� ��ġ���ش�.

        _offsetMatrices.push_back(offSetMatrix);

        // ���� �̸������� ���� ��� ���� ��ȸ�� �ڿ� �� �� ���� �ε������� Ȯ���Ѵ�.
        int32 boneIndex = model->GetBoneIndex(bone->mName.data);
        if (-1 == boneIndex)
            return E_FAIL;
        
        // ���� �ε����� ã������ Ǫ�����ش�
        _bones.push_back(boneIndex);

        // �� ���� �� ���� �������� ������ �ִ� �� ������ ����.
        for (size_t j = 0; j < bone->mNumWeights; j++)
        {
            if (0.f == pVertices[bone->mWeights[j].mVertexId].blendWeights.x)
            {
                pVertices[bone->mWeights[j].mVertexId].blendIndices.x = i;
                pVertices[bone->mWeights[j].mVertexId].blendWeights.x = bone->mWeights[j].mWeight;
            }
            else if (0.f == pVertices[bone->mWeights[j].mVertexId].blendWeights.y)
            {
                pVertices[bone->mWeights[j].mVertexId].blendIndices.y = i;
                pVertices[bone->mWeights[j].mVertexId].blendWeights.y = bone->mWeights[j].mWeight;
            }
            else if (0.f == pVertices[bone->mWeights[j].mVertexId].blendWeights.z)
            {
                pVertices[bone->mWeights[j].mVertexId].blendIndices.z = i;
                pVertices[bone->mWeights[j].mVertexId].blendWeights.z = bone->mWeights[j].mWeight;
            }
            else if (0.f == pVertices[bone->mWeights[j].mVertexId].blendWeights.w)
            {
                pVertices[bone->mWeights[j].mVertexId].blendIndices.w = i;
                pVertices[bone->mWeights[j].mVertexId].blendWeights.w = bone->mWeights[j].mWeight;
            }
        }
    }

    // TODO 
    
    // �޽ÿ� ������ �ִ� ������ ������ aIMesh�� mNumBones�� �������.
    // �ش� ���� �޽ø� �̷���ִ� ���� �� ������ ������ �ִ� ���� aIMesh�� mBones �迭�� mNumWeights�� �����ִ�.

    ZeroMemory(&_subResourceData, sizeof _subResourceData);
    _subResourceData.pSysMem = pVertices;

    if (FAILED(__super::CreateBuffer(&_vertexBuffer)))
        return E_FAIL;
    Safe_Delete_Array(pVertices);

    return S_OK;
}

Mesh* Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Model::MODEL_TYPE type, const Model* model, const aiMesh* pAIMesh, FXMMATRIX pivotMat)
{
    Mesh* pInstance = new Mesh(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype(type, model, pAIMesh, pivotMat)))
    {
        MSG_BOX("Failed to Created : CMesh");
        Safe_Release<Mesh*>(pInstance);
        return nullptr;
    }

    return pInstance;
}

Component* Mesh::Clone(void* pArg)
{
    Mesh* pInstance = new Mesh(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CMesh");
        Safe_Release<Mesh*>(pInstance);
        return nullptr;
    }

    return pInstance;
}

void Mesh::Free()
{
    __super::Free();
}

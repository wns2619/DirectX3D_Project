#include "pch.h"
#include "Mesh.h"

Mesh::Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : VIBuffer(pDevice, pContext)
{
}

Mesh::Mesh(const Mesh& rhs)
    : VIBuffer(rhs)

{

}

HRESULT Mesh::InitializePrototype(Model::MODEL_TYPE type, const aiMesh* pAIMesh, FXMMATRIX pivotMat)
{
    _stride = Model::MODEL_TYPE::NONE == type ? sizeof(VTXMESH) : sizeof(VTXANIMMESH);
    
    // 마테리얼의 인덱스 숫자를 저장한다. 이 인덱스의 마테리얼 정보를 이용
    _materialIndex = pAIMesh->mMaterialIndex;
    _numvertices = pAIMesh->mNumVertices;
    _indexStride = 4; /* 인덱스 하나의 크기. 2 or 4 */
    _numIndices = pAIMesh->mNumFaces * 3;
    _indexFormat = _indexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    _topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    _numVBs = 1;

#pragma region VERTEX_BUFFER

    /* 정점버퍼와 인덱스 버퍼를 만드낟. */
    ZeroMemory(&_bufferDesc, sizeof _bufferDesc);

    // m_BufferDesc.ByteWidth = 정점하나의 크기(Byte) * 정점의 갯수;
    _bufferDesc.ByteWidth = _stride * _numvertices;
    _bufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 할당한다. (Lock, unLock 호출 불가)*/
    _bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    _bufferDesc.CPUAccessFlags = 0;
    _bufferDesc.MiscFlags = 0;
    _bufferDesc.StructureByteStride = _stride;

    HRESULT hr = Model::MODEL_TYPE::NONE == type ? ReadyVertexBufferNoneAnim(pAIMesh, pivotMat) : ReadyVertexBufferAnim(pAIMesh, pivotMat);

#pragma endregion

#pragma region INDEX_BUFFER

    /* 정점버퍼와 인덱스 버퍼를 만드낟. */
    ZeroMemory(&_bufferDesc, sizeof _bufferDesc);
    _bufferDesc.ByteWidth = _indexStride * _numIndices;
    _bufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 할당한다. (Lock, unLock 호출 불가)*/
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

HRESULT Mesh::ReadyVertexBufferAnim(const aiMesh* mesh, FXMMATRIX pivotMat)
{
    VTXANIMMESH* pVertices = new VTXANIMMESH[_numvertices];
    ZeroMemory(pVertices, sizeof(VTXANIMMESH) * _numvertices);

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

    // TODO 
    
    // 메시에 영향을 주는 뼈들의 개수는 aIMesh의 mNumBones가 들고있음.
    // 해당 뼈가 메시를 이루고있는 정점 몇 개에게 영향을 주는 지는 aIMesh의 mBones 배열에 mNumWeights가 갖고있다.

    ZeroMemory(&_subResourceData, sizeof _subResourceData);
    _subResourceData.pSysMem = pVertices;

    if (FAILED(__super::CreateBuffer(&_vertexBuffer)))
        return E_FAIL;
    Safe_Delete_Array(pVertices);

    return S_OK;
}

Mesh* Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, Model::MODEL_TYPE type, const aiMesh* pAIMesh, FXMMATRIX pivotMat)
{
    Mesh* pInstance = new Mesh(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype(type, pAIMesh, pivotMat)))
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

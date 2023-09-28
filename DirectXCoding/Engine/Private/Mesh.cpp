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

HRESULT Mesh::InitializePrototype(const aiMesh* pAIMesh, FXMMATRIX pivotMat)
{
    _materialIndex = pAIMesh->mMaterialIndex;
    // 마테리얼의 인덱스 숫자를 저장한다.

    _stride = sizeof(VTXMESH);
    _numvertices = pAIMesh->mNumVertices;
    _indexStride = 4;
    _numIndices = pAIMesh->mNumFaces * 3;
    _indexFormat = _indexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    _topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    _numVBs = 1;

    ::ZeroMemory(&_bufferDesc, sizeof(_bufferDesc));

    _bufferDesc.ByteWidth = _stride * _numvertices;
    _bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    _bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    _bufferDesc.CPUAccessFlags = 0;
    _bufferDesc.MiscFlags = 0;
    _bufferDesc.StructureByteStride = _stride;

    VTXMESH* vertices = new VTXMESH[_numvertices];
    ::ZeroMemory(vertices, sizeof(VTXMESH) * _numvertices);

    for (size_t i = 0; i < _numvertices; i++)
    {
        ::memcpy(&vertices[i].position, &pAIMesh->mVertices[i], sizeof(Vec3));
        ::XMStoreFloat3(&vertices[i].position, ::XMVector3TransformCoord(::XMLoadFloat3(&vertices[i].position), pivotMat));
        ::memcpy(&vertices[i].normal, &pAIMesh->mNormals[i], sizeof(Vec3));
        ::XMStoreFloat3(&vertices[i].normal, ::XMVector3TransformNormal(::XMLoadFloat3(&vertices[i].normal), pivotMat));

        ::memcpy(&vertices[i].texcoord, &pAIMesh->mTextureCoords[0][i], sizeof(Vec2));
        ::memcpy(&vertices[i].tangent, &pAIMesh->mTangents[i], sizeof(Vec3));
    }

    ::ZeroMemory(&_subResourceData, sizeof(_subResourceData));
    _subResourceData.pSysMem = vertices;

    if (FAILED(__super::CreateBuffer(&_vertexBuffer)))
        return E_FAIL;

    Safe_Delete_Array<VTXMESH*>(vertices);

    ::ZeroMemory(&_bufferDesc, sizeof(_bufferDesc));

    _bufferDesc.ByteWidth = _indexStride * _numIndices;
    _bufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 할당한다. (Lock, unLock 호출 불가)*/
    _bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    _bufferDesc.CPUAccessFlags = 0;
    _bufferDesc.MiscFlags = 0;
    _bufferDesc.StructureByteStride = 0;

    uint32* pIndices = new uint32[_numIndices];
    ::ZeroMemory(pIndices, sizeof(uint32) * _numIndices);

    uint32		iNumIndices = 0;

    for (uint32 i = 0; i < pAIMesh->mNumFaces; i++)
    {

      pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
      pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
      pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];

    }

    ::ZeroMemory(&_subResourceData, sizeof _subResourceData);
    _subResourceData.pSysMem = pIndices;

    if (FAILED(__super::CreateBuffer(&_indexBuffer)))
        return E_FAIL;

    Safe_Delete_Array<uint32*>(pIndices);

    return S_OK;
}

HRESULT Mesh::Initialize(void* pArg)
{
    return S_OK  ;
}

Mesh* Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, FXMMATRIX pivotMat)
{
    Mesh* pInstance = new Mesh(pDevice, pContext);

    if (FAILED(pInstance->InitializePrototype(pAIMesh, pivotMat)))
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

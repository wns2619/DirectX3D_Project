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
    // ���׸����� �ε��� ���ڸ� �����Ѵ�.

    _stride = sizeof(VTXMESH); /* �����ϳ��� ũ�� .*/
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

    VTXMESH* pVertices = new VTXMESH[_numvertices];
    ZeroMemory(pVertices, sizeof(VTXMESH) * _numvertices);

    for (size_t i = 0; i < _numvertices; i++)
    {
        memcpy(&pVertices[i].position, &pAIMesh->mVertices[i], sizeof(Vec3));
        XMStoreFloat3(&pVertices[i].position, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].position), pivotMat));

        memcpy(&pVertices[i].normal, &pAIMesh->mNormals[i], sizeof(Vec3));
        XMStoreFloat3(&pVertices[i].normal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].normal), pivotMat));

        memcpy(&pVertices[i].texcoord, &pAIMesh->mTextureCoords[0][i], sizeof(Vec2));
        memcpy(&pVertices[i].tangent, &pAIMesh->mTangents[i], sizeof(Vec3));
        memcpy(&pVertices[i].bitangent, &pAIMesh->mBitangents[i], sizeof(Vec3));
    }

    ZeroMemory(&_subResourceData, sizeof _subResourceData);
    _subResourceData.pSysMem = pVertices;

    if (FAILED(__super::CreateBuffer(&_vertexBuffer)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

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
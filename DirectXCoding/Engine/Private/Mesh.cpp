#include "pch.h"
#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"
#include "Utils.h"

Mesh::Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : VIBuffer(pDevice, pContext)
{
}

Mesh::Mesh(const Mesh& rhs)
    : VIBuffer(rhs)
    , _MeshBufferDesc(rhs._MeshBufferDesc)
    , _materialIndex(rhs._materialIndex)
    , _numBones(rhs._numBones)
    , _szBoneName(rhs._szBoneName)
{

}

HRESULT Mesh::InitializePrototype(Model::MODEL_TYPE type, const Model* model, const aiMesh* pAIMesh, FXMMATRIX pivotMat)
{
    strcpy_s(szName, pAIMesh->mName.data);

    _BufferDesc._stride = Model::MODEL_TYPE::NONE == type ? sizeof(VTXMESH) : sizeof(VTXANIMMESH);
    
    // 마테리얼의 인덱스 숫자를 저장한다. 이 인덱스의 마테리얼 정보를 이용
    _materialIndex = pAIMesh->mMaterialIndex;
    _BufferDesc._numvertices = pAIMesh->mNumVertices;
    _BufferDesc._indexStride = 4; /* 인덱스 하나의 크기. 2 or 4 */
    _BufferDesc._numIndices = pAIMesh->mNumFaces * 3; // 아 여기서 3을 곱했구나.
    _BufferDesc._indexFormat = _BufferDesc._indexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    _BufferDesc._topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    _BufferDesc._numVBs = 1;

#pragma region VERTEX_BUFFER

    /* 정점버퍼와 인덱스 버퍼를 만드낟. */
    ZeroMemory(&_BufferDesc._bufferDesc, sizeof _BufferDesc._bufferDesc);

    // m_BufferDesc.ByteWidth = 정점하나의 크기(Byte) * 정점의 갯수;
    _MeshBufferDesc._vertexBufferDesc.ByteWidth             = _BufferDesc._bufferDesc.ByteWidth = _BufferDesc._stride * _BufferDesc._numvertices;
    _MeshBufferDesc._vertexBufferDesc.Usage                 = _BufferDesc._bufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 할당한다. (Lock, unLock 호출 불가)*/
    _MeshBufferDesc._vertexBufferDesc.BindFlags             = _BufferDesc._bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    _MeshBufferDesc._vertexBufferDesc.CPUAccessFlags        = _BufferDesc._bufferDesc.CPUAccessFlags = 0;
    _MeshBufferDesc._vertexBufferDesc.MiscFlags             = _BufferDesc._bufferDesc.MiscFlags = 0;
    _MeshBufferDesc._vertexBufferDesc.StructureByteStride   = _BufferDesc._bufferDesc.StructureByteStride = _BufferDesc._stride;

    HRESULT hr = Model::MODEL_TYPE::NONE == type ? ReadyVertexBufferNoneAnim(pAIMesh, pivotMat) : ReadyVertexBufferAnim(pAIMesh, model);

#pragma endregion

#pragma region INDEX_BUFFER

    /* 정점버퍼와 인덱스 버퍼를 만드낟. */
    ZeroMemory(&_BufferDesc._bufferDesc, sizeof _BufferDesc._bufferDesc);
    _MeshBufferDesc._indexBufferDesc.ByteWidth           = _BufferDesc._bufferDesc.ByteWidth = _BufferDesc._indexStride * _BufferDesc._numIndices;
    _MeshBufferDesc._indexBufferDesc.Usage               = _BufferDesc._bufferDesc.Usage = D3D11_USAGE_DEFAULT; /* 정적버퍼로 할당한다. (Lock, unLock 호출 불가)*/
    _MeshBufferDesc._indexBufferDesc.BindFlags           = _BufferDesc._bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    _MeshBufferDesc._indexBufferDesc.CPUAccessFlags      = _BufferDesc._bufferDesc.CPUAccessFlags = 0;
    _MeshBufferDesc._indexBufferDesc.MiscFlags           = _BufferDesc._bufferDesc.MiscFlags = 0;
    _MeshBufferDesc._indexBufferDesc.StructureByteStride = _BufferDesc._bufferDesc.StructureByteStride = 0;

    _pIndices = new _ulong[_BufferDesc._numIndices];
    ZeroMemory(_pIndices, sizeof(_ulong) * _BufferDesc._numIndices);

    uint32		iNumIndices = 0;

    for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
    {
        _pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[0];
        _pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[1];
        _pIndices[iNumIndices++] = pAIMesh->mFaces[i].mIndices[2];
    }

    ZeroMemory(&_BufferDesc._subResourceData, sizeof _BufferDesc._subResourceData);
    _BufferDesc._subResourceData.pSysMem = _pIndices;

    if (FAILED(__super::CreateBuffer(&_indexBuffer)))
        return E_FAIL;


#pragma endregion

    return S_OK;
}

HRESULT Mesh::Initialize(void* pArg)
{
    return S_OK  ;
}

HRESULT Mesh::BindBoneMatrices(Shader* shader, const vector<class Bone*>& bones, const char* constantName, FXMMATRIX pivotMatrix)
{
    Matrix boneMatrices[256];
    ::ZeroMemory(boneMatrices, sizeof(Matrix) * 256);

    uint32 index = 0;

    for (auto& boneIndex : _bones)
    {
        ::XMStoreFloat4x4(&boneMatrices[index++],
            ::XMLoadFloat4x4(&_offsetMatrices[index]) * bones[boneIndex]->GetCombinedTransformCaculator() * pivotMatrix);
    }
       
    // 쉐이더에 넘길 본 매트릭스에 오프셋 매트릭스와 * 부모행렬이 곱해진 최종 변환행렬의 곱을 저장한다.

    return shader->BindMatrices(constantName, boneMatrices, _numBones);
}

HRESULT Mesh::ReadyVertexBufferNoneAnim(const aiMesh* mesh, FXMMATRIX pivotMat)
{
    _pVertices = new VTXMESH[_BufferDesc._numvertices];
    ZeroMemory(_pVertices, sizeof(VTXMESH) * _BufferDesc._numvertices);

    _pVertexPos = new Vec3[_BufferDesc._numvertices];

    for (size_t i = 0; i < _BufferDesc._numvertices; i++)
    {
        memcpy(&_pVertices[i].position, &mesh->mVertices[i], sizeof(Vec3));
        XMStoreFloat3(&_pVertices[i].position, XMVector3TransformCoord(XMLoadFloat3(&_pVertices[i].position), pivotMat));

        memcpy(&_pVertices[i].normal, &mesh->mNormals[i], sizeof(Vec3));
        XMStoreFloat3(&_pVertices[i].normal, XMVector3TransformNormal(XMLoadFloat3(&_pVertices[i].normal), pivotMat));

        memcpy(&_pVertices[i].texcoord, &mesh->mTextureCoords[0][i], sizeof(Vec2));
        memcpy(&_pVertices[i].tangent, &mesh->mTangents[i], sizeof(Vec3));
        memcpy(&_pVertices[i].bitangent, &mesh->mBitangents[i], sizeof(Vec3));
       

        _pVertexPos[i] = _pVertices[i].position;
    }

    ZeroMemory(&_BufferDesc._subResourceData, sizeof _BufferDesc._subResourceData);
    _BufferDesc._subResourceData.pSysMem = _pVertices;

    if (FAILED(__super::CreateBuffer(&_vertexBuffer)))
        return E_FAIL;



    return S_OK;
}

HRESULT Mesh::ReadyVertexBufferAnim(const aiMesh* mesh, const Model* model)
{
    _pAnimVertex = new VTXANIMMESH[_BufferDesc._numvertices];
    ZeroMemory(_pAnimVertex, sizeof(VTXANIMMESH) * _BufferDesc._numvertices);

    _pVertexPos = new Vec3[_BufferDesc._numvertices];

    for (size_t i = 0; i < _BufferDesc._numvertices; i++)
    {
        memcpy(&_pAnimVertex[i].position, &mesh->mVertices[i], sizeof(Vec3));
        memcpy(&_pAnimVertex[i].normal, &mesh->mNormals[i], sizeof(Vec3));
        memcpy(&_pAnimVertex[i].texcoord, &mesh->mTextureCoords[0][i], sizeof(Vec2));
        memcpy(&_pAnimVertex[i].tangent, &mesh->mTangents[i], sizeof(Vec3));
        memcpy(&_pAnimVertex[i].bitangent, &mesh->mBitangents[i], sizeof(Vec3));

        _pVertexPos[i] = _pAnimVertex[i].position;
    }
    
    // 메쉬를 이루고 있는 정점에 영향을 주는 뼈의 개수.
    _numBones = mesh->mNumBones;
    
    for (size_t i = 0; i < _numBones; i++)
    {
        // i 번째 뼈에 접근.
        aiBone* bone = mesh->mBones[i];

        // 공통으로 사용되는 뼈대들을 각기 다른 형태의 메시에 적용하기 위해서는 보정이 필요하다.
         Matrix offSetMatrix;
        ::memcpy(&offSetMatrix, &bone->mOffsetMatrix, sizeof(Matrix));
        ::XMStoreFloat4x4(&offSetMatrix, ::XMMatrixTranspose(::XMLoadFloat4x4(&offSetMatrix)));
        // 전치 되어서 저장되어있으므로 다시 전치해준다.

        _offsetMatrices.push_back(offSetMatrix);

        // 뼈의 이름정보를 토대로 모든 뼈를 순회한 뒤에 몇 번 뼈의 인덱스인지 확인한다.
        _szBoneName.push_back(bone->mName.data);
        int32 boneIndex = model->GetBoneIndex(bone->mName.data);
        if (-1 == boneIndex)
            return E_FAIL;
        
        // 뼈의 인덱스를 찾았으면 푸쉬해준다
        _bones.push_back(boneIndex);

        // 이 뼈가 몇 개의 정점에게 영향을 주는 지 포문을 돈다.
        for (size_t j = 0; j < bone->mNumWeights; j++)
        {
             if (0.f == _pAnimVertex[bone->mWeights[j].mVertexId].blendWeights.x)
            {
                _pAnimVertex[bone->mWeights[j].mVertexId].blendIndices.x = i;
                _pAnimVertex[bone->mWeights[j].mVertexId].blendWeights.x = bone->mWeights[j].mWeight;
            }
            else if (0.f == _pAnimVertex[bone->mWeights[j].mVertexId].blendWeights.y)
            {
                _pAnimVertex[bone->mWeights[j].mVertexId].blendIndices.y = i;
                _pAnimVertex[bone->mWeights[j].mVertexId].blendWeights.y = bone->mWeights[j].mWeight;
            }
            else if (0.f == _pAnimVertex[bone->mWeights[j].mVertexId].blendWeights.z)
            {
                _pAnimVertex[bone->mWeights[j].mVertexId].blendIndices.z = i;
                _pAnimVertex[bone->mWeights[j].mVertexId].blendWeights.z = bone->mWeights[j].mWeight;
            }
            else if (0.f == _pAnimVertex[bone->mWeights[j].mVertexId].blendWeights.w)
            {
                _pAnimVertex[bone->mWeights[j].mVertexId].blendIndices.w = i;
                _pAnimVertex[bone->mWeights[j].mVertexId].blendWeights.w = bone->mWeights[j].mWeight;
            }
        }
    }

    if (0 == _numBones)
    {
        _numBones = 1;

        uint32 iIndex = model->GetBoneIndex(szName);
        if (-1 == iIndex)
            return E_FAIL;

        _szBoneName.push_back(szName);
        Matrix offsetMatrix = Matrix::Identity;

        _offsetMatrices.push_back(offsetMatrix);
        _bones.push_back(iIndex);
    }

    // TODO 
    
    // 메시에 영향을 주는 뼈들의 개수는 aIMesh의 mNumBones가 들고있음.
    // 해당 뼈가 메시를 이루고있는 정점 몇 개에게 영향을 주는 지는 aIMesh의 mBones 배열에 mNumWeights가 갖고있다.

    ZeroMemory(&_BufferDesc._subResourceData, sizeof _BufferDesc._subResourceData);
    _BufferDesc._subResourceData.pSysMem = _pAnimVertex;

    if (FAILED(__super::CreateBuffer(&_vertexBuffer)))
        return E_FAIL;
   

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

    _szBoneName.clear();
   
    Safe_Delete_Array<VTXMESH*>(_pVertices);
    Safe_Delete_Array<_ulong*>(_pIndices);
    Safe_Delete_Array<Vec3*>(_pVertexPos);
    Safe_Delete_Array<VTXANIMMESH*>(_pAnimVertex);

}

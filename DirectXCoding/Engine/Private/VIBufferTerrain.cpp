#include "pch.h"
#include "VIBufferTerrain.h"
#include "Transform.h"

VIBufferTerrain::VIBufferTerrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
    : VIBuffer(device, deviceContext)
{
    ::ZeroMemory(&_terrainDesc, sizeof(TERRAIN_DESC));
}
    
VIBufferTerrain::VIBufferTerrain(const VIBufferTerrain& rhs)
    : VIBuffer(rhs), _terrainDesc(rhs._terrainDesc)
{

}

HRESULT VIBufferTerrain::InitializePrototype(const wstring& heightMapPath)
{
    if (heightMapPath == TEXT(""))
        return S_OK;

    _ulong dwByte = 0;
    HANDLE hFile = ::CreateFile(heightMapPath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    BITMAPFILEHEADER fh;
    BITMAPINFOHEADER ih;

    if(!::ReadFile(hFile, &fh, sizeof(fh), &dwByte, nullptr))
        return E_FAIL;

    if (!::ReadFile(hFile, &ih, sizeof(ih), &dwByte, nullptr))
        return E_FAIL;

    _ulong* pixel = nullptr;
    pixel = new _ulong[ih.biWidth * ih.biHeight];
    
    if (!::ReadFile(hFile, pixel, sizeof(_ulong) * ih.biWidth * ih.biHeight, &dwByte, nullptr))
        return E_FAIL;

    if (!::CloseHandle(hFile))
        return E_FAIL;

    _terrainDesc.numVerticesX = ih.biWidth;
    _terrainDesc.numVerticesZ = ih.biHeight;
    _BufferDesc._stride = sizeof(VertexTextureNormalData);
    _BufferDesc._numvertices = _terrainDesc.numVerticesX * _terrainDesc.numVerticesZ;

    _BufferDesc._indexStride = 4;
    _BufferDesc._numIndices = (_terrainDesc.numVerticesX - 1) * (_terrainDesc.numVerticesZ - 1) * 2 * 3;
    _BufferDesc._indexFormat = _BufferDesc._indexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    _BufferDesc._topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    _BufferDesc._numVBs = 1;

    VertexTextureNormalData* vertices = new VertexTextureNormalData[_BufferDesc._numvertices];
    ::ZeroMemory(vertices, sizeof(VertexTextureNormalData) * _BufferDesc._numvertices);

    _pVerticesPos = new Vec3[_BufferDesc._numvertices];

    for(_ulong i = 0; i < _terrainDesc.numVerticesZ; ++i)
        for (_ulong j = 0; j < _terrainDesc.numVerticesX; ++j)
        {
            _ulong index = i * _terrainDesc.numVerticesX + j;

            vertices[index].position = Vec3(j, _float(pixel[index] & 0x000000ff) / 10.f, i);
            vertices[index].normal = Vec3(0.f, 0.f, 0.f);
            vertices[index].uv = Vec2(j / (_terrainDesc.numVerticesX - 1.f), i / (_terrainDesc.numVerticesZ - 1.f));

            _pVerticesPos[index] = vertices[index].position;
        }

    Safe_Delete_Array<_ulong*>(pixel);


    uint32* indices = new uint32[_BufferDesc._numIndices];
    ::ZeroMemory(indices, sizeof(uint32) * _BufferDesc._numIndices);

    uint32 tricount = 0;

    for(uint32 i = 0; i < _terrainDesc.numVerticesZ - 1; ++i)
        for (uint32 j = 0; j < _terrainDesc.numVerticesX - 1; ++j)
        {
            uint32 Index = i * _terrainDesc.numVerticesX + j;

            uint32 indexArray[4] = {
                Index + _terrainDesc.numVerticesX, // 5
                Index + _terrainDesc.numVerticesX + 1, // 6
                Index + 1, // 1
                Index // 0
            };

            Vec4		vSourDir, vDestDir, vNormal;

            indices[tricount++] = indexArray[0];
            indices[tricount++] = indexArray[1];
            indices[tricount++] = indexArray[2];

            vSourDir = Vec4(vertices[indexArray[1]].position - vertices[indexArray[0]].position);

            vDestDir = Vec4(vertices[indexArray[2]].position - vertices[indexArray[1]].position);

            vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

           vertices[indexArray[0]].normal = vertices[indexArray[0]].normal + vNormal;
           vertices[indexArray[1]].normal = vertices[indexArray[1]].normal + vNormal;
           vertices[indexArray[2]].normal = vertices[indexArray[2]].normal + vNormal;

           indices[tricount++] = indexArray[0];
           indices[tricount++] = indexArray[2];
           indices[tricount++] = indexArray[3];

            vSourDir = Vec4(vertices[indexArray[2]].position - vertices[indexArray[0]].position);

            vDestDir = Vec4(vertices[indexArray[3]].position - vertices[indexArray[2]].position);

            vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

            vertices[indexArray[0]].normal = vertices[indexArray[0]].normal + vNormal;
            vertices[indexArray[2]].normal = vertices[indexArray[2]].normal + vNormal;
            vertices[indexArray[3]].normal = vertices[indexArray[3]].normal + vNormal;

        }

    // Vertex

    ::ZeroMemory(&_BufferDesc._bufferDesc, sizeof(_BufferDesc._bufferDesc));
    {
        _BufferDesc._bufferDesc.ByteWidth = _BufferDesc._stride * _BufferDesc._numvertices;
        _BufferDesc._bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        _BufferDesc._bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        _BufferDesc._bufferDesc.CPUAccessFlags = 0;
        _BufferDesc._bufferDesc.MiscFlags = 0;
        _BufferDesc._bufferDesc.StructureByteStride = _BufferDesc._stride;
    }

    ::ZeroMemory(&_BufferDesc._subResourceData, sizeof(_BufferDesc._subResourceData));
    _BufferDesc._subResourceData.pSysMem = vertices;

    if (FAILED(__super::CreateBuffer(&_vertexBuffer)))
        return E_FAIL;

    // Index

    ::ZeroMemory(&_BufferDesc._bufferDesc, sizeof(_BufferDesc._bufferDesc));
    {
        _BufferDesc._bufferDesc.ByteWidth = _BufferDesc._indexStride * _BufferDesc._numIndices;
        _BufferDesc._bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        _BufferDesc._bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        _BufferDesc._bufferDesc.CPUAccessFlags = 0;
        _BufferDesc._bufferDesc.MiscFlags = 0;
        _BufferDesc._bufferDesc.StructureByteStride = 0;
    }
    //D3D11_RASTERIZER_DESC

    ::ZeroMemory(&_BufferDesc._subResourceData, sizeof(_BufferDesc._subResourceData));
    _BufferDesc._subResourceData.pSysMem = indices;

    if (FAILED(__super::CreateBuffer(&_indexBuffer)))
        return E_FAIL;

 
    Safe_Delete_Array<VertexTextureNormalData*>(vertices);
    Safe_Delete_Array<uint32*>(indices);

    int a = 0;

    return S_OK;
}

HRESULT VIBufferTerrain::Initialize(void* argument)
{
    if (nullptr != argument)
    {
        TERRAIN_DESC* terraininfo = static_cast<TERRAIN_DESC*>(argument);
        _terrainDesc.numVerticesX = terraininfo->numVerticesX;
        _terrainDesc.numVerticesZ = terraininfo->numVerticesZ;

        _BufferDesc._stride = sizeof(VertexTextureNormalData);
        _BufferDesc._numvertices = _terrainDesc.numVerticesX * _terrainDesc.numVerticesZ;

        _BufferDesc._indexStride = 4;
        _BufferDesc._numIndices = (_terrainDesc.numVerticesX - 1) * (_terrainDesc.numVerticesZ - 1) * 2 * 3;
        _BufferDesc._indexFormat = _BufferDesc._indexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        _BufferDesc._topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        _BufferDesc._numVBs = 1;

        VertexTextureNormalData* vertices = new VertexTextureNormalData[_BufferDesc._numvertices];
        ::ZeroMemory(vertices, sizeof(VertexTextureNormalData) * _BufferDesc._numvertices);

        _pVerticesPos = new Vec3[_BufferDesc._numvertices];

        for (_ulong i = 0; i < _terrainDesc.numVerticesZ; ++i)
            for (_ulong j = 0; j < _terrainDesc.numVerticesX; ++j)
            {
                _ulong index = i * _terrainDesc.numVerticesX + j;

       
                vertices[index].position = Vec3(_float(j), _float(0.f) / 10.f, _float(i));
                vertices[index].normal = Vec3(0.f, 0.f, 0.f);
                vertices[index].uv = Vec2(j / (_terrainDesc.numVerticesX - 1.f), i / (_terrainDesc.numVerticesZ - 1.f));
           
                _pVerticesPos[index] = vertices[index].position;
            }

        uint32* indices = new uint32[_BufferDesc._numIndices];
        ::ZeroMemory(indices, sizeof(uint32) * _BufferDesc._numIndices);

        uint32 tricount = 0;

        for (uint32 i = 0; i < _terrainDesc.numVerticesZ - 1; ++i)
            for (uint32 j = 0; j < _terrainDesc.numVerticesX - 1; ++j)
            {
                uint32 Index = i * _terrainDesc.numVerticesX + j;

                uint32 indexArray[4] = {
                    Index + _terrainDesc.numVerticesX, // 5
                    Index + _terrainDesc.numVerticesX + 1, // 6
                    Index + 1, // 1
                    Index // 0
                };


                Vec4		vSourDir, vDestDir, vNormal;

                indices[tricount++] = indexArray[0];
                indices[tricount++] = indexArray[1];
                indices[tricount++] = indexArray[2];

                vSourDir = Vec4(vertices[indexArray[1]].position - vertices[indexArray[0]].position);

                vDestDir = Vec4(vertices[indexArray[2]].position - vertices[indexArray[1]].position);

                vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

                vertices[indexArray[0]].normal = vertices[indexArray[0]].normal + vNormal;
                vertices[indexArray[1]].normal = vertices[indexArray[1]].normal + vNormal;
                vertices[indexArray[2]].normal = vertices[indexArray[2]].normal + vNormal;

                indices[tricount++] = indexArray[0];
                indices[tricount++] = indexArray[2];
                indices[tricount++] = indexArray[3];

                vSourDir = Vec4(vertices[indexArray[2]].position - vertices[indexArray[0]].position);

                vDestDir = Vec4(vertices[indexArray[3]].position - vertices[indexArray[2]].position);

                vNormal = XMVector3Normalize(XMVector3Cross(vSourDir, vDestDir));

                vertices[indexArray[0]].normal = vertices[indexArray[0]].normal + vNormal;
                vertices[indexArray[2]].normal = vertices[indexArray[2]].normal + vNormal;
                vertices[indexArray[3]].normal = vertices[indexArray[3]].normal + vNormal;
            }

        // Vertex

        ::ZeroMemory(&_BufferDesc._bufferDesc, sizeof(_BufferDesc._bufferDesc));
        {
            _BufferDesc._bufferDesc.ByteWidth = _BufferDesc._stride * _BufferDesc._numvertices;
            _BufferDesc._bufferDesc.Usage = D3D11_USAGE_DEFAULT;
            _BufferDesc._bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            _BufferDesc._bufferDesc.CPUAccessFlags = 0;
            _BufferDesc._bufferDesc.MiscFlags = 0;
            _BufferDesc._bufferDesc.StructureByteStride = _BufferDesc._stride;
        }

        ::ZeroMemory(&_BufferDesc._subResourceData, sizeof(_BufferDesc._subResourceData));
        _BufferDesc._subResourceData.pSysMem = vertices;

        if (FAILED(__super::CreateBuffer(&_vertexBuffer)))
            return E_FAIL;

        // Index

        ::ZeroMemory(&_BufferDesc._bufferDesc, sizeof(_BufferDesc._bufferDesc));
        {
            _BufferDesc._bufferDesc.ByteWidth = _BufferDesc._indexStride * _BufferDesc._numIndices;
            _BufferDesc._bufferDesc.Usage = D3D11_USAGE_DEFAULT;
            _BufferDesc._bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            _BufferDesc._bufferDesc.CPUAccessFlags = 0;
            _BufferDesc._bufferDesc.MiscFlags = 0;
            _BufferDesc._bufferDesc.StructureByteStride = 0;
        }
        //D3D11_RASTERIZER_DESC

        ::ZeroMemory(&_BufferDesc._subResourceData, sizeof(_BufferDesc._subResourceData));
        _BufferDesc._subResourceData.pSysMem = indices;

        if (FAILED(__super::CreateBuffer(&_indexBuffer)))
            return E_FAIL;


        Safe_Delete_Array<VertexTextureNormalData*>(vertices);
        Safe_Delete_Array<uint32*>(indices);
    }


    return S_OK;
}

XMVECTOR VIBufferTerrain::SetUp_OnTerrain(Transform* pTerrainTransform, FXMVECTOR vWorldPos)
{
    /*vWorldPos *  지형 월드 행렬의 역 => vWorldPos의 위치를 지형의 로컬 스페이스 상의 정보로 변환한다. */
    XMVECTOR		vLocalPos = XMVector3TransformCoord(vWorldPos, pTerrainTransform->GetInverseMatrixCaculator());

    /* 현재 플레이어가 존재하고 있는 네모 영역의 왼쪽 하단 정점의 인덱스를 구했다. */
    uint32		iIndex = static_cast<uint32>((XMVectorGetZ(vLocalPos))) * 
        _terrainDesc.numVerticesX + static_cast<uint32>((XMVectorGetX(vLocalPos)));

    uint32		iIndices[4] = {
        iIndex + _terrainDesc.numVerticesX,
        iIndex + _terrainDesc.numVerticesX + 1,
        iIndex + 1,
        iIndex
    };

    _float		fWidth = XMVectorGetX(vLocalPos) - _pVerticesPos[iIndices[0]].x;
    _float		fDepth = _pVerticesPos[iIndices[0]].z - XMVectorGetZ(vLocalPos);

    XMVECTOR		vPlane;

    /* 오른쪽 위 삼각형 */
    if (fWidth >= fDepth)
    {
        vPlane = XMPlaneFromPoints(XMLoadFloat3(&_pVerticesPos[iIndices[0]]),
            XMLoadFloat3(&_pVerticesPos[iIndices[1]]),
            XMLoadFloat3(&_pVerticesPos[iIndices[2]]));
    }
    /* 왼쪽 하단 삼각형 */
    else
    {
        vPlane = XMPlaneFromPoints(XMLoadFloat3(&_pVerticesPos[iIndices[0]]),
            XMLoadFloat3(&_pVerticesPos[iIndices[2]]),
            XMLoadFloat3(&_pVerticesPos[iIndices[3]]));
    }

    /* 평면 방정식에 a, b, c, d 다 구했다. */
    /* 내 위치 x, y, z */
    // ax + by + cz + d = 0
    // y = (-ax - cz - d) / b

    /* 평면 상에 존재할 수 있도록하는 y를 구하자. */
    /* 평면상에 존재하면 되는 것이기 때문에 x, z는 변할 이유가 없다. */
    _float	fY = ((-XMVectorGetX(vPlane) * XMVectorGetX(vLocalPos) - (XMVectorGetZ(vPlane) * XMVectorGetZ(vLocalPos)) - XMVectorGetW(vPlane))) / XMVectorGetY(vPlane);

    vLocalPos = XMVectorSetY(vLocalPos, fY);

    return XMVector3TransformCoord(vLocalPos, pTerrainTransform->GetWorldMatrixCaculator());
}


VIBufferTerrain* VIBufferTerrain::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wstring& heightMapPath)
{
    VIBufferTerrain* terrain = new VIBufferTerrain(device, deviceContext);

    if (FAILED(terrain->InitializePrototype(heightMapPath)))
    {
        MSG_BOX("Failed to Created : VIBuffer Terrain");
        Safe_Release<VIBufferTerrain*>(terrain);
    }

    return terrain;
}

Component* VIBufferTerrain::Clone(void* argument)
{
    VIBufferTerrain* terrain = new VIBufferTerrain(*this);

    if (FAILED(terrain->Initialize(argument)))
    {
        MSG_BOX("Failed to Cloned : VIBuffer Terrain");
        Safe_Release<VIBufferTerrain*>(terrain);
    }

    return terrain;
}

void VIBufferTerrain::Free()
{
    __super::Free();
}

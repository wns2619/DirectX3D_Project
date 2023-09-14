#include "pch.h"
#include "VIBufferTerrain.h"

VIBufferTerrain::VIBufferTerrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
    : VIBuffer(device, deviceContext)
{
}

VIBufferTerrain::VIBufferTerrain(const VIBufferTerrain& rhs)
    : VIBuffer(rhs)
{

}

HRESULT VIBufferTerrain::InitializePrototype(const wstring& heightMapPath)
{
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

    _numVerticesX = ih.biWidth;
    _numVerticesZ = ih.biHeight;
    _stride = sizeof(VertexTextureNormalData);
    _numvertices = _numVerticesX * _numVerticesZ;

    _indexStride = 4;
    _numIndices = (_numVerticesX - 1) * (_numVerticesZ - 1) * 2 * 3;
    _indexFormat = _indexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    _topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    _numVBs = 1;

    ::ZeroMemory(&_bufferDesc, sizeof(_bufferDesc));
    {
        _bufferDesc.ByteWidth = _stride * _numvertices;
        _bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        _bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        _bufferDesc.CPUAccessFlags = 0;
        _bufferDesc.MiscFlags = 0;
        _bufferDesc.StructureByteStride = _stride;
    }

    VertexTextureNormalData* vertices = new VertexTextureNormalData[_numvertices];
    ::ZeroMemory(vertices, sizeof(VertexTextureNormalData) * _numvertices);

    for(_ulong i = 0; i < _numVerticesZ; ++i)
        for (_ulong j = 0; j < _numVerticesX; ++j)
        {
            _ulong index = i * _numVerticesX + j;

            vertices[index].position = Vec3(_float(j), _float(pixel[index] & 0x000000ff), _float(i));
            vertices[index].normal = Vec3(0.f, 1.f, 0.f);
            vertices[index].uv = Vec2(_float(j) / (_numVerticesX - 1) * 20.f, _float(i) / (_numVerticesZ - 1) * 20.f);

        }

    ::ZeroMemory(&_subResourceData, sizeof(_subResourceData));
    _subResourceData.pSysMem = vertices;

    if (FAILED(__super::CreateBuffer(&_vertexBuffer)))
        return E_FAIL;

    Safe_Delete_Array<VertexTextureNormalData*>(vertices);

    ::ZeroMemory(&_bufferDesc, sizeof(_bufferDesc));
    {
        _bufferDesc.ByteWidth = _indexStride * _numIndices;
        _bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        _bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        _bufferDesc.CPUAccessFlags = 0;
        _bufferDesc.MiscFlags = 0;
        _bufferDesc.StructureByteStride = 0;
    }

    _ulong* indices = new _ulong[_numIndices];
    ::ZeroMemory(indices, sizeof(_ulong) * _numIndices);

    _ulong tricount = 0;

    for(_ulong i = 0; i < _numVerticesZ - 1; ++i)
        for (_ulong j = 0; j < _numVerticesX - 1; ++j)
        {
            indices[tricount] = i * _numVerticesX + j;
            indices[tricount + 1] = i * _numVerticesX + j + 1;
            indices[tricount + 2] = (i + 1) * _numVerticesX + j;

            indices[tricount + 3] = (i + 1) * _numVerticesX + j;
            indices[tricount + 4] = i * _numVerticesX + j + 1;
            indices[tricount + 5] = (i + 1) * _numVerticesX + j + 1;

            tricount += 6;
        }


    ::ZeroMemory(&_subResourceData, sizeof(_subResourceData));
    _subResourceData.pSysMem = indices;

    if (FAILED(__super::CreateBuffer(&_indexBuffer)))
        return E_FAIL;

    Safe_Delete_Array<_ulong*>(indices);

    Safe_Delete_Array<_ulong*>(pixel);

    return S_OK;
}

HRESULT VIBufferTerrain::Initialize(void* argument)
{
    return S_OK;
}

VIBufferTerrain* VIBufferTerrain::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wstring& heightMapPath)
{
    VIBufferTerrain* terrain = new VIBufferTerrain(device, deviceContext);

    if (FAILED(terrain->InitializePrototype(heightMapPath)))
    {
        MSG_BOX("Failed to Created : VIBuffer Terrain");
        Safe_Release<VIBufferTerrain*>(terrain);
        return nullptr;
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
        return nullptr;
    }

    return terrain;
}

void VIBufferTerrain::Free()
{
    __super::Free();
}

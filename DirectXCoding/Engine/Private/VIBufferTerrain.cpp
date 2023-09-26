#include "pch.h"
#include "VIBufferTerrain.h"
#include "Transform.h"
#include "CameraHelper.h"

VIBufferTerrain::VIBufferTerrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
    : VIBuffer(device, deviceContext)
{
}
    
VIBufferTerrain::VIBufferTerrain(const VIBufferTerrain& rhs)
    : VIBuffer(rhs), _numVerticesX(rhs._numVerticesX), _numVerticesZ(rhs._numVerticesZ)
{
    _vertices = new VertexTextureNormalData[_numvertices];

    // 데이터를 복사
    for (int i = 0; i < _numvertices; ++i)
        _vertices[i] = rhs._vertices[i];

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

    _vertices = new VertexTextureNormalData[_numvertices];
    ::ZeroMemory(_vertices, sizeof(VertexTextureNormalData) * _numvertices);

    for(_ulong i = 0; i < _numVerticesZ; ++i)
        for (_ulong j = 0; j < _numVerticesX; ++j)
        {
            _ulong index = i * _numVerticesX + j;

            _vertices[index].position = Vec3(_float(j), _float(pixel[index] & 0x000000ff) / 10.f, _float(i));
            _vertices[index].normal = Vec3(0.f, 0.f, 0.f);
            _vertices[index].uv = Vec2(_float(j) / (_numVerticesX - 1), _float(i) / (_numVerticesZ - 1));

        }

    Safe_Delete_Array<_ulong*>(pixel);


    uint32* indices = new uint32[_numIndices];
    ::ZeroMemory(indices, sizeof(uint32) * _numIndices);

    uint32 tricount = 0;

    for(uint32 i = 0; i < _numVerticesZ - 1; ++i)
        for (uint32 j = 0; j < _numVerticesX - 1; ++j)
        {
            uint32 Index = i * _numVerticesX + j;

            uint32 indexArray[4] = {
                Index + _numVerticesX, // 5
                Index + _numVerticesX + 1, // 6
                Index + 1, // 1
                Index // 0
            };

            XMVECTOR sourDirection, destDirection, normal;

            indices[tricount++] = indexArray[0];
            indices[tricount++] = indexArray[1];
            indices[tricount++] = indexArray[2];

            sourDirection = ::XMLoadFloat3(&_vertices[indexArray[1]].position) -
                ::XMLoadFloat3(&_vertices[indexArray[0]].position);

            destDirection = ::XMLoadFloat3(&_vertices[indexArray[2]].position) -
                ::XMLoadFloat3(&_vertices[indexArray[1]].position);

            normal = ::XMVector3Normalize(::XMVector3Cross(sourDirection, destDirection));

            ::XMStoreFloat3(&_vertices[indexArray[0]].normal,
                ::XMVector3Normalize(::XMLoadFloat3(&_vertices[indexArray[0]].normal) + normal));

            ::XMStoreFloat3(&_vertices[indexArray[1]].normal,
                ::XMVector3Normalize(::XMLoadFloat3(&_vertices[indexArray[1]].normal) + normal));

            ::XMStoreFloat3(&_vertices[indexArray[2]].normal,
                ::XMVector3Normalize(::XMLoadFloat3(&_vertices[indexArray[2]].normal) + normal));

            indices[tricount++] = indexArray[0];
            indices[tricount++] = indexArray[2];
            indices[tricount++] = indexArray[3];

            sourDirection = ::XMLoadFloat3(&_vertices[indexArray[2]].position) -
                ::XMLoadFloat3(&_vertices[indexArray[0]].position);

            destDirection = ::XMLoadFloat3(&_vertices[indexArray[3]].position) -
                ::XMLoadFloat3(&_vertices[indexArray[2]].position);

            normal = ::XMVector3Normalize(::XMVector3Cross(sourDirection, destDirection));

            ::XMStoreFloat3(&_vertices[indexArray[0]].normal,
                ::XMVector3Normalize(::XMLoadFloat3(&_vertices[indexArray[0]].normal) + normal));

            ::XMStoreFloat3(&_vertices[indexArray[2]].normal,
                ::XMVector3Normalize(::XMLoadFloat3(&_vertices[indexArray[2]].normal) + normal));

            ::XMStoreFloat3(&_vertices[indexArray[3]].normal,
                ::XMVector3Normalize(::XMLoadFloat3(&_vertices[indexArray[3]].normal) + normal));
        }

    // Vertex

    ::ZeroMemory(&_bufferDesc, sizeof(_bufferDesc));
    {
        _bufferDesc.ByteWidth = _stride * _numvertices;
        _bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        _bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        _bufferDesc.CPUAccessFlags = 0;
        _bufferDesc.MiscFlags = 0;
        _bufferDesc.StructureByteStride = _stride;
    }

    ::ZeroMemory(&_subResourceData, sizeof(_subResourceData));
    _subResourceData.pSysMem = _vertices;

    if (FAILED(__super::CreateBuffer(&_vertexBuffer)))
        return E_FAIL;

    // Index

    ::ZeroMemory(&_bufferDesc, sizeof(_bufferDesc));
    {
        _bufferDesc.ByteWidth = _indexStride * _numIndices;
        _bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        _bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        _bufferDesc.CPUAccessFlags = 0;
        _bufferDesc.MiscFlags = 0;
        _bufferDesc.StructureByteStride = 0;
    }


    ::ZeroMemory(&_subResourceData, sizeof(_subResourceData));
    _subResourceData.pSysMem = indices;

    if (FAILED(__super::CreateBuffer(&_indexBuffer)))
        return E_FAIL;

 
    Safe_Delete_Array<uint32*>(indices);

    int a = 0;

    return S_OK;
}

HRESULT VIBufferTerrain::Initialize(void* argument)
{
    if (nullptr != argument)
    {

        TERRAIN_DESC* desc = static_cast<TERRAIN_DESC*>(argument);

        _numVerticesX = desc->numVerticesX;
        _numVerticesZ = desc->numVerticesZ;

        _stride = sizeof(VertexTextureNormalData);
        _numvertices = _numVerticesX * _numVerticesZ;

        _indexStride = 4;
        _numIndices = (_numVerticesX - 1) * (_numVerticesZ - 1) * 2 * 3;
        _indexFormat = _indexStride == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        _topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        _numVBs = 1;

        VertexTextureNormalData* vertices = new VertexTextureNormalData[_numvertices];
        ::ZeroMemory(vertices, sizeof(VertexTextureNormalData) * _numvertices);

        for (_ulong i = 0; i < _numVerticesZ; ++i)
            for (_ulong j = 0; j < _numVerticesX; ++j)
            {
                _ulong index = i * _numVerticesX + j;

                vertices[index].position = Vec3(_float(j), 0.f, _float(i));
                vertices[index].normal = Vec3(0.f, 0.f, 0.f);
                vertices[index].uv = Vec2(_float(j) / (_numVerticesX - 1), _float(i) / (_numVerticesZ - 1));

            }

        //Safe_Delete_Array<_ulong*>(pixel);


        uint32* indices = new uint32[_numIndices];
        ::ZeroMemory(indices, sizeof(uint32) * _numIndices);

        uint32 tricount = 0;

        for (uint32 i = 0; i < _numVerticesZ - 1; ++i)
            for (uint32 j = 0; j < _numVerticesX - 1; ++j)
            {
                uint32 Index = i * _numVerticesX + j;

                uint32 indexArray[4] = {
                    Index + _numVerticesX, // 5
                    Index + _numVerticesX + 1, // 6
                    Index + 1, // 1
                    Index // 0
                };

                XMVECTOR sourDirection, destDirection, normal;

                indices[tricount++] = indexArray[0];
                indices[tricount++] = indexArray[1];
                indices[tricount++] = indexArray[2];

                sourDirection = ::XMLoadFloat3(&vertices[indexArray[1]].position) -
                    ::XMLoadFloat3(&vertices[indexArray[0]].position);

                destDirection = ::XMLoadFloat3(&vertices[indexArray[2]].position) -
                    ::XMLoadFloat3(&vertices[indexArray[1]].position);

                normal = ::XMVector3Normalize(::XMVector3Cross(sourDirection, destDirection));

                ::XMStoreFloat3(&vertices[indexArray[0]].normal,
                    ::XMVector3Normalize(::XMLoadFloat3(&vertices[indexArray[0]].normal) + normal));

                ::XMStoreFloat3(&vertices[indexArray[1]].normal,
                    ::XMVector3Normalize(::XMLoadFloat3(&vertices[indexArray[1]].normal) + normal));

                ::XMStoreFloat3(&vertices[indexArray[2]].normal,
                    ::XMVector3Normalize(::XMLoadFloat3(&vertices[indexArray[2]].normal) + normal));

                indices[tricount++] = indexArray[0];
                indices[tricount++] = indexArray[2];
                indices[tricount++] = indexArray[3];

                sourDirection = ::XMLoadFloat3(&vertices[indexArray[2]].position) -
                    ::XMLoadFloat3(&vertices[indexArray[0]].position);

                destDirection = ::XMLoadFloat3(&vertices[indexArray[3]].position) -
                    ::XMLoadFloat3(&vertices[indexArray[2]].position);

                normal = ::XMVector3Normalize(::XMVector3Cross(sourDirection, destDirection));

                ::XMStoreFloat3(&vertices[indexArray[0]].normal,
                    ::XMVector3Normalize(::XMLoadFloat3(&vertices[indexArray[0]].normal) + normal));

                ::XMStoreFloat3(&vertices[indexArray[2]].normal,
                    ::XMVector3Normalize(::XMLoadFloat3(&vertices[indexArray[2]].normal) + normal));

                ::XMStoreFloat3(&vertices[indexArray[3]].normal,
                    ::XMVector3Normalize(::XMLoadFloat3(&vertices[indexArray[3]].normal) + normal));
            }

        // Vertex

        ::ZeroMemory(&_bufferDesc, sizeof(_bufferDesc));
        {
            _bufferDesc.ByteWidth = _stride * _numvertices;
            _bufferDesc.Usage = D3D11_USAGE_DEFAULT;
            _bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            _bufferDesc.CPUAccessFlags = 0;
            _bufferDesc.MiscFlags = 0;
            _bufferDesc.StructureByteStride = _stride;
        }

        ::ZeroMemory(&_subResourceData, sizeof(_subResourceData));
        _subResourceData.pSysMem = vertices;
   

        if (FAILED(__super::CreateBuffer(&_vertexBuffer)))
            return E_FAIL;

        // Index

        ::ZeroMemory(&_bufferDesc, sizeof(_bufferDesc));
        {
            _bufferDesc.ByteWidth = _indexStride * _numIndices;
            _bufferDesc.Usage = D3D11_USAGE_DEFAULT;
            _bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            _bufferDesc.CPUAccessFlags = 0;
            _bufferDesc.MiscFlags = 0;
            _bufferDesc.StructureByteStride = 0;
        }


        ::ZeroMemory(&_subResourceData, sizeof(_subResourceData));
        _subResourceData.pSysMem = indices;



        if (FAILED(__super::CreateBuffer(&_indexBuffer)))
            return E_FAIL;

        Safe_Delete_Array<VertexTextureNormalData*>(vertices);
        Safe_Delete_Array<uint32*>(indices);
        int a = 0;
    }


    return S_OK;
}

_bool VIBufferTerrain::Pick(int32 screenX, int32 screenY, Vec3& pickPos, _float& distance, 
    Transform& trans, _float& winSizeX, _float& winSizeY)
{
    Matrix W = trans.GetWorldMatrix();
    Matrix V = CameraHelper::GetInstance()->GetTransform(CameraHelper::TRANSFORMSTATE::D3DTS_VIEW);
    Matrix P = CameraHelper::GetInstance()->GetTransform(CameraHelper::TRANSFORMSTATE::D3DTS_PROJ);
    // 플레이어 월드행렬, 카메라 행렬, 투영행렬 

    Vec3 viewNear = Engine::Unproject(Vec3(screenX, screenY, 0), W, V, P, winSizeX, winSizeY);
    Vec3 viewFar = Engine::Unproject(Vec3(screenX, screenY, 1), W, V, P, winSizeX, winSizeY);

    Vec3 start = viewNear;
    Vec3 direction = viewFar - viewNear;
    direction.Normalize();

    Ray ray = Ray(start, direction);

    const auto& vertices = _vertices;

    for (int32 z = 0; z < _numVerticesZ; z++)
    {
        for (int32 x = 0; x < _numVerticesX; x++)
        {
            uint32 Index = z * _numVerticesX + x;

            uint32 indexArray[4] = {
                Index + _numVerticesX, // 5
                Index + _numVerticesX + 1, // 6
                Index + 1, // 1
                Index // 0
            };

            Vec3 p[4];
            for (int32 i = 0; i < 4; ++i)
                p[i] = vertices[indexArray[i]].position;

            if (ray.Intersects(p[0], p[1], p[2], OUT distance))
            {
                pickPos = ray.position + ray.direction * distance;
                return true;
            }

            if (ray.Intersects(p[3], p[1], p[2], OUT distance))
            {
                pickPos = ray.position + ray.direction * distance;
                return true;
            }
        }
    }

    return false;
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
    Safe_Delete_Array<VertexTextureNormalData*>(_vertices);

    __super::Free();
}

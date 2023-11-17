#include "pch.h"
#include "VIBufferInstancing.h"

VIBufferInstancing::VIBufferInstancing(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : VIBuffer(pDevice, pDeviceContext)
{
}

VIBufferInstancing::VIBufferInstancing(const VIBufferInstancing& rhs)
    : VIBuffer(rhs)
{
}

HRESULT VIBufferInstancing::InitializePrototype()
{
    _iStrideInstance = sizeof(VTXINSTANCE);

    ::ZeroMemory(&_BufferDesc._bufferDesc, sizeof(_BufferDesc._bufferDesc));

    _BufferDesc._bufferDesc.ByteWidth = _iStrideInstance * _iNumInstance;
    _BufferDesc._bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    _BufferDesc._bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    _BufferDesc._bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    _BufferDesc._bufferDesc.MiscFlags = 0;
    _BufferDesc._bufferDesc.StructureByteStride = _iStrideInstance;

    VTXINSTANCE* pVertices = new VTXINSTANCE[_iNumInstance];
    ::ZeroMemory(pVertices, sizeof(VTXINSTANCE) * _iNumInstance);

    for (uint32 i = 0; i < _iNumInstance; ++i)
    {
        pVertices[i].vRight = Vec4(1.f, 0.f, 0.f, 0.f);
        pVertices[i].vUp = Vec4(0.f, 1.f, 0.f, 0.f);
        pVertices[i].vLook = Vec4(0.f, 0.f, 1.f, 0.f);
        pVertices[i].vTranslation = Vec4(rand() % 5, rand() % 5, rand() % 5, 1.f);
    }

    ::ZeroMemory(&_BufferDesc._subResourceData, sizeof(_BufferDesc._subResourceData));
    _BufferDesc._subResourceData.pSysMem = pVertices;

    if (FAILED(__super::CreateBuffer(&_pVBInstance)))
        return E_FAIL;

    Safe_Delete_Array<VTXINSTANCE*>(pVertices);

    return S_OK;
}

HRESULT VIBufferInstancing::Initialize(void* argument)
{
    return S_OK;
}

HRESULT VIBufferInstancing::Render()
{
    ID3D11Buffer* pVertexBuffers[] = {
        _vertexBuffer,
        _pVBInstance
    };

    uint32 iStrides[] = {
        _BufferDesc._stride,
        _iStrideInstance
    };

    uint32 iOffset[] = {
        0,
        0
    };

    _deviceContext->IASetVertexBuffers(0, _BufferDesc._numVBs, pVertexBuffers, iStrides, iOffset);

    _deviceContext->IASetIndexBuffer(_indexBuffer, _BufferDesc._indexFormat, 0);

    _deviceContext->IASetPrimitiveTopology(_BufferDesc._topology);

    _deviceContext->DrawIndexedInstanced(_iNumIndicesPerInstance, _iNumInstance, 0, 0, 0);

    return S_OK;
}


void VIBufferInstancing::Free()
{
    __super::Free();

    Safe_Release<ID3D11Buffer*>(_pVBInstance);
}

#include "pch.h"
#include "VIBufferInstancing.h"

VIBufferInstancing::VIBufferInstancing(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : VIBuffer(pDevice, pDeviceContext)
{
}

VIBufferInstancing::VIBufferInstancing(const VIBufferInstancing& rhs)
    : VIBuffer(rhs)
    , _iStrideInstance(rhs._iStrideInstance)
    , _iNumInstance(rhs._iNumInstance)
    , _iNumIndicesPerInstance(rhs._iNumIndicesPerInstance)
    , _pInstanceVertices(rhs._pInstanceVertices)
    , _pSpeeds(rhs._pSpeeds)
    , _pLifeTimes(rhs._pLifeTimes)
    , _pTimeAccs(rhs._pTimeAccs)
{
}

HRESULT VIBufferInstancing::InitializePrototype(const INSTANCE_DESC& InstanceDesc)
{
    _iStrideInstance = sizeof(VTXINSTANCE);

    ::ZeroMemory(&_BufferDesc._bufferDesc, sizeof(_BufferDesc._bufferDesc));

    _BufferDesc._bufferDesc.ByteWidth = _iStrideInstance * _iNumInstance;
    _BufferDesc._bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    _BufferDesc._bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    _BufferDesc._bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    _BufferDesc._bufferDesc.MiscFlags = 0;
    _BufferDesc._bufferDesc.StructureByteStride = _iStrideInstance;

    _pInstanceVertices = new VTXINSTANCE[_iNumInstance];
    ::ZeroMemory(_pInstanceVertices, sizeof(VTXINSTANCE) * _iNumInstance);


    //random_device		RandomDevice;

    //mt19937_64								RandomNumber;
    //uniform_real_distribution<float>		RandomX;
    //uniform_real_distribution<float>		RandomY;
    //uniform_real_distribution<float>		RandomZ;

    //uniform_real_distribution<float>		ScaleRandom;
    //uniform_real_distribution<float>		SpeedRandom;
    //uniform_real_distribution<float>		LifeTimeRandom;

    //SpeedRandom = uniform_real_distribution<float>(InstanceDesc.fSpeedMin, InstanceDesc.fSpeedMax);
    //LifeTimeRandom = uniform_real_distribution<float>(InstanceDesc.fLifeTimeMin, InstanceDesc.fLifeTimeMax);

    _pSpeeds = new _float[InstanceDesc.iNumInstance];
    _pLifeTimes = new _float[InstanceDesc.iNumInstance];
    _pTimeAccs = new _float[InstanceDesc.iNumInstance];

    for (size_t i = 0; i < InstanceDesc.iNumInstance; i++)
    {
        //_pSpeeds[i] = SpeedRandom(RandomDevice);
        _pLifeTimes[i] = 1.f;
        _pTimeAccs[i] = 0.f;
    }

    //RandomNumber = mt19937_64(RandomDevice());
    //RandomX = uniform_real_distribution<float>(InstanceDesc.vRange.x * -0.5f, InstanceDesc.vRange.x * 0.5f);
    //RandomY = uniform_real_distribution<float>(InstanceDesc.vRange.y * -0.5f, InstanceDesc.vRange.y * 0.5f);
    //RandomZ = uniform_real_distribution<float>(InstanceDesc.vRange.z * -0.5f, InstanceDesc.vRange.z * 0.5f);

    //ScaleRandom = uniform_real_distribution<float>(InstanceDesc.fScaleMin, InstanceDesc.fScaleMax);

    for (uint32 i = 0; i < _iNumInstance; ++i)
    {
        //_float		fScale = ScaleRandom(RandomNumber);

        _pInstanceVertices[i].vRight = Vec4(1.f, 0.f, 0.f, 0.f);
        _pInstanceVertices[i].vUp = Vec4(0.f, 1.f, 0.f, 0.f);
        _pInstanceVertices[i].vLook = Vec4(0.f, 0.f, 1.f, 0.f);
        _pInstanceVertices[i].vTranslation = Vec4(
            InstanceDesc.vCenter.x,
            InstanceDesc.vCenter.y,
            InstanceDesc.vCenter.z,
            1.f);
    }

    ::ZeroMemory(&_BufferDesc._subResourceData, sizeof(_BufferDesc._subResourceData));
    _BufferDesc._subResourceData.pSysMem = _pInstanceVertices;

    return S_OK;
}

HRESULT VIBufferInstancing::Initialize(void* argument)
{
    if (FAILED(__super::CreateBuffer(&_pVBInstance)))
        return E_FAIL;

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

    if (false == _isCloned)
    {
        Safe_Delete_Array<VTXINSTANCE*>(_pInstanceVertices);
        Safe_Delete_Array<_float*>(_pTimeAccs);
        Safe_Delete_Array<_float*>(_pLifeTimes);
        Safe_Delete_Array<_float*>(_pSpeeds);
    }

    Safe_Release<ID3D11Buffer*>(_pVBInstance);
}

#include "pch.h"
#include "Level.h"

Level::Level(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
    : _device(device), _deviceContext(deviceContext)
{
    Safe_AddRef<ID3D11Device*>(_device);
    Safe_AddRef<ID3D11DeviceContext*>(_deviceContext);
}

HRESULT Level::Initialize()
{
    return S_OK;
}

HRESULT Level::Tick(const _float& timeDelta)
{
    return S_OK;
}

HRESULT Level::LateTick(const _float& timeDelta)
{
    return S_OK;
}

void Level::Free()
{
    Safe_Release<ID3D11Device*>(_device);
    Safe_Release<ID3D11DeviceContext*>(_deviceContext);
}

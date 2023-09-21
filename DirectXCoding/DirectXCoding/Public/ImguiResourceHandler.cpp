#include "pch.h"
#include "ImguiResourceHandler.h"

HRESULT ImGuiResourceHandler::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    Safe_AddRef<ID3D11Device*>(device);
    Safe_AddRef<ID3D11DeviceContext*>(deviceContext);

    _device = device;
    _deviceContext = deviceContext;

    return S_OK;
}

void ImGuiResourceHandler::Tick(const _float& timeDelta)
{
}

void ImGuiResourceHandler::LateTick(const _float& timeDelta)
{
}

HRESULT ImGuiResourceHandler::Render()
{
    return S_OK;
}

void ImGuiResourceHandler::Free()
{
    Safe_Release<ID3D11Device*>(_device);
    Safe_Release<ID3D11DeviceContext*>(_deviceContext);
}

#include "pch.h"
#include "UI.h"

UI::UI(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: GameObject(device, deviceContext, OBJECT_TYPE::UI)
{
}


UI::UI(const UI& rhs)
	: GameObject(rhs)
{

}

HRESULT UI::InitializePrototype()
{
	return S_OK;
}

HRESULT UI::Initialize(void* argument)
{
	return S_OK;
}

void UI::PriorityTick(const _float& timeDelta)
{
}

void UI::Tick(const _float& timeDelta)
{
}

void UI::LateTick(const _float& timeDelta)
{
}

HRESULT UI::Render()
{
	return S_OK;
}

void UI::Free()
{
	__super::Free();
}

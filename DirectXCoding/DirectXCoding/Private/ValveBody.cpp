#include "pch.h"
#include "ValveBody.h"
#include "GameInstance.h"

ValveBody::ValveBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: GameObject(pDevice, pDeviceContext, OBJECT_TYPE::DYNAMIC)
{
}

ValveBody::ValveBody(const ValveBody& rhs)
	: GameObject(rhs)
{
}

HRESULT ValveBody::InitializePrototype()
{
	return S_OK;
}

HRESULT ValveBody::Initialize(void* pArg)
{

	return S_OK;
}

void ValveBody::Tick(const _float& fTimeDelta)
{
}

void ValveBody::LateTick(const _float& fTimeDelta)
{
}

HRESULT ValveBody::Render()
{
	return S_OK;
}

HRESULT ValveBody::Ready_Components()
{
	return S_OK;
}

HRESULT ValveBody::BindShaderResources()
{
	return S_OK;
}

ValveBody* ValveBody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	return nullptr;
}

GameObject* ValveBody::Clone(void* pArg)
{
	return nullptr;
}

void ValveBody::Free()
{
}

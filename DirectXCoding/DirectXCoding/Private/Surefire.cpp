#include "pch.h"
#include "Surefire.h"

Surefire::Surefire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: GameObject(pDevice, pContext, OBJECT_TYPE::PLAYER)
{

}

Surefire::Surefire(const GameObject& rhs)
	: GameObject(rhs)
{

}

HRESULT Surefire::Initialize_Prototype()
{
	return S_OK;
}

HRESULT Surefire::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void Surefire::Tick(_float fTimeDelta)
{
}

void Surefire::LateTick(_float fTimeDelta)
{
}

HRESULT Surefire::Render()
{
	return E_NOTIMPL;
}

HRESULT Surefire::Ready_Components()
{
	return E_NOTIMPL;
}

HRESULT Surefire::Bind_ShaderResources()
{
	return E_NOTIMPL;
}

Surefire* Surefire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

GameObject* Surefire::Clone(void* pArg)
{
	return nullptr;
}

void Surefire::Free()
{
}

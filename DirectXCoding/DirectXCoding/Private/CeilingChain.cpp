#include "pch.h"
#include "CeilingChain.h"

#include "GameInstance.h"

CeilingChain::CeilingChain(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
    : DynamicObject(device, deviceContext, DYNAMIC_TYPE::CHAIN)
{

}

CeilingChain::CeilingChain(const CeilingChain& rhs)
    : DynamicObject(rhs)
{
}

HRESULT CeilingChain::InitializePrototype()
{
    return S_OK;
}

HRESULT CeilingChain::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CeilingChain::Tick(const _float& timeDelta)
{
    if (!_enabled)
        return;
}

void CeilingChain::LateTick(const _float& timeDelta)
{
	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

HRESULT CeilingChain::Render()
{
	if (_enabled)
		return S_OK;


	if (FAILED(__super::BindShaderResource()))
		return E_FAIL;


	uint32 numMeshes = _binaryModel->GetNumMeshes();

	for (size_t i = 0; i < numMeshes; i++)
	{
		if (FAILED(_binaryModel->BindMaterialTexture(_shader, "DiffuseMap", i, TextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(_shader->Begin(0)))
			return E_FAIL;

		if (FAILED(_binaryModel->Render(i)))
			return E_FAIL;
	}


	return S_OK;

}

CeilingChain* CeilingChain::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	CeilingChain* pInstance = new CeilingChain(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Create to Failed : CeilingChain");
		Safe_Release<CeilingChain*>(pInstance);
	}

	return pInstance;
}

GameObject* CeilingChain::Clone(void* argument)
{
	CeilingChain* pInstance = new CeilingChain(*this);

	if (FAILED(pInstance->Initialize(argument)))
	{
		MSG_BOX("Create to Failed : BreakDoor");
		Safe_Release<CeilingChain*>(pInstance);
	}

	return pInstance;
}

void CeilingChain::Free()
{
	__super::Free();
}

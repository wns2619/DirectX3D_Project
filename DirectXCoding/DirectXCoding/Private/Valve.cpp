#include "pch.h"
#include "Valve.h"

#include "GameInstance.h"

Valve::Valve(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: DynamicObject(device, deviceContext, DYNAMIC_TYPE::VALVE)
{

}

Valve::Valve(const Valve& rhs)
	: DynamicObject(rhs)
{
}

HRESULT Valve::InitializePrototype()
{
	return S_OK;
}

HRESULT Valve::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void Valve::Tick(const _float& timeDelta)
{
	if (!_enabled)
		return;
}

void Valve::LateTick(const _float& timeDelta)
{
	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

HRESULT Valve::Render()
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

Valve* Valve::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Valve* pInstance = new Valve(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Create to Failed : Valve");
		Safe_Release<Valve*>(pInstance);
	}

	return pInstance;
}

GameObject* Valve::Clone(void* argument)
{
	Valve* pInstance = new Valve(*this);

	if (FAILED(pInstance->Initialize(argument)))
	{
		MSG_BOX("Create to Failed : BreakDoor");
		Safe_Release<Valve*>(pInstance);
	}

	return pInstance;
}

void Valve::Free()
{
	__super::Free();
}

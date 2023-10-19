#include "pch.h"
#include "BreakDoor.h"

#include "GameInstance.h"

BreakDoor::BreakDoor(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: DynamicObject(device, deviceContext, DYNAIMC_TYPE::BREAK_DOOR)
{

}

BreakDoor::BreakDoor(const BreakDoor& rhs)
	: DynamicObject(rhs)
{

}

HRESULT BreakDoor::InitializePrototype()
{
	return S_OK;
}

HRESULT BreakDoor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

}

void BreakDoor::Tick(const _float& timeDelta)
{
	if (!_enabled)
		return;
}

void BreakDoor::LateTick(const _float& timeDelta)
{
	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);

}

HRESULT BreakDoor::Render()
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

BreakDoor* BreakDoor::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	BreakDoor* pInstance = new BreakDoor(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Create to Failed : BreakDoor");
		Safe_Release<BreakDoor*>(pInstance);
	}

	return pInstance;
}

GameObject* BreakDoor::Clone(void* argument)
{
	BreakDoor* pInstance = new BreakDoor(*this);

	if (FAILED(pInstance->Initialize(argument)))
	{
		MSG_BOX("Create to Failed : BreakDoor");
		Safe_Release<BreakDoor*>(pInstance);
	}

	return pInstance;
}

void BreakDoor::Free()
{
	__super::Free();

	Safe_Release<Renderer*>(_render);
	Safe_Release<Shader*>(_shader);
}

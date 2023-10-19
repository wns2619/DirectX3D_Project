#include "pch.h"
#include "HorrorMask.h"

#include "GameInstance.h"

HorrorMask::HorrorMask(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: DynamicObject(device, deviceContext, DYNAIMC_TYPE::MASK)
{

}

HorrorMask::HorrorMask(const HorrorMask& rhs)
	: DynamicObject(rhs)
{

}

HRESULT HorrorMask::InitializePrototype()
{
	return S_OK;
}

HRESULT HorrorMask::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

}

void HorrorMask::Tick(const _float& timeDelta)
{
	if (!_enabled)
		return;
}

void HorrorMask::LateTick(const _float& timeDelta)
{
	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);

}

HRESULT HorrorMask::Render()
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

HorrorMask* HorrorMask::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	HorrorMask* pInstance = new HorrorMask(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Create to Failed : HorrorMask");
		Safe_Release<HorrorMask*>(pInstance);
	}

	return pInstance;
}

GameObject* HorrorMask::Clone(void* argument)
{
	HorrorMask* pInstance = new HorrorMask(*this);

	if (FAILED(pInstance->Initialize(argument)))
	{
		MSG_BOX("Create to Failed : HorrorMask");
		Safe_Release<HorrorMask*>(pInstance);
	}

	return pInstance;
}

void HorrorMask::Free()
{
	__super::Free();

	Safe_Release<Renderer*>(_render);
	Safe_Release<Shader*>(_shader);
}

#include "pch.h"
#include "WallPainting.h"

#include "GameInstance.h"
WallPainting::WallPainting(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: StaticObject(device, deviceContext)
{
	// Empty¿´´Ù°¡ 
}

WallPainting::WallPainting(const WallPainting& rhs)
	: StaticObject(rhs)
{
}

HRESULT WallPainting::InitializePrototype()
{
	return S_OK;
}

HRESULT WallPainting::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void WallPainting::Tick(const _float& timeDelta)
{
}

void WallPainting::LateTick(const _float& timeDelta)
{
	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

HRESULT WallPainting::Render()
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


WallPainting* WallPainting::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	WallPainting* Instance = new WallPainting(device, deviceContext);

	if (FAILED(Instance->InitializePrototype()))
	{
		MSG_BOX("Create Failed WallPainting");
		Safe_Release<WallPainting*>(Instance);
	}

	return Instance;
}

GameObject* WallPainting::Clone(void* argument)
{
	WallPainting* Instance = new WallPainting(*this);

	if (FAILED(Instance->Initialize(argument)))
	{
		MSG_BOX("Create Failed WallPainting");
		Safe_Release<WallPainting*>(Instance);
	}

	return Instance;
}

void WallPainting::Free()
{
	__super::Free();
}

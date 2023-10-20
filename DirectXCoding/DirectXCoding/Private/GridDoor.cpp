#include "pch.h"
#include "GridDoor.h"

#include "GameInstance.h"

GridDoor::GridDoor(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: DynamicObject(device, deviceContext, DYNAMIC_TYPE::GRID_DOOR)
{

}

GridDoor::GridDoor(const GridDoor& rhs)
	: DynamicObject(rhs)
{
}

HRESULT GridDoor::InitializePrototype()
{
	return S_OK;
}

HRESULT GridDoor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void GridDoor::Tick(const _float& timeDelta)
{
	if (!_enabled)
		return;
}

void GridDoor::LateTick(const _float& timeDelta)
{
	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

HRESULT GridDoor::Render()
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

GridDoor* GridDoor::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	GridDoor* pInstance = new GridDoor(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Create to Failed : GridDoor");
		Safe_Release<GridDoor*>(pInstance);
	}

	return pInstance;
}

GameObject* GridDoor::Clone(void* argument)
{
	GridDoor* pInstance = new GridDoor(*this);

	if (FAILED(pInstance->Initialize(argument)))
	{
		MSG_BOX("Create to Failed : GridDoor");
		Safe_Release<GridDoor*>(pInstance);
	}

	return pInstance;
}

void GridDoor::Free()
{
	__super::Free();
}

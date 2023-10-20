#include "pch.h"
#include "Cage.h"

#include "GameInstance.h"

Cage::Cage(ID3D11Device* device, ID3D11DeviceContext* deviceContext, CAGE_TYPE eType)
	: DynamicObject(device, deviceContext, DYNAMIC_TYPE::GRID_DOOR)
{
	_eType = eType;
	// 밸브랑 연동할 케이지 타입.
}

Cage::Cage(const Cage& rhs)
	: DynamicObject(rhs)
{
}

HRESULT Cage::InitializePrototype()
{
	return S_OK;
}

HRESULT Cage::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void Cage::Tick(const _float& timeDelta)
{
	if (!_enabled)
		return;
}

void Cage::LateTick(const _float& timeDelta)
{
	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

HRESULT Cage::Render()
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

Cage* Cage::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, CAGE_TYPE eType)
{
	Cage* pInstance = new Cage(device, deviceContext, eType);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Create to Failed : Cage");
		Safe_Release<Cage*>(pInstance);
	}

	return pInstance;
}

GameObject* Cage::Clone(void* argument)
{
	Cage* pInstance = new Cage(*this);

	if (FAILED(pInstance->Initialize(argument)))
	{
		MSG_BOX("Create to Failed : Cage");
		Safe_Release<Cage*>(pInstance);
	}

	return pInstance;
}

void Cage::Free()
{
	__super::Free();
}

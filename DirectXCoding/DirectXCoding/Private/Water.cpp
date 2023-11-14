#include "pch.h"
#include "Water.h"

#include "GameInstance.h"

Water::Water(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: DynamicObject(device, deviceContext, DYNAMIC_TYPE::WATER)
{

}

Water::Water(const Water& rhs)
	: DynamicObject(rhs)
{
}

HRESULT Water::InitializePrototype()
{
	return S_OK;
}

HRESULT Water::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyCollider()))
		return E_FAIL;

	if (nullptr != pArg)
		_transform->SetWorldMatrix(static_cast<ComponentNames*>(pArg)->_saveWorldMatrix);

	return S_OK;
}

void Water::Tick(const _float& timeDelta)
{
	//_fStream += timeDelta * timeDelta;

	//if (FAILED(_shader->BindRawValue("gTimeDelta", &_fStream, sizeof(_float))))
	//	return;
}

void Water::LateTick(const _float& timeDelta)
{
	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

HRESULT Water::Render()
{
	if (_enabled)
		return S_OK;

	if (FAILED(__super::BindShaderResource()))
		return E_FAIL;

	uint32 numMeshes = _binaryModel->GetNumMeshes();

	for (size_t i = 0; i < numMeshes; i++)
	{
		//if (FAILED(_binaryModel->BindMaterialTexture(_shader, "DiffuseMap", i, TextureType_DIFFUSE)))
		//	return E_FAIL;

		if (FAILED(_binaryModel->BindMaterialTexture(_shader, "NormalMap", i, TextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(_shader->Begin(1)))
			return E_FAIL;


		if (FAILED(_binaryModel->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void Water::OnCollisionEnter(Collider* pOther)
{
}

void Water::OnCollisionStay(Collider* pOther)
{
}

void Water::OnCollisionExit(Collider* pOther)
{
}

HRESULT Water::ReadyCollider()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);


	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == pGameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);


	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform))))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

Water* Water::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Water* pInstance = new Water(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Create to Failed : Water");
		Safe_Release<Water*>(pInstance);
	}

	return pInstance;
}

GameObject* Water::Clone(void* argument)
{
	Water* pInstance = new Water(*this);

	if (FAILED(pInstance->Initialize(argument)))
	{
		MSG_BOX("Create to Failed : Water");
		Safe_Release<Water*>(pInstance);
	}

	return pInstance;
}

void Water::Free()
{
	__super::Free();
}

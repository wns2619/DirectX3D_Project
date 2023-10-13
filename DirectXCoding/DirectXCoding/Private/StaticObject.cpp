#include "pch.h"
#include "StaticObject.h"

#include "GameInstance.h"
StaticObject::StaticObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: GameObject(device, deviceContext, OBJECT_TYPE::STATIC)
{
	// Empty였다가 
}

StaticObject::StaticObject(const StaticObject& rhs)
	: GameObject(rhs)
{
}

HRESULT StaticObject::InitializePrototype()
{
	return S_OK;
}

HRESULT StaticObject::Initialize(void* pArg)
{
	if (FAILED(ReadyComponents()))
		return E_FAIL;


	if (nullptr != pArg)
	{
		// TODO
		// 복사할 떄 사용할 쉐이더와 모델 이름을 던진다.
		
	}

	return S_OK;
}

void StaticObject::Tick(const _float& timeDelta)
{
}

void StaticObject::LateTick(const _float& timeDelta)
{
	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

HRESULT StaticObject::Render()
{
	if (_enabled)
		return S_OK;


	if (FAILED(BindShaderResource()))
		return E_FAIL;

	uint32 numMeshes = _model->GetNumMeshes();

	for (size_t i = 0; i < numMeshes; i++)
	{
		if (FAILED(_model->BindMaterialTexture(_shader, "DiffuseMap", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(_shader->Begin(0)))
			return E_FAIL;

		if (FAILED(_model->Render(i)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT StaticObject::ReadyComponents()
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);
	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == gameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);

	/* Renderer Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentRenderer"),
		TEXT("ComponentRenderer"), reinterpret_cast<Component**>(&_render))))
		return E_FAIL;

	if (FAILED(__super::AddComponent(level,
		TEXT("ProtoTypeComponentDefaultMeshShader"),
		TEXT("Component_Shader"), reinterpret_cast<Component**>(&_shader))))
		return E_FAIL;


	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform))))
		return E_FAIL;


	if (FAILED(__super::AddComponent(level, TEXT("ProtoType2stBottom"),
		TEXT("ComponentModel"), reinterpret_cast<Component**>(&_model))))
		return E_FAIL;


	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT StaticObject::BindShaderResource()
{
	GameInstance* gameInstance = GameInstance::GetInstance();
	Safe_AddRef<GameInstance*>(gameInstance);


	if (FAILED(_transform->BindShaderResources(_shader, "W")))
		return E_FAIL;

	if (FAILED(gameInstance->BindTransformToShader(_shader, "V", CameraHelper::TRANSFORMSTATE::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(gameInstance->BindTransformToShader(_shader, "P", CameraHelper::TRANSFORMSTATE::D3DTS_PROJ)))
		return E_FAIL;

	const LIGHT_DESC* lightdesc = gameInstance->GetLightDesc(0);

	if (FAILED(_shader->BindRawValue("GlobalLight", lightdesc, sizeof(LIGHT_DESC))))
		return E_FAIL;

	MESH_MATERIAL materialDesc;
	::ZeroMemory(&materialDesc, sizeof(materialDesc));
	materialDesc.ambient = Vec4(0.8);
	materialDesc.diffuse = Vec4(1.f);
	materialDesc.specular = Vec4(1.f);

	if (FAILED(_shader->BindRawValue("Material", &materialDesc, 80)))
		return E_FAIL;

	Safe_Release<GameInstance*>(gameInstance);

	return S_OK;
}

StaticObject* StaticObject::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	StaticObject* Instance = new StaticObject(device, deviceContext);

	if (FAILED(Instance->InitializePrototype()))
	{
		MSG_BOX("Create Failed StaticObject");
		Safe_Release<StaticObject*>(Instance);
	}

	return Instance;
}

GameObject* StaticObject::Clone(void* argument)
{
	StaticObject* Instance = new StaticObject(*this);

	if (FAILED(Instance->Initialize(argument)))
	{
		MSG_BOX("Create Failed StaticObject");
		Safe_Release<StaticObject*>(Instance);
	}

	return Instance;
}

void StaticObject::Free()
{
	__super::Free();

	Safe_Release<Renderer*>(_render);
	Safe_Release<Shader*>(_shader);
}

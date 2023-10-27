#include "pch.h"
#include "DynamicObject.h"

#include "GameInstance.h"
DynamicObject::DynamicObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext, DYNAMIC_TYPE eType)
	: GameObject(device, deviceContext, OBJECT_TYPE::DYNAMIC), _eDynamicType(eType)
{
	// Empty였다가 
}

DynamicObject::DynamicObject(const DynamicObject& rhs)
	: GameObject(rhs)
{
}

HRESULT DynamicObject::InitializePrototype()
{
	return S_OK;
}

HRESULT DynamicObject::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		// TODO
		// 복사할 떄 사용할 쉐이더와 모델 이름을 던진다.
		ComponentNames* names = static_cast<ComponentNames*>(pArg);
		_comNames._protoTypeTag = names->_protoTypeName;
		_comNames._strModelComponentName = names->_strModelComponentName;
		_modelName = names->_strModelName;
		_comNames._strShaderName = names->_strShaderName;
		_id = names->_modelID;
	}

	if (FAILED(ReadyComponents()))
		return E_FAIL;

	//_transform->SetScaling(Vec3(0.01f, 0.01f, 0.01f));

	//_transform->SetScaling(Vec3(0.01f, 0.01f, 0.01f));

	//_transform->SetWorldMatrix()
	//_transform->FixRotation(Vec3(0.f, 1.f, 0.f), ::XMConvertToRadians(90.f));
	return S_OK;
}

void DynamicObject::Tick(const _float& timeDelta)
{
}

void DynamicObject::LateTick(const _float& timeDelta)
{
	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

HRESULT DynamicObject::Render()
{
	if (_enabled)
		return S_OK;


	if (FAILED(BindShaderResource()))
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

HRESULT DynamicObject::ReadyComponents()
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
		_comNames._strShaderName,
		TEXT("Component_Shader"), reinterpret_cast<Component**>(&_shader))))
		return E_FAIL;


	if (FAILED(__super::AddComponent(level, _comNames._strModelComponentName,
		TEXT("ComponentModel"), reinterpret_cast<Component**>(&_binaryModel))))
		return E_FAIL;


	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT DynamicObject::BindShaderResource()
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

void DynamicObject::Free()
{
	__super::Free();

	Safe_Release<Renderer*>(_render);
	Safe_Release<Shader*>(_shader);
}

#include "pch.h"
#include "Terrain.h"

#include "GameInstance.h"

Terrain::Terrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: GameObject(device, deviceContext, OBJECT_TYPE::TERRAIN)
{
	_objectType = OBJECT_TYPE::TERRAIN;
	_modelName = "Terrain";
}

Terrain::Terrain(const Terrain& rhs)
	: GameObject(rhs)
{

}

HRESULT Terrain::InitializePrototype()
{
	return S_OK;
}

HRESULT Terrain::Initialize(void* pArg)
{


	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT Terrain::Render()
{
	if (_enabled)
		return S_OK;


	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_shader->Begin(0);

	_viBuffer->Render();

	return S_OK;
}

void Terrain::Tick(const _float& fTimeDelta)
{
	if (_enabled)
		return;
}

void Terrain::LateTick(const _float& fTimeDelta)
{
	if (!_enabled)
		_renderComponent->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

HRESULT Terrain::Ready_Components()
{
	/* Renderer Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentRenderer"),
		TEXT("ComponentRenderer"), reinterpret_cast<Component**>(&_renderComponent))))
		return E_FAIL;
		

	/* Shader Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME),
		TEXT("ProtoTypeComponentHillsShader"),
		TEXT("ComponentShader"), reinterpret_cast<Component**>(&_shader))))
		return E_FAIL;

	/* VIBuffer Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeComponentVIBufferTerrain"),
		TEXT("ComponentVIBuffer"), reinterpret_cast<Component**>(&_viBuffer))))
		return E_FAIL;


	/* Texture Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeComponentTextureTerrain"),
		TEXT("ComponenetTexture"), reinterpret_cast<Component**>(&_texture))))
		return E_FAIL;

	/* Transform Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform))))
		return E_FAIL;

	/* Light Component */
	//Light::DirectinoalLight testlightinfo;
	//::ZeroMemory(&testlightinfo, sizeof(testlightinfo));
	//{
	//	testlightinfo.Direction = Vec3(1.f, -1.f, 1.f);
	//	testlightinfo.Diffuse = Vec4(1.f, 1.f, 1.f, 1.f);
	//	testlightinfo.Ambient = Vec4(1.f, 1.f, 1.f, 1.f);
	//	testlightinfo.Specular = Vec4(1.f, 1.f, 1.f, 1.f);
	//	testlightinfo.Pad = 0.f;
	//}
	//if(FAILED(__super::AddLightComponent(static_cast<uint32>(LEVEL::GAME), Light::LightType::DIRECTIONAL,
	//	TEXT("ProtoTypeComponentLight"),reinterpret_cast<Component**>(&_light), &testlightinfo)))
	//	return E_FAIL;

	

 	return S_OK;
}

HRESULT Terrain::Bind_ShaderResources()
{
	GameInstance* gameInstance = GameInstance::GetInstance();
	Safe_AddRef<GameInstance*>(gameInstance);

	if (FAILED(_transform->BindShaderResources(_shader, "W")))
		return E_FAIL;
	if (FAILED(gameInstance->BindTransformToShader(_shader, "V", CameraHelper::TRANSFORMSTATE::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(gameInstance->BindTransformToShader(_shader, "P", CameraHelper::TRANSFORMSTATE::D3DTS_PROJ)))
		return E_FAIL;

	if (FAILED(gameInstance->BindCameraPosition(_shader, "cameraPosition", sizeof(Vec4))))
		return E_FAIL;


	const LIGHT_DESC* lightdesc = gameInstance->GetLightDesc(0);

	if (FAILED(_shader->BindRawValue("Lightinfo", lightdesc, sizeof(LIGHT_DESC))))
		return E_FAIL;

	//if (FAILED(_light->BindingLightToShader(_shader, "dirLight", Light::LightType::DIRECTIONAL, sizeof(Light::DirectinoalLight))))
	//	return E_FAIL;

	Safe_Release<GameInstance*>(gameInstance);

	if (FAILED(_texture->BindShaderResource(_shader, "DiffuseTexture", 0)))
		return E_FAIL;

	return S_OK;
}



GameObject* Terrain::Clone(void* argument)
{
	Terrain* terrain = new Terrain(*this);

	if (FAILED(terrain->Initialize(argument)))
	{
 		MSG_BOX("Failed to Cloned : Terrain");
		Safe_Release<Terrain*>(terrain);
		return nullptr;
	}

	return terrain;
}


Terrain* Terrain::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Terrain* terrain = new Terrain(device, deviceContext);

	if (FAILED(terrain->InitializePrototype()))
	{
		MSG_BOX("Failed to Create : Terrain");
		Safe_Release<Terrain*>(terrain);
		return nullptr;
	}

	return terrain;
}

void Terrain::Free()
{
	__super::Free();

	Safe_Release<Transform*>(_transform);
	Safe_Release<Shader*>(_shader);
	Safe_Release<Texture*>(_texture);
	//Safe_Release<Light*>(_light);
	Safe_Release<VIBufferTerrain*>(_viBuffer);
	Safe_Release<Renderer*>(_renderComponent);

}
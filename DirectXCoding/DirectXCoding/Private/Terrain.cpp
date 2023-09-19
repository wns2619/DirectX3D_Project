#include "pch.h"
#include "Terrain.h"

#include "GameInstance.h"

Terrain::Terrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: GameObject(device, deviceContext)
{
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
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_shader->Begin(0);

	_viBuffer->Render();

	return S_OK;
}

void Terrain::Tick(const _float& fTimeDelta)
{

}

void Terrain::LateTick(const _float& fTimeDelta)
{
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


 	return S_OK;
}

HRESULT Terrain::Bind_ShaderResources()
{
	//XMStoreFloat4x4(&_viewMatrix, XMMatrixLookAtLH(XMVectorSet(0.f, 100.f, -80.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.0f, 1.f, 0.f, 0.f)));
	//XMStoreFloat4x4(&_projMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), g_iWinSizeX / g_iWinSizeY, 0.2f, 300.f));

	GameInstance* gameInstance = GameInstance::GetInstance();
	Safe_AddRef<GameInstance*>(gameInstance);

	if (FAILED(_transform->BindShaderResources(_shader, "worldMatrix")))
		return E_FAIL;
	if (FAILED(gameInstance->BindTransformToShader(_shader, "viewMatrix", CameraHelper::TRANSFORMSTATE::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(gameInstance->BindTransformToShader(_shader, "projMatrix", CameraHelper::TRANSFORMSTATE::D3DTS_PROJ)))
		return E_FAIL;
	
	if (FAILED(_texture->BindShaderResource(_shader, "ShadersTexture", 0)))
		return E_FAIL;

	Safe_Release<GameInstance*>(gameInstance);

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

	Safe_Release<Shader*>(_shader);
	Safe_Release<Texture*>(_texture);
	Safe_Release<Transform*>(_transform);
	Safe_Release<VIBufferTerrain*>(_viBuffer);
	Safe_Release<Renderer*>(_renderComponent);

}
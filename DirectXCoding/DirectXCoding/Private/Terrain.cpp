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
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), 
		TEXT("ProtoTypeComponentShaderVertexTextureNormalData"),
		TEXT("ComponentTerrainShader"), reinterpret_cast<Component**>(&_shader))))
		return E_FAIL;

	/* VIBuffer Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentVIBufferTerrain"),
		TEXT("ComponentTerrainVIBuffer"), reinterpret_cast<Component**>(&_viBuffer))))
		return E_FAIL;

	/* Texture Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeComponentTextureTerrain"),
		TEXT("ComponentTerrainTexture"), reinterpret_cast<Component**>(&_texture))))
		return E_FAIL;

	/* Transform Component */
	Transform::TRANSFORM_DESC transformDesc;
	ZeroMemory(&transformDesc, sizeof(transformDesc));
	//{
	//	transformDesc.speedPerSec = 5.f;
	//	transformDesc.rotationRadianPerSec = ::XMConvertToRadians(90.f);
	//}

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform), &transformDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT Terrain::Bind_ShaderResources()
{
	if (FAILED(_transform->BindShaderResources(_shader, "worldMatrix")))
		return E_FAIL;
	if (FAILED(_shader->BindMatrix("viewMatrix", &_viewMatrix)))
		return E_FAIL;
	if (FAILED(_shader->BindMatrix("ProjMatrix", &_projMatrix)))
		return E_FAIL;

	if (FAILED(_texture->BindShaderResource(_shader, "ShadersTexture", 0)))
		return E_FAIL;
	if (FAILED(_texture->BindShaderReosurces(_shader, "ShadersTextures")))
		return E_FAIL;

	return S_OK;
}

Terrain* Terrain::Clone(void* argument)
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
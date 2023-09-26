#include "pch.h"
#include "EditorTerrain.h"

#include "GameInstance.h"


EditorTerrain::EditorTerrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: GameObject(device, deviceContext, OBJECT_TYPE::TERRAIN)
{
	_objectType = OBJECT_TYPE::TERRAIN;
}

EditorTerrain::EditorTerrain(const EditorTerrain& rhs)
	: GameObject(rhs)
{

}

HRESULT EditorTerrain::InitializePrototype()
{
 	return S_OK;
}

HRESULT EditorTerrain::Initialize(void* pArg)
{
	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT EditorTerrain::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_shader->Begin(0);

	
	_viBuffer->Render();

	//D3D11_RASTERIZER_DESC rasterizerDesc;
	//::ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

	//rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME; 
	//rasterizerDesc.CullMode = D3D11_CULL_NONE;      

	//ID3D11RasterizerState* wireframeRasterizerState;
	//_device->CreateRasterizerState(&rasterizerDesc, &wireframeRasterizerState);
	//_deviceContext->RSSetState(wireframeRasterizerState);


	return S_OK;
}

void EditorTerrain::Tick(const _float& fTimeDelta)
{
}

void EditorTerrain::LateTick(const _float& fTimeDelta)
{
	_renderComponent->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

HRESULT EditorTerrain::Ready_Components(void* pArg)
{
	/* Renderer Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentRenderer"),
		TEXT("ComponentEditRenderer"), reinterpret_cast<Component**>(&_renderComponent))))
		return E_FAIL;


	/* Shader Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::EDIT),
		TEXT("ProtoTypeComponentEditHillsShader"),
		TEXT("ComponentEditShader"), reinterpret_cast<Component**>(&_shader))))
		return E_FAIL;

	
	VIBufferTerrain::TERRAIN_DESC* desc = nullptr;

	if (nullptr != pArg)
		desc = static_cast<VIBufferTerrain::TERRAIN_DESC*>(pArg);
	

	/* VIBuffer Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeComponentEditVIBufferTerrain"),
		TEXT("ComponentEditVIBuffer"), reinterpret_cast<Component**>(&_viBuffer), desc)))
		return E_FAIL;


	/* Transform Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentEditTransform"), reinterpret_cast<Component**>(&_transform))))
		return E_FAIL;


	return S_OK;
}

HRESULT EditorTerrain::Bind_ShaderResources()
{
	GameInstance* gameInstance = GameInstance::GetInstance();
	Safe_AddRef<GameInstance*>(gameInstance);

	if (FAILED(_transform->BindShaderResources(_shader, "worldMatrix")))
		return E_FAIL;
	if (FAILED(gameInstance->BindTransformToShader(_shader, "viewMatrix", CameraHelper::TRANSFORMSTATE::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(gameInstance->BindTransformToShader(_shader, "projMatrix", CameraHelper::TRANSFORMSTATE::D3DTS_PROJ)))
		return E_FAIL;

	if (FAILED(gameInstance->BindCameraPosition(_shader, "camPosition", sizeof(Vec4))))
		return E_FAIL;

	Safe_Release<GameInstance*>(gameInstance);

	//if (FAILED(_texture->BindShaderResource(_shader, "DiffuseTexture", 0)))
	//	return E_FAIL;

	return S_OK;
}

GameObject* EditorTerrain::Clone(void* argument)
{
	EditorTerrain* terrain = new EditorTerrain(*this);

	if (FAILED(terrain->Initialize(argument)))
	{
		MSG_BOX("Failed to Cloned : EditorTerrain");
		Safe_Release<EditorTerrain*>(terrain);
		return nullptr;
	}

	return terrain;
}

void EditorTerrain::Free()
{
	__super::Free();

	Safe_Release<Shader*>(_shader);
	Safe_Release<Texture*>(_texture);
	Safe_Release<Transform*>(_transform);
	Safe_Release<VIBufferTerrain*>(_viBuffer);
	Safe_Release<Renderer*>(_renderComponent);
}

EditorTerrain* EditorTerrain::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	EditorTerrain* terrain = new EditorTerrain(device, deviceContext);

	if (FAILED(terrain->InitializePrototype()))
	{
		MSG_BOX("Failed to Create : EditorTerrain");
		Safe_Release<EditorTerrain*>(terrain);
		return nullptr;
	}

	return terrain;
}

#include "pch.h"
#include "EditorTerrain.h"

#include "GameInstance.h"


EditorTerrain::EditorTerrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: GameObject(device, deviceContext, OBJECT_TYPE::TERRAIN)
{
	_objectType = OBJECT_TYPE::TERRAIN;
	_modelName = "EditTerrain";
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

	if (pArg != nullptr)
	{
		VIBufferTerrain::TERRAIN_DESC* TerrainDesc = static_cast<VIBufferTerrain::TERRAIN_DESC*>(pArg);

		_NumVerticesX = TerrainDesc->numVerticesX;
		_NumVerticesZ = TerrainDesc->numVerticesZ;
	}



	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

HRESULT EditorTerrain::Render()
{
	if (_enabled)
		return S_OK;


	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_shader->Begin(0);

	
	_viBuffer->Render();

	return S_OK;
}

void EditorTerrain::Tick(const _float& fTimeDelta)
{
	if (_enabled)
		return;

}

void EditorTerrain::LateTick(const _float& fTimeDelta)
{
	if (!_enabled)
		_renderComponent->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

Vec4 EditorTerrain::TerrainPick()
{
	if (_enabled)
		return Vec4();

	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	POINT pt;
	::GetCursorPos(&pt);
	::ScreenToClient(g_hWnd, &pt);

	Vec4 terrainpos;

	terrainpos = gameInstance->TerrainPicking(pt, _transform, _viBuffer);

	RELEASE_INSTANCE(GameInstance);

	return terrainpos;
}

HRESULT EditorTerrain::Ready_Components()
{
	/* Renderer Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentRenderer"),
		TEXT("Com_Renderer"), reinterpret_cast<Component**>(&_renderComponent))))
		return E_FAIL;


	/* Shader Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::EDIT),
		TEXT("ProtoTypeComponentEditHillsShader"),
		TEXT("Com_Shader"), reinterpret_cast<Component**>(&_shader))))
		return E_FAIL;

	VIBufferTerrain::TERRAIN_DESC TerrainDesc;
	::ZeroMemory(&TerrainDesc, sizeof(TerrainDesc));

	TerrainDesc.numVerticesX = _NumVerticesX;
	TerrainDesc.numVerticesZ = _NumVerticesZ;

	/* VIBuffer Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeComponentEditVIBufferTerrain"),
		TEXT("Com_ViBuffer"), reinterpret_cast<Component**>(&_viBuffer), &TerrainDesc)))
		return E_FAIL;

	/* Transform Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("Com_Transform"), reinterpret_cast<Component**>(&_transform))))
		return E_FAIL;


	return S_OK;
}

HRESULT EditorTerrain::Bind_ShaderResources()
{
	GameInstance* gameInstance = GameInstance::GetInstance();
	Safe_AddRef<GameInstance*>(gameInstance);

	if (FAILED(_transform->BindShaderResources(_shader, "W")))
		return E_FAIL;
	if (FAILED(gameInstance->BindTransformToShader(_shader, "V", CameraHelper::TRANSFORMSTATE::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(gameInstance->BindTransformToShader(_shader, "P", CameraHelper::TRANSFORMSTATE::D3DTS_PROJ)))
		return E_FAIL;

	//if (FAILED(gameInstance->BindCameraPosition(_shader, "cameraPosition", sizeof(Vec4))))
	//	return E_FAIL;

	Safe_Release<GameInstance*>(gameInstance);

	return S_OK;
}

GameObject* EditorTerrain::Clone(void* argument)
{
	EditorTerrain* terrain = new EditorTerrain(*this);

	if (FAILED(terrain->Initialize(argument)))
	{
		MSG_BOX("Failed to Cloned : EditorTerrain");
		Safe_Release<EditorTerrain*>(terrain);
	}

	return terrain;
}

void EditorTerrain::Free()
{
	__super::Free();

	Safe_Release<Shader*>(_shader);
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
	}

	return terrain;
}

#include "pch.h"
#include "Player.h"
#include "Terrain.h"

#include "GameInstance.h"

Player::Player(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: GameObject(device, deviceContext, OBJECT_TYPE::PLAYER)
{
	_objectType = OBJECT_TYPE::PLAYER;
	_modelName = "Player";
	_modelPath = L"Player";
}

Player::Player(const Player& rhs)
	: GameObject(rhs)
{
	
}

HRESULT Player::InitializePrototype()
{
	return S_OK;
}

HRESULT Player::Initialize(void* pArg)
{
	if (FAILED(ReadyComponents()))
		return E_FAIL;

	return S_OK;
}

void Player::Tick(const _float& fTimeDelta)
{
	if (_enabled)
		return;

	//_model->PlayAnimation(fTimeDelta);
}

void Player::LateTick(const _float& fTimeDelta)
{
	if(!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

HRESULT Player::Render()
{
	if (_enabled)
		return S_OK;


	if (FAILED(BindShaderResuorces()))
		return E_FAIL;

	uint32 numMeshes = _binaryModel->GetNumMeshes();

	for (size_t i = 0; i < numMeshes; i++)
	{

		//if (FAILED(_binaryModel->BindBoneMatrices(_shader, i, "BoneMatrices")))
		//	return E_FAIL;

		if(FAILED(_binaryModel->BindMaterialTexture(_shader, "DiffuseMap", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(_shader->Begin(0)))
			return E_FAIL;

		if (FAILED(_binaryModel->Render(i)))
			return E_FAIL;
	}
	

	return S_OK;
}

_bool Player::Intersects(POINT pt)
{
	return false;
}

HRESULT Player::ReadyComponents()
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);
	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == gameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);

	/* Renderer Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentRenderer"),
		TEXT("ComponentRenderer"), reinterpret_cast<Component**>(&_render))))
		return E_FAIL;

	/* Shader Component */
	if (FAILED(__super::AddComponent(level,
		TEXT("ProtoTypeComponentDefaultMeshShader"),
		TEXT("Component_Shader"), reinterpret_cast<Component**>(&_shader))))
		return E_FAIL;

	/* Transform Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform))))
		return E_FAIL;

	/* Model Component */
	if (FAILED(__super::AddComponent(level, TEXT("ProtoTypeModelPlayer"),
		TEXT("ComponentModel"), reinterpret_cast<Component**>(&_binaryModel))))
		return E_FAIL;



	/* Collider Component */
	//if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeComponentCollider"),
	//	TEXT("ComponentCollider"), reinterpret_cast<Component**>(&_owner))))
	//	return E_FAIL;

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
	//if (FAILED(__super::AddLightComponent(level, Light::LightType::DIRECTIONAL,
	//	TEXT("ProtoTypeComponentLight"), reinterpret_cast<Component**>(&_light), &testlightinfo)))
	//	return E_FAIL;

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT Player::BindShaderResuorces()
{
	GameInstance* gameInstance = GameInstance::GetInstance();
	Safe_AddRef<GameInstance*>(gameInstance);

	if (FAILED(_transform->BindShaderResources(_shader, "W")))
		return E_FAIL;
	if (FAILED(gameInstance->BindTransformToShader(_shader, "V", CameraHelper::TRANSFORMSTATE::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(gameInstance->BindTransformToShader(_shader, "P", CameraHelper::TRANSFORMSTATE::D3DTS_PROJ)))
		return E_FAIL;

	//if (FAILED(gameInstance->BindCameraPosition(_shader, "CameraPostiion", sizeof(Vec4))))
	//	return E_FAIL;

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

	//if (FAILED(_light->BindingLightToShader(_shader, "dirLight", Light::LightType::DIRECTIONAL, sizeof(Light::DirectinoalLight))))
	//	return E_FAIL;

	Safe_Release<GameInstance*>(gameInstance);

	return S_OK;

}

Player* Player::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Player* player = new Player(device, deviceContext);

	if (FAILED(player->InitializePrototype()))
	{
		MSG_BOX("Create Player Failed : Prototype ");
		Safe_Release<Player*>(player);
		return nullptr;
	}

	return player;
}

GameObject* Player::Clone(void* argument)
{
	Player* player = new Player(*this);

	if (FAILED(player->Initialize(argument)))
	{
		MSG_BOX("Create Player Failed : Cloned ");
		Safe_Release<Player*>(player);
		return nullptr;
	}

	return player;
}

void Player::Free()
{
	__super::Free();

	Safe_Release<Shader*>(_shader);
	Safe_Release<Renderer*>(_render);
}

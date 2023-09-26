#include "pch.h"
#include "Player.h"

#include "GameInstance.h"

Player::Player(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: GameObject(device, deviceContext)
{
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
}

void Player::LateTick(const _float& fTimeDelta)
{
	_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

HRESULT Player::Render()
{
	if (FAILED(BindShaderResuorces()))
		return E_FAIL;

	_model->Render();
	

	return S_OK;
}

HRESULT Player::ReadyComponents()
{
	/* Renderer Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentRenderer"),
		TEXT("ComponentRenderer"), reinterpret_cast<Component**>(&_render))))
		return E_FAIL;

	/* Shader Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME),
		TEXT("ProtoTypeComponentHillsShader"),
		TEXT("ComponentShader"), reinterpret_cast<Component**>(&_shader))))
		return E_FAIL;

	/* Transform Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform))))
		return E_FAIL;

	/* Model Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeModelFiona"),
		TEXT("ComponentModel"), reinterpret_cast<Component**>(&_model))))
		return E_FAIL;

	

	/* Light Component */
	Light::DirectinoalLight testlightinfo;
	::ZeroMemory(&testlightinfo, sizeof(testlightinfo));
	{
		testlightinfo.Direction = Vec3(1.f, -1.f, 1.f);
		testlightinfo.Diffuse = Vec4(1.f, 1.f, 1.f, 1.f);
		testlightinfo.Ambient = Vec4(1.f, 1.f, 1.f, 1.f);
		testlightinfo.Specular = Vec4(1.f, 1.f, 1.f, 1.f);
		testlightinfo.Pad = 0.f;
	}
	if (FAILED(__super::AddLightComponent(static_cast<uint32>(LEVEL::GAME), Light::LightType::DIRECTIONAL,
		TEXT("ProtoTypeComponentLight"), reinterpret_cast<Component**>(&_light), &testlightinfo)))
		return E_FAIL;

	return S_OK;
}

HRESULT Player::BindShaderResuorces()
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

	if (FAILED(_light->BindingLightToShader(_shader, "dirLight", Light::LightType::DIRECTIONAL, sizeof(Light::DirectinoalLight))))
		return E_FAIL;

	_shader->Begin(0);

	Safe_Release<GameInstance*>(gameInstance);

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

	Safe_Release<Transform*>(_transform);
	Safe_Release<Shader*>(_shader);
	Safe_Release<Model*>(_model);
	Safe_Release<Renderer*>(_render);
	Safe_Release<Light*>(_light);
}

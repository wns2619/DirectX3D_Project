#include "pch.h"
#include "PlayerCamera.h"

#include "GameInstance.h"

PlayerCamera::PlayerCamera(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Camera(device, deviceContext)
{
	::ZeroMemory(&_playerCameraDesc, sizeof(_playerCameraDesc));
	_objectType = OBJECT_TYPE::CAMERA;
	_modelName = "EditCamera";
}

PlayerCamera::PlayerCamera(const PlayerCamera& rhs)
	: Camera(rhs)
	, _playerCameraDesc(rhs._playerCameraDesc)
	, _physics(rhs._physics)
{

}

PlayerCamera::~PlayerCamera()
{
}

HRESULT PlayerCamera::InitializePrototype()
{
	if (FAILED(ReadyComponents()))
		return E_FAIL;

	return S_OK;
}

HRESULT PlayerCamera::Initialize(void* argument)
{
	PLAYERCAMERA_DESC* playercameraDesc = static_cast<PLAYERCAMERA_DESC*>(argument);

	_playerCameraDesc._mouseSensitive = playercameraDesc->_mouseSensitive;

	if(FAILED(__super::Initialize(argument)))
		return E_FAIL;

	return S_OK;
}

void PlayerCamera::Tick(const _float& timeDelta)
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	if (gameInstance->Get_DIKeyState(DIK_A) & 0x80)
		addForce(Transform::Direction::LEFT, _transform, timeDelta);

	if (gameInstance->Get_DIKeyState(DIK_D) & 0x80)
		addForce(Transform::Direction::RIGHT, _transform, timeDelta);

	if (gameInstance->Get_DIKeyState(DIK_W) & 0x80)
		addForce(Transform::Direction::FORWARD, _transform, timeDelta);

	if (gameInstance->Get_DIKeyState(DIK_S) & 0x80)
		addForce(Transform::Direction::BACKWARD, _transform, timeDelta);


	_long mouseMove = 0l;

	if (mouseMove = gameInstance->Get_DIMouseMove(InputManager::MMS_X))
		_transform->Turn(::XMVectorSet(0.f, 1.f, 0.f, 0.f), mouseMove * _playerCameraDesc._mouseSensitive * timeDelta);

	if(mouseMove = gameInstance->Get_DIMouseMove(InputManager::MMS_Y))
		_transform->Turn(_transform->GetState(Transform::STATE::RIGHT), mouseMove * _playerCameraDesc._mouseSensitive* timeDelta);


	RELEASE_INSTANCE(GameInstance);

	__super::Tick(timeDelta);
}

void PlayerCamera::LateTick(const _float& timeDelta)
{

}

HRESULT PlayerCamera::ReadyComponents()
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);
	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == gameInstance->GetCurrentLevelIndex() - 1)
		level = static_cast<uint32>(LEVEL::EDIT);

	Physics::Physics_Desc desc;
	::ZeroMemory(&desc, sizeof(desc));
	{
		desc._acceleration = Vec3(0.1f, 0.1f, 0.1f);
		desc._deceleration = Vec3(0.001f, 0.001f, 0.001f);
		desc._mass = 60.f;
		desc._maxSpeed = 20.f;
		desc._accelMultiplier = 1.f;
		desc._accelSpeedUpMultiplier = 1.f;
		desc._decelMultiplier = 1.f;
	}


	if (FAILED(__super::AddComponent(level, TEXT("ProtoTypeComponentPhysics"),
		TEXT("ComponentPhysics"), reinterpret_cast<Component**>(&_physics), &desc)))
		return E_FAIL;


	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

void PlayerCamera::Rotate(int32 mouseX, int32 mouseY)
{
	// Rotate Camera
	Vec2 mouseDelta = Vec2((_float)mouseX, (_float)mouseY);

	// Set picth
	Vec3 rotationV3;
	::XMStoreFloat3(&rotationV3, _transform->GetWorldRotation());
	rotationV3.x += mouseDelta.y * _playerCameraDesc._mouseSensitive;

	// Limit
	_float limit = XM_PI / 2.f - 0.01f;
	rotationV3.x = max(-limit, rotationV3.x);
	rotationV3.x = min(limit, rotationV3.x);

	// Set yaw
	rotationV3.y += mouseDelta.x * _playerCameraDesc._mouseSensitive;

	if (rotationV3.x > XM_PI)
		rotationV3.y -= XM_PI * 2.0f;
	else if (rotationV3.x < XM_PI)
		rotationV3.y += XM_PI * 2.0f;

	_transform->SetWorldRotation(::XMLoadFloat3(&rotationV3));
	_transform->UpdateDirVectors();
}

void PlayerCamera::addForce(Transform::Direction direction, Transform* trans, const _float& timeDelta)
{
	_physics->addForceDir(direction, trans, timeDelta);
}

GameObject* PlayerCamera::Clone(void* argument)
{
	PlayerCamera* playerCamera = new PlayerCamera(*this);

	if (FAILED(playerCamera->Initialize(argument)))
	{
		MSG_BOX("Failed to Cloned : Player Camera");
		Safe_Release<PlayerCamera*>(playerCamera);
		return nullptr;
	}

	return playerCamera;
}

void PlayerCamera::Free()
{
	__super::Free();

	Safe_Release<Physics*>(_physics);
}

PlayerCamera* PlayerCamera::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	PlayerCamera* playerCamera = new PlayerCamera(device, deviceContext);

	if (FAILED(playerCamera->InitializePrototype()))
	{
		MSG_BOX("Failed to Create : Player Camera");
		Safe_Release<PlayerCamera*>(playerCamera);
		return nullptr;
	}

	return playerCamera;
}

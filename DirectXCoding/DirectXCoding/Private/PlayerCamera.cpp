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
	//CameraControls(timeDelta);

	//GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	//if (gameInstance->Get_DIKeyState(DIK_A) & 0x80)
	//	addForce(Transform::Direction::LEFT, _transform, timeDelta);

	//if (gameInstance->Get_DIKeyState(DIK_D) & 0x80)
	//	addForce(Transform::Direction::RIGHT, _transform, timeDelta);

	//if (gameInstance->Get_DIKeyState(DIK_W) & 0x80)
	//	addForce(Transform::Direction::FORWARD, _transform, timeDelta);

	//if (gameInstance->Get_DIKeyState(DIK_S) & 0x80)
	//	addForce(Transform::Direction::BACKWARD, _transform, timeDelta);

	//_long mouseMove = 0l;

	//if (mouseMove = gameInstance->Get_DIMouseMove(InputManager::MMS_X))
	//	_transform->Turn(::XMVectorSet(0.f, 1.f, 0.f, 0.f), mouseMove * _playerCameraDesc._mouseSensitive * timeDelta);

	//if(mouseMove = gameInstance->Get_DIMouseMove(InputManager::MMS_Y))
	//	_transform->Turn(_transform->GetState(Transform::STATE::RIGHT), mouseMove * _playerCameraDesc._mouseSensitive* timeDelta);


	//RELEASE_INSTANCE(GameInstance);

	__super::Tick(timeDelta);
}

void PlayerCamera::LateTick(const _float& timeDelta)
{

}

HRESULT PlayerCamera::ReadyComponents()
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);
	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == gameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);

	Physics::Physics_Desc desc;
	::ZeroMemory(&desc, sizeof(desc));
	{
		desc._acceleration = Vec3(1.f, 1.f, 1.f);
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

void PlayerCamera::addForce(Transform::Direction direction, Transform* trans, const _float& timeDelta, _float multiplier)
{
	_physics->addForceDir(direction, trans, timeDelta, multiplier);
}

void PlayerCamera::CameraControls(const _float& timeDelta)
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	if (!gameInstance->GetInputHandler()->keyBufferIsEmpty())
	{
		//KeyBoardEvent keyboard = gameInstance->GetInputHandler()->readKey();

		if (gameInstance->GetInputHandler()->keyIsPressed(KeyCodes::F) || gameInstance->GetInputHandler()->isMouseRightDown())
			_mouseCamerarotation = true;
		if (gameInstance->GetInputHandler()->keyIsPressed(KeyCodes::G) || gameInstance->GetInputHandler()->isMouseLeftDown())
		{
			if (_mouseCamerarotation)
			{
				while (::ShowCursor(TRUE) < 0);
				//ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
				//Application::getInstance().setClipCursor(false);
			}
			_mouseCamerarotation = false;
		}

		while (!gameInstance->GetInputHandler()->mouseBufferIsEmpty())
		{
			MouseEvent mouseEvent = gameInstance->GetInputHandler()->readMouseEvent();
			if (mouseEvent.type == MouseEventType::RawMove)
			{
				if (_mouseCamerarotation)
				{
					// Hide Cursor
					while (::ShowCursor(FALSE) >= 0);
					//ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
					// Clip To Window
					//Application::getInstance().setClipCursor(true);

					// Camera Rotation
					_transform->Rotate(mouseEvent.point.x, mouseEvent.point.y, _playerCameraDesc._mouseSensitive * timeDelta);
				}
			}
			else if (mouseEvent.type == MouseEventType::LPress)
			{
				/*if (m_selectedIndex > -1)
				{
					if (!m_dragging)
					{
						float arrowHit = m_renderHandler->selectionArrowPicking((UINT)mouseEvent.point.x, (UINT)mouseEvent.point.y, 'x');
						if (arrowHit)
						{
							m_dragging = true;
							m_origin = (-arrowHit / 2.f) - 1.f;
							m_draggingDimension = 'x';
						}

						arrowHit = m_renderHandler->selectionArrowPicking((UINT)mouseEvent.point.x, (UINT)mouseEvent.point.y, 'y');
						if (arrowHit)
						{
							m_dragging = true;
							m_origin = (-arrowHit / 2.f) - 1.f;
							m_draggingDimension = 'y';
						}

						arrowHit = m_renderHandler->selectionArrowPicking((UINT)mouseEvent.point.x, (UINT)mouseEvent.point.y, 'z');
						if (arrowHit)
						{
							m_dragging = true;
							m_origin = (-arrowHit / 2.f) - 1.f;
							m_draggingDimension = 'z';
						}
					}
				}*/
			}
			else if (mouseEvent.type == MouseEventType::LRelease)
			{
				/*if (m_dragging)
				{
					m_dragging = false;
					m_draggingDimension = 'n';
				}*/
			}
			else if (mouseEvent.type == MouseEventType::Move)
			{
				//if (m_selectedIndex > -1 && m_dragging)
				//{
				//	// Dragging Logic
				//	XMFLOAT3 objectPosition = m_gameObjects[m_selectedIndex]->getPositionF3();
				//	XMFLOAT3 rayDirection = m_renderHandler->getRayWorldDirection(mouseEvent.point.x, mouseEvent.point.y);
				//	XMFLOAT3 planeNormal;
				//	switch (m_draggingDimension)
				//	{
				//	case 'x':
				//		planeNormal = XMFLOAT3(0.f, -rayDirection.y, -rayDirection.z);
				//		break;

				//	case 'y':
				//		planeNormal = XMFLOAT3(-rayDirection.x, 0.f, -rayDirection.z);
				//		break;

				//	case 'z':
				//		planeNormal = XMFLOAT3(-rayDirection.x, -rayDirection.y, 0.f);
				//		break;

				//	default:
				//		assert(!"Error, invalid dragging dimension!");
				//		break;
				//	}
				//	SimpleMath::Plane plane(objectPosition, planeNormal);
				//	SimpleMath::Ray ray(m_camera.getPositionF3(), rayDirection);

				//	float distance = -1.f;
				//	ray.Intersects(plane, distance);
				//	SimpleMath::Vector3 rayDirectionNorm = ray.direction;
				//	rayDirectionNorm.Normalize();

				//	XMFLOAT3 endPosition = (rayDirectionNorm * distance) + ray.position;

				//	switch (m_draggingDimension)
				//	{
				//	case 'x':
				//		m_gameObjects[m_selectedIndex]->setPosition(XMVectorSet((m_origin + endPosition.x), objectPosition.y, objectPosition.z, 1.f));
				//		break;

				//	case 'y':
				//		m_gameObjects[m_selectedIndex]->setPosition(XMVectorSet(objectPosition.x, (m_origin + endPosition.y), objectPosition.z, 1.f));
				//		break;

				//	case 'z':
				//		m_gameObjects[m_selectedIndex]->setPosition(XMVectorSet(objectPosition.x, objectPosition.y, (m_origin + endPosition.z), 1.f));
				//		break;

				//	default:
				//		assert(!"Error, invalid dragging dimension!");
				//		break;
				//	}
				//	m_renderHandler->updateSelectedObject(m_gameObjects[m_selectedIndex]->getKey(), m_gameObjects[m_selectedIndex]->getPositionF3());
				//}
			}
		}

		// Camera Movement
		_float speedUpMultiplier = ((float)gameInstance->GetInputHandler()->keyIsPressed(KeyCodes::LeftShift) * 3.f) + 1.f;
		if (gameInstance->GetInputHandler()->keyIsPressed(KeyCodes::Home))
		{

		}

		if (gameInstance->GetInputHandler()->keyIsPressed(KeyCodes::W))
			addForce(Transform::Direction::FORWARD, _transform, timeDelta, speedUpMultiplier);
		

		if (gameInstance->GetInputHandler()->keyIsPressed(KeyCodes::S))
			addForce(Transform::Direction::BACKWARD, _transform, timeDelta, speedUpMultiplier);

		if (gameInstance->GetInputHandler()->keyIsPressed(KeyCodes::A))
			addForce(Transform::Direction::LEFT, _transform, timeDelta, speedUpMultiplier);

		if (gameInstance->GetInputHandler()->keyIsPressed(KeyCodes::D))
			addForce(Transform::Direction::RIGHT, _transform, timeDelta, speedUpMultiplier);

		if (gameInstance->GetInputHandler()->keyIsPressed(KeyCodes::E))
			addForce(Transform::Direction::UP, _transform, timeDelta, speedUpMultiplier);

		if (gameInstance->GetInputHandler()->keyIsPressed(KeyCodes::Q))
			addForce(Transform::Direction::DOWN, _transform, timeDelta, speedUpMultiplier);
	}

	//KeyBoardEvent keyboardEvent = gameInstance->GetInputHandler()->readKey();

	RELEASE_INSTANCE(GameInstance);
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

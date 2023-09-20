#include "pch.h"
#include "PlayerCamera.h"

#include "GameInstance.h"

PlayerCamera::PlayerCamera(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Camera(device, deviceContext)
{
	::ZeroMemory(&_playerCameraDesc, sizeof(_playerCameraDesc));
}

PlayerCamera::PlayerCamera(const PlayerCamera& rhs)
	: Camera(rhs)
	, _playerCameraDesc(rhs._playerCameraDesc)
{

}

PlayerCamera::~PlayerCamera()
{
}

HRESULT PlayerCamera::InitializePrototype()
{
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

	POINT		pt{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	::ClientToScreen(g_hWnd, &pt);
	::SetCursorPos(pt.x, pt.y);


	if (gameInstance->Get_DIKeyState(DIK_A) & 0x80)
		_transform->Left(timeDelta);

	if (gameInstance->Get_DIKeyState(DIK_D) & 0x80)
		_transform->Right(timeDelta);

	if (gameInstance->Get_DIKeyState(DIK_W) & 0x80)
		_transform->Forward(timeDelta);

	if (gameInstance->Get_DIKeyState(DIK_S) & 0x80)
		_transform->Backward(timeDelta);

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

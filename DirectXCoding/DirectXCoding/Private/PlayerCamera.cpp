#include "pch.h"
#include "PlayerCamera.h"
#include "Transform.h"
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
	

	if (::GetAsyncKeyState('W') & 0x8000)
		_transform->Forward(timeDelta);

	if (::GetAsyncKeyState('S') & 0x8000)
		_transform->Backward(timeDelta);

	if (::GetAsyncKeyState('D') & 0x8000)
		_transform->Right(timeDelta);

	if (::GetAsyncKeyState('A') & 0x8000)
		_transform->Left(timeDelta);

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

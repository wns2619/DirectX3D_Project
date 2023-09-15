#include "pch.h"
#include "PlayerCamera.h"
#include "Transform.h"
#include "GameInstance.h"

PlayerCamera::PlayerCamera(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Camera(device, deviceContext)
{

}

PlayerCamera::PlayerCamera(const PlayerCamera& rhs)
	: Camera(rhs)
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

	if (FAILED(Ready_Components()))
		return E_FAIL;

	CAMERA_DESC desc;
	::ZeroMemory(&desc, sizeof(desc));
	{
		desc._aspect = g_iWinSizeX / (_float)g_iWinSizeY;
		desc._far	= 300.f;
		desc._fov	= ::XMConvertToRadians(60.f);
		desc._near	= 0.2f;
	}


	_transform->SetState(Transform::STATE::POSITION, FXMVECTOR(::XMVectorSet(20.f,50,-100.f,1.f)));

	if (FAILED(Camera::Initialize(&desc)))
		return E_FAIL;

	

	return S_OK;
}

void PlayerCamera::Tick(const _float& timeDelta)
{
	

	if (::GetAsyncKeyState('D') & 0x8000)
		_transform->RotateY(10.f);

	if (::GetAsyncKeyState('E') & 0x8000)
		_transform->RotateY(10.f);

	if (::GetAsyncKeyState('D') & 0x8000)
		_transform->Strafe(100 * timeDelta);

	if (::GetAsyncKeyState('A') & 0x8000)
		_transform->Strafe(-100 * timeDelta);

}

void PlayerCamera::LateTick(const _float& timeDelta)
{
	__super::LateTick(timeDelta);
}

HRESULT PlayerCamera::Ready_Components()
{
	/* Transform Component */

	Transform::TRANSFORM_DESC desc;
	{
		desc.rotationRadianPerSec = ::XMConvertToRadians(50.f);
		desc.speedPerSec = 30.f;
	}

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform), &desc)))
		return E_FAIL;

	return S_OK;
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

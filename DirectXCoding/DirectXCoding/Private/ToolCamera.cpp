#include "pch.h"
#include "ToolCamera.h"
#include "GameInstance.h"

ToolCamera::ToolCamera(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Camera(device, deviceContext)
{
}

ToolCamera::ToolCamera(const ToolCamera& rhs)
	: Camera(rhs)
{

}

HRESULT ToolCamera::InitializePrototype()
{
	_modelName = "ToolCamera";
	_objectType = OBJECT_TYPE::CAMERA;

	return S_OK;
}

HRESULT ToolCamera::Initialize(void* pArg)
{
	CAMERATOOL_DESC* pCameraToolDesc = static_cast<CAMERATOOL_DESC*>(pArg);

	_mouseSensitive = pCameraToolDesc->fMouseSensitive;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void ToolCamera::Tick(const _float& timeDelta)
{
	MouseMove(timeDelta);

	__super::Tick(timeDelta);
}

void ToolCamera::LateTick(const _float& timeDelta)
{
}

void ToolCamera::MouseMove(const _float& timeDelta)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	_long _MouseMove = 0l;

	if (pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
		_transform->Left(timeDelta);

	if (pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
		_transform->Right(timeDelta);

	if (pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
		_transform->Forward(timeDelta);

	if (pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
		_transform->Backward(timeDelta);

	if (pGameInstance->Get_DIMouseState(InputManager::MOUSEKEYSTATE::MKS_RBUTTON))
	{
		if (_MouseMove = pGameInstance->Get_DIMouseMove(InputManager::MOUSEMOVESTATE::MMS_X))
			_transform->Turn(::XMVectorSet(0.f, 1.f, 0.f, 0.f), _mouseSensitive * _MouseMove * timeDelta);

		if (_MouseMove = pGameInstance->Get_DIMouseMove(InputManager::MOUSEMOVESTATE::MMS_Y))
			_transform->Turn(_transform->GetState(Transform::STATE::RIGHT), _mouseSensitive * _MouseMove * timeDelta);
	}

	RELEASE_INSTANCE(GameInstance);

}

ToolCamera* ToolCamera::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ToolCamera* camera = new ToolCamera(device, deviceContext);

	if (FAILED(camera->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : ToolCamera");
		Safe_Release<ToolCamera*>(camera);
	}

	return camera;
}

GameObject* ToolCamera::Clone(void* pArg)
{
	ToolCamera* camera = new ToolCamera(*this);

	if (FAILED(camera->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : ToolCamera");
		Safe_Release<ToolCamera*>(camera);
	}

	return camera;
}

void ToolCamera::Free()
{
	__super::Free();
}

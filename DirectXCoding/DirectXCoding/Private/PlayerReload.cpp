#include "pch.h"
#include "PlayerReload.h"
#include "GameInstance.h"
#include "Transform.h"
#include "LandObject.h"
#include "Player.h"

PlayerReload::PlayerReload(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
}

HRESULT PlayerReload::ReadyState(GameObject* pOwner)
{
	_pOwner = pOwner;

	return S_OK;
}

State::STATE PlayerReload::UpdateState(const _float& timeDelta)
{
	STATE eState = STATE::RELOAD;

	if (nullptr != _pOwner)
		eState = KeyInput(timeDelta);

	return eState;
}

void PlayerReload::LateUpdateState(const _float& timeDelta)
{

}

void PlayerReload::RenderState()
{
	std::cout << "Reload" << std::endl;

}

void PlayerReload::ChangeSetState()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	pGameInstance->StopSound(CHANNELID::SOUND_RELOAD);

	_bReload = true;

	RELEASE_INSTANCE(GameInstance);
}

State::STATE PlayerReload::KeyInput(const _float& timeDelta)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	STATE eState = STATE::RELOAD;

	LandObject* pLandObject = static_cast<LandObject*>(_pOwner);

	wstring soundfileName = L"";
	if (false == dynamic_cast<Player*>(_pOwner)->GetOnWater())
		soundfileName = TEXT("walkPlayer.wav");
	else
		soundfileName = TEXT("walkWATER.wav");

	if (pGameInstance->KeyPressing(DIK_W))
	{
		pGameInstance->PlaySound(soundfileName.c_str(), SOUND_PLAYER, 1.f);

		_pOwner->GetTransform()->Forward(timeDelta, pLandObject->GetNavigation());
		eState = STATE::RELOAD;

		_bWalk = true;
	}
	// What Animation
	else if (pGameInstance->KeyPressing(DIK_S))
	{
		pGameInstance->PlaySound(soundfileName.c_str(), SOUND_PLAYER, 1.f);

		_pOwner->GetTransform()->Backward(timeDelta, pLandObject->GetNavigation());
		eState = STATE::RELOAD;

		_bWalk = true;
	}

	if (pGameInstance->KeyPressing(DIK_A))
	{
		pGameInstance->PlaySound(soundfileName.c_str(), SOUND_PLAYER, 1.f);

		_pOwner->GetTransform()->Left(timeDelta, pLandObject->GetNavigation());
		eState = STATE::RELOAD;

		_bWalk = true;
	}
	else if (pGameInstance->KeyPressing(DIK_D))
	{
		pGameInstance->PlaySound(soundfileName.c_str(), SOUND_PLAYER, 1.f);

		_pOwner->GetTransform()->Right(timeDelta, pLandObject->GetNavigation());
		eState = STATE::RELOAD;

		_bWalk = true;
	}

	if (false == _bWalk)
		pGameInstance->StopSound(SOUND_PLAYER);

	if (true == _bReload)
	{
		pGameInstance->PlaySound(TEXT("Reload.wav"), CHANNELID::SOUND_RELOAD, 1.f);
		_bReload = false;
	}

	RELEASE_INSTANCE(GameInstance);

	return eState;
}

PlayerReload* PlayerReload::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, GameObject* pOwner)
{
	PlayerReload* pInstance = new PlayerReload(device, deviceContext);

	if (FAILED(pInstance->ReadyState(pOwner)))
	{
		MSG_BOX("Failed to Created : PlayerReload");
		Safe_Release<PlayerReload*>(pInstance);
	}

	return pInstance;
}

void PlayerReload::Free()
{
	__super::Free();
}

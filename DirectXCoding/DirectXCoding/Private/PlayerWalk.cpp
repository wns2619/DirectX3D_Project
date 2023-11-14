#include "pch.h"
#include "PlayerWalk.h"
#include "GameInstance.h"
#include "Transform.h"
#include "LandObject.h"
#include "Bullet.h"
#include "Player.h"
#include "PartObject.h"
#include "BinaryBone.h"

PlayerWalk::PlayerWalk(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
}

HRESULT PlayerWalk::ReadyState(GameObject* pOwner)
{
	_pOwner = pOwner;

	return S_OK;
}

State::STATE PlayerWalk::UpdateState(const _float& timeDelta)
{
	STATE eState = STATE::IDLE;

	_bWalk = false;

	if (nullptr != _pOwner)
		eState = KeyInput(timeDelta);

	return eState;
}

void PlayerWalk::LateUpdateState(const _float& timeDelta)
{

}

void PlayerWalk::RenderState()
{
	std::cout << "IDLE" << std::endl;

}

void PlayerWalk::ChangeSetState()
{
}

State::STATE PlayerWalk::KeyInput(const _float& timeDelta)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	STATE eState = STATE::IDLE;

	LandObject* pLandObject = static_cast<LandObject*>(_pOwner);

	// TODO
	// 물이 아니라면.
	wstring soundfileName = L"";
	if (false == dynamic_cast<Player*>(_pOwner)->GetOnWater())
		soundfileName = TEXT("walkPlayer.wav");
	else
		soundfileName = TEXT("walkWATER.wav");

	if (pGameInstance->KeyPressing(DIK_W))
	{
		pGameInstance->PlaySound(soundfileName.c_str(), SOUND_PLAYER, 1.f);

		_pOwner->GetTransform()->Forward(timeDelta, pLandObject->GetNavigation());
		eState = STATE::IDLE;

		_bWalk = true;
	}
		// What Animation
	else if (pGameInstance->KeyPressing(DIK_S))
	{
	
		pGameInstance->PlaySound(soundfileName.c_str(), SOUND_PLAYER, 1.f);

		_pOwner->GetTransform()->Backward(timeDelta, pLandObject->GetNavigation());
		eState = STATE::IDLE;

		_bWalk = true;
	}

	if (pGameInstance->KeyPressing(DIK_A))
	{
		
		pGameInstance->PlaySound(soundfileName.c_str(), SOUND_PLAYER, 1.f);

		_pOwner->GetTransform()->Left(timeDelta, pLandObject->GetNavigation());
		eState = STATE::IDLE;

		_bWalk = true;
	}
	else if (pGameInstance->KeyPressing(DIK_D))
	{
		
		pGameInstance->PlaySound(soundfileName.c_str(), SOUND_PLAYER, 1.f);

		_pOwner->GetTransform()->Right(timeDelta, pLandObject->GetNavigation());
		eState = STATE::IDLE;

		_bWalk = true;
	}


	if(true == dynamic_cast<Player*>(_pOwner)->GetIsJoom())
		if (pGameInstance->mouseDown(DIMK::DIMK_LBUTTON))
			eState = STATE::SHOOT;
	
	if (pGameInstance->keyDown(DIK_R))
		eState = STATE::RELOAD;

	if (false == _bWalk)
		pGameInstance->StopSound(SOUND_PLAYER);

	RELEASE_INSTANCE(GameInstance);

	return eState;
}

PlayerWalk* PlayerWalk::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, GameObject* pOwner)
{
	PlayerWalk* pInstance = new PlayerWalk(device, deviceContext);

	if (FAILED(pInstance->ReadyState(pOwner)))
	{
		MSG_BOX("Failed to Created : PlayerWalk");
		Safe_Release<PlayerWalk*>(pInstance);
	}

	return pInstance;
}

void PlayerWalk::Free()
{
	__super::Free();
}

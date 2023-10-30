#include "pch.h"
#include "PlayerWalk.h"
#include "GameInstance.h"
#include "Transform.h"
#include "LandObject.h"

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

State::STATE PlayerWalk::KeyInput(const _float& timeDelta)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	STATE eState = STATE::IDLE;

	LandObject* pLandObject = static_cast<LandObject*>(_pOwner);

	if (pGameInstance->KeyPressing(DIK_W))
	{
		_pOwner->GetTransform()->Forward(timeDelta, pLandObject->GetNavigation());
		eState = STATE::IDLE;
	}
		// What Animation
	else if (pGameInstance->KeyPressing(DIK_S))
	{
		_pOwner->GetTransform()->Backward(timeDelta, pLandObject->GetNavigation());
		eState = STATE::IDLE;
	}

	if (pGameInstance->KeyPressing(DIK_A))
	{
		_pOwner->GetTransform()->Left(timeDelta, pLandObject->GetNavigation());
		eState = STATE::IDLE;
	}
	else if (pGameInstance->KeyPressing(DIK_D))
	{
		_pOwner->GetTransform()->Right(timeDelta, pLandObject->GetNavigation());
		eState = STATE::IDLE;
	}


	if (pGameInstance->mouseDown(DIMK::DIMK_LBUTTON))
		eState = STATE::SHOOT;
	else if (pGameInstance->keyDown(DIK_R))
		eState = STATE::RELOAD;

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

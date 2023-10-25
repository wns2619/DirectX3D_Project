#include "pch.h"
#include "PlayerFire.h"
#include "GameInstance.h"
#include "Transform.h"
#include "LandObject.h"

PlayerFire::PlayerFire(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
}

HRESULT PlayerFire::ReadyState(GameObject* pOwner)
{
	_pOwner = pOwner;

	return S_OK;
}

State::STATE PlayerFire::UpdateState(const _float& timeDelta)
{
	STATE eState = STATE::SHOOT;

	if (nullptr != _pOwner)
		eState = KeyInput(timeDelta);

	return eState;
}

void PlayerFire::LateUpdateState(const _float& timeDelta)
{

}

void PlayerFire::RenderState()
{
	//std::cout << "Fire" << std::endl;

}

State::STATE PlayerFire::KeyInput(const _float& timeDelta)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	STATE eState = STATE::SHOOT;

	LandObject* pLandObject = static_cast<LandObject*>(_pOwner);

	if (pGameInstance->KeyPressing(DIK_W))
	{
		_pOwner->GetTransform()->Forward(timeDelta, pLandObject->GetNavigation());
		eState = STATE::SHOOT;
	}
	// What Animation
	else if (pGameInstance->KeyPressing(DIK_S))
	{
		_pOwner->GetTransform()->Backward(timeDelta, pLandObject->GetNavigation());
		eState = STATE::SHOOT;
	}

	if (pGameInstance->KeyPressing(DIK_A))
	{
		_pOwner->GetTransform()->Left(timeDelta, pLandObject->GetNavigation());
		eState = STATE::SHOOT;
	}
	else if (pGameInstance->KeyPressing(DIK_D))
	{
		_pOwner->GetTransform()->Right(timeDelta, pLandObject->GetNavigation());
		eState = STATE::SHOOT;
	}


	RELEASE_INSTANCE(GameInstance);

	return eState;
}

PlayerFire* PlayerFire::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, GameObject* pOwner)
{
	PlayerFire* pInstance = new PlayerFire(device, deviceContext);

	if (FAILED(pInstance->ReadyState(pOwner)))
	{
		MSG_BOX("Failed to Created : PlayerFire");
		Safe_Release<PlayerFire*>(pInstance);
	}

	return pInstance;
}

void PlayerFire::Free()
{
	__super::Free();
}

#include "pch.h"
#include "PlayerReload.h"
#include "GameInstance.h"
#include "Transform.h"
#include "LandObject.h"

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
}

State::STATE PlayerReload::KeyInput(const _float& timeDelta)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	STATE eState = STATE::RELOAD;

	LandObject* pLandObject = static_cast<LandObject*>(_pOwner);

	if (pGameInstance->KeyPressing(DIK_W))
	{
		_pOwner->GetTransform()->Forward(timeDelta, pLandObject->GetNavigation());
		eState = STATE::RELOAD;
	}
	// What Animation
	else if (pGameInstance->KeyPressing(DIK_S))
	{
		_pOwner->GetTransform()->Backward(timeDelta, pLandObject->GetNavigation());
		eState = STATE::RELOAD;
	}

	if (pGameInstance->KeyPressing(DIK_A))
	{
		_pOwner->GetTransform()->Left(timeDelta, pLandObject->GetNavigation());
		eState = STATE::RELOAD;
	}
	else if (pGameInstance->KeyPressing(DIK_D))
	{
		_pOwner->GetTransform()->Right(timeDelta, pLandObject->GetNavigation());
		eState = STATE::RELOAD;
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

#include "pch.h"
#include "MonsterDance.h"
#include "GameInstance.h"

MonsterDance::MonsterDance(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
}

HRESULT MonsterDance::ReadyState(GameObject* pOwner)
{
	_pOwner = pOwner;

	return S_OK;
}

State::STATE MonsterDance::UpdateState(const _float& timeDelta)
{
	State::STATE eState = STATE::IDLE;

	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	if (pGameInstance->keyDown(DIK_Z))
		eState = STATE::RUN;

	RELEASE_INSTANCE(GameInstance);

	return eState;
}

void MonsterDance::LateUpdateState(const _float& timeDelta)
{
}

void MonsterDance::RenderState()
{
}

void MonsterDance::ChangeSetState()
{
}

MonsterDance* MonsterDance::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, GameObject* pOwner)
{
	MonsterDance* pInstance = new MonsterDance(device, deviceContext);

	if (FAILED(pInstance->ReadyState(pOwner)))
	{
		MSG_BOX("Failed to Created : MonsterDance");
		Safe_Release<MonsterDance*>(pInstance);
	}

	return pInstance;

}

void MonsterDance::Free()
{
	__super::Free();
}

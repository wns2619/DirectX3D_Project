#include "pch.h"
#include "MonsterRun.h"
#include "GameInstance.h"

MonsterRun::MonsterRun(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
}

HRESULT MonsterRun::ReadyState(GameObject* pOwner)
{
	_pOwner = pOwner;

	return S_OK;
}

State::STATE MonsterRun::UpdateState(const _float& timeDelta)
{
	State::STATE eState = STATE::RUN;

	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	if (pGameInstance->keyDown(DIK_Z))
		eState = STATE::IDLE;

	RELEASE_INSTANCE(GameInstance);

	return eState;
}

void MonsterRun::LateUpdateState(const _float& timeDelta)
{
}

void MonsterRun::RenderState()
{
}

void MonsterRun::ChangeSetState()
{
}

MonsterRun* MonsterRun::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, GameObject* pOwner)
{
	MonsterRun* pInstance = new MonsterRun(device, deviceContext);

	if (FAILED(pInstance->ReadyState(pOwner)))
	{
		MSG_BOX("Failed to Created : MonsterRun");
		Safe_Release<MonsterRun*>(pInstance);
	}

	return pInstance;

}

void MonsterRun::Free()
{
	__super::Free();
}

#include "pch.h"
#include "MonsterWalk.h"
#include "GameInstance.h"

MonsterWalk::MonsterWalk(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
}

HRESULT MonsterWalk::ReadyState(GameObject* pOwner)
{
	_pOwner = pOwner;

	return S_OK;
}

State::STATE MonsterWalk::UpdateState(const _float& timeDelta)
{
	State::STATE eState = STATE::RUN;

	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	if (pGameInstance->keyDown(DIK_Z))
		eState = STATE::RUN;

	RELEASE_INSTANCE(GameInstance);

	return eState;
}

void MonsterWalk::LateUpdateState(const _float& timeDelta)
{
}

void MonsterWalk::RenderState()
{
}

MonsterWalk* MonsterWalk::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, GameObject* pOwner)
{
	MonsterWalk* pInstance = new MonsterWalk(device, deviceContext);

	if (FAILED(pInstance->ReadyState(pOwner)))
	{
		MSG_BOX("Failed to Created : MonsterWalk");
		Safe_Release<MonsterWalk*>(pInstance);
	}

	return pInstance;

}

void MonsterWalk::Free()
{
	__super::Free();
}

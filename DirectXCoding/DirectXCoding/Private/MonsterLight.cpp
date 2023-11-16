#include "pch.h"
#include "MonsterLight.h"
#include "GameInstance.h"

MonsterLight::MonsterLight(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: MonsterPartObject(pDevice, pDeviceContext)
{
}

MonsterLight::MonsterLight(const MonsterLight& rhs)
	: MonsterPartObject(rhs)
{
}

HRESULT MonsterLight::InitializePrototype()
{
	return S_OK;
}

HRESULT MonsterLight::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		MONSTER_FLASH* pPartDesc = static_cast<MONSTER_FLASH*>(pArg);

		_mSocketPivotMatrix = pPartDesc->mSocketPivot;
		_pSocketMatrix = pPartDesc->pSocketMatrix;

		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}

	if (FAILED(ReadyComponents()))
		return E_FAIL;



	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	
	vector<OtherLight*>* flashLight = pGameInstance->getLightList();

	auto iter = [&](OtherLight* pLight)->OtherLight*
		{
			if (pLight->GetLightDesc()->MonsterLight == true)
				return pLight;

			return nullptr;
		};

	auto finditer = find_if(flashLight->begin(), flashLight->end(), iter);

	if (finditer != flashLight->end())
		_pLight = *finditer;


	RELEASE_INSTANCE(GameInstance);


	return S_OK;
}

void MonsterLight::Tick(const _float& fTimeDelta)
{
	XMMATRIX worldMatrix = (*_pSocketMatrix) * _mSocketPivotMatrix;

	Compute_RenderMatrix(_transform->GetWorldMatrix() * worldMatrix);

	if (nullptr != _pLight)
	{
		Vec3 vLook = _pParentTransform->GetState(Transform::STATE::LOOK);
		vLook.Normalize();

		Vec4 FinalPos = Vec4(_mWorldMatrix._41, _mWorldMatrix._42, _mWorldMatrix._43, 1.f);


		_pLight->GetLightDesc()->Position = FinalPos;
		::XMStoreFloat3(&_pLight->GetLightDesc()->Direction, -vLook);
	}
}

void MonsterLight::LateTick(const _float& fTimeDelta)
{
}

HRESULT MonsterLight::Render()
{
	return S_OK;
}

HRESULT MonsterLight::ReadyComponents()
{

	/* Transform Component */
	Transform::TRANSFORM_DESC transformDesc;
	transformDesc.speedPerSec = 5.f;
	transformDesc.rotationRadianPerSec = ::XMConvertToRadians(40.f);

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform), &transformDesc)))
		return E_FAIL;


	return S_OK;
}

MonsterLight* MonsterLight::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	MonsterLight* pInstance = new MonsterLight(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : MonsterLight");
		Safe_Release<MonsterLight*>(pInstance);
	}

	return pInstance;
}

GameObject* MonsterLight::Clone(void* pArg)
{
	MonsterLight* pInstance = new MonsterLight(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : MonsterLight");
		Safe_Release<MonsterLight*>(pInstance);
	}

	return pInstance;
}

void MonsterLight::Free()
{
	__super::Free();
}

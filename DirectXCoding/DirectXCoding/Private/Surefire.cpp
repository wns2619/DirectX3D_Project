#include "pch.h"
#include "Surefire.h"
#include "GameInstance.h"

Surefire::Surefire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: PartObject(pDevice, pContext)
{
	_modelName = "Surefire";
}

Surefire::Surefire(const Surefire& rhs)
	: PartObject(rhs)
{

}

HRESULT Surefire::InitializePrototype()
{
	return S_OK;
}

HRESULT Surefire::Initialize(void* pArg)
{
	if (nullptr != pArg)
	{
		FLASHLIGHT_DESC* pPartDesc = static_cast<FLASHLIGHT_DESC*>(pArg);

		m_SocketPivotMatrix = pPartDesc->SocketPivot;
		m_SocketMatrix = pPartDesc->SocketMatrix;

		if (FAILED(__super::Initialize(pArg)))
			return E_FAIL;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//_transform->FixRotation(Vec3(0.f, -1.f, 0.f), ::XMConvertToRadians(180));



	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	vector<OtherLight*>* flashLight = pGameInstance->getLightList();

	auto iter = [&](OtherLight* pLight)->OtherLight*
		{
			if (pLight->GetLightDesc()->PlayerLight == true)
				return pLight;

			return nullptr;
		};

	auto finditer = find_if(flashLight->begin(), flashLight->end(), iter);

	if (finditer != flashLight->end())
		_pLight = *finditer;




	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

void Surefire::Tick(const _float& fTimeDelta)
{
	XMMATRIX worldMatrix = ::XMLoadFloat4x4(m_SocketMatrix) * ::XMLoadFloat4x4(&m_SocketPivotMatrix);

	worldMatrix.r[0] = XMVector3Normalize(worldMatrix.r[0]);
	worldMatrix.r[1] = XMVector3Normalize(worldMatrix.r[1]);
	worldMatrix.r[2] = XMVector3Normalize(worldMatrix.r[2]);

	Compute_RenderMatrix(_transform->GetWorldMatrix() * worldMatrix);

	if (nullptr != _pLight)
	{
		Vec3 vLook = _pParentTransform->GetState(Transform::STATE::LOOK);
		vLook.Normalize();

		Vec4 vPosition = _pParentTransform->GetState(Transform::STATE::POSITION);

		_pLight->GetLightDesc()->Position = vPosition;
		::XMStoreFloat3(&_pLight->GetLightDesc()->Direction, -vLook);
	}
}

void Surefire::LateTick(const _float& fTimeDelta)
{

}

HRESULT Surefire::Render()
{
	if (_enabled)
		return S_OK;



	return S_OK;
}

HRESULT Surefire::Ready_Components()
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


Surefire* Surefire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	Surefire* pInstance = new Surefire(pDevice, pContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : Surefire");
		Safe_Release<Surefire*>(pInstance);
	}

	return pInstance;
}

GameObject* Surefire::Clone(void* pArg)
{
	Surefire* pInstance = new Surefire(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : Surefire");
		Safe_Release<Surefire*>(pInstance);
	}

	return pInstance;
}

void Surefire::Free()
{
	__super::Free();

}

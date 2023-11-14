#include "pch.h"
#include "EventMainDoor.h"

#include "GameInstance.h"
#include "BoundingOBB.h"
#include "TrigerBox.h"

EventMainDoor::EventMainDoor(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: DynamicObject(device, deviceContext, DYNAMIC_TYPE::EVENT_DOOR)
{

}

EventMainDoor::EventMainDoor(const EventMainDoor& rhs)
	: DynamicObject(rhs)
{
}

HRESULT EventMainDoor::InitializePrototype()
{
	return S_OK;
}

HRESULT EventMainDoor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyCollider()))
		return E_FAIL;

	if (nullptr != pArg)
		_transform->SetWorldMatrix(static_cast<ComponentNames*>(pArg)->_saveWorldMatrix);



	return S_OK;
}

void EventMainDoor::Tick(const _float& timeDelta)
{
	if (_enabled)
		return;

}

void EventMainDoor::LateTick(const _float& timeDelta)
{
	__super::LateTick(timeDelta);
}

HRESULT EventMainDoor::Render()
{
	if (_enabled)
		return S_OK;

	if (FAILED(__super::BindShaderResource()))
		return E_FAIL;

	uint32 numMeshes = _binaryModel->GetNumMeshes();

	for (size_t i = 0; i < numMeshes; i++)
	{
		if (FAILED(_binaryModel->BindMaterialTexture(_shader, "DiffuseMap", i, TextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(_shader->Begin(0)))
			return E_FAIL;

		if (FAILED(_binaryModel->Render(i)))
			return E_FAIL;
	}

	return S_OK;

}

void EventMainDoor::OnCollisionEnter(Collider* pOther)
{
}

void EventMainDoor::OnCollisionStay(Collider* pOther)
{

}

void EventMainDoor::OnCollisionExit(Collider* pOther)
{
}

HRESULT EventMainDoor::ReadyCollider()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);


	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == pGameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);


	Transform::TRANSFORM_DESC transDesc;
	{
		transDesc.speedPerSec = 0.f;
		transDesc.rotationRadianPerSec = ::XMConvertToRadians(30.f);
	}


	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform), &transDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

EventMainDoor* EventMainDoor::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	EventMainDoor* pInstance = new EventMainDoor(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Create to Failed : EventMainDoor");
		Safe_Release<EventMainDoor*>(pInstance);
	}

	return pInstance;
}

GameObject* EventMainDoor::Clone(void* argument)
{
	EventMainDoor* pInstance = new EventMainDoor(*this);

	if (FAILED(pInstance->Initialize(argument)))
	{
		MSG_BOX("Create to Failed : BreakDoor");
		Safe_Release<EventMainDoor*>(pInstance);
	}

	return pInstance;
}

void EventMainDoor::Free()
{
	__super::Free();
}

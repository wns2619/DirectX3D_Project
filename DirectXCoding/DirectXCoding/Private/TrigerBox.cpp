#include "pch.h"
#include "TrigerBox.h"

#include "GameInstance.h"
#include "Bounding.h"
#include "FileUtils.h"
#include "BoundingAABB.h"
#include "DynamicObject.h"
#include "EventMainDoor.h"

TrigerBox::TrigerBox(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: StaticObject(device, deviceContext)
{
	_modelName = "TrigerBox";
	_comNames._protoTypeTag = TEXT("ProtoTypeTrigerBox");
}

TrigerBox::TrigerBox(const TrigerBox& rhs)
	: StaticObject(rhs)
{

}

HRESULT TrigerBox::InitializePrototype()
{

	return S_OK;
}

HRESULT TrigerBox::Initialize(void* pArg)
{
	if (FAILED(ReadyComponents()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		_id = static_cast<ComponentNames*>(pArg)->_modelID;
		_transform->SetWorldMatrix(static_cast<ComponentNames*>(pArg)->_saveWorldMatrix);
	}

	EventTarget();

	return S_OK;
}

void TrigerBox::PriorityTick(const _float& timeDelta)
{
}

void TrigerBox::Tick(const _float& timeDelta)
{
	if (_id == 230 && true == _bTrigerOn)
	{

		if (_iTurnCount < 120)
		{
			++_iTurnCount;
			dynamic_cast<EventMainDoor*>(_pTargetObject)->GetTransform()->Turn(Vec4(0.f, 1.f, 0.f, 1.f), timeDelta);
		}
	}



	_pCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());
}

void TrigerBox::LateTick(const _float& timeDelta)
{
	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

HRESULT TrigerBox::Render()
{
#ifdef _DEBUG
	_pCollider->Render();
#endif // _DEBUG


	return S_OK;
}

void TrigerBox::OnCollisionEnter(Collider* pOther)
{
}

void TrigerBox::OnCollisionStay(Collider* pOther)
{

}

void TrigerBox::OnCollisionExit(Collider* pOther)
{

}

void TrigerBox::EventTarget()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	vector<GameObject*>* pGameList = pGameInstance->GetCurrentObjectList(LAYER_TAG::LAYER_DYNAMIC);

	if (_id == 230)
	{
		auto iter = find_if(pGameList->begin(), pGameList->end(), [&](GameObject* pObject)
			{
				if (pObject->GetIdNumber() == 196)
					return true;

				return false;
			});

		if (nullptr == (*iter))
			return;

		_pTargetObject = *iter;
	}

	RELEASE_INSTANCE(GameInstance);
}

HRESULT TrigerBox::ReadyComponents()
{

	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == pGameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentRenderer"),
		TEXT("ComponentRenderer"), reinterpret_cast<Component**>(&_render))))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform))))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentVIBufferRect"),
		TEXT("ComponentVIBufferRect"), reinterpret_cast<Component**>(&_pVIBufferRect))))
		return E_FAIL;


	BoundingAABB::BOUNDING_AABB_DESC aabbDesc;
	{
		aabbDesc.vCenter = Vec3(0.f, 0.f, 0.f);
		aabbDesc.vExtents = Vec3(1.f, 1.f, 1.f);
		aabbDesc.pOwner = this;
	}

	if (FAILED(__super::AddComponent(level, TEXT("ProtoTypeAABBCollider"),
		TEXT("ComponentCollider"), reinterpret_cast<Component**>(&_pCollider), &aabbDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

TrigerBox* TrigerBox::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	TrigerBox* pInstance = new TrigerBox(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : TrigerBox");
		Safe_Release<TrigerBox*>(pInstance);
	}

	return pInstance;
}

GameObject* TrigerBox::Clone(void* pArg)
{
	TrigerBox* pInstance = new TrigerBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : TrigerBox");
		Safe_Release<TrigerBox*>(pInstance);
	}

	return pInstance;
}

void TrigerBox::Free()
{
	__super::Free();

	Safe_Release<VIBufferRect*>(_pVIBufferRect);
}

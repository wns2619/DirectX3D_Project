#include "pch.h"
#include "DoorCollision.h"

#include "GameInstance.h"
#include "Bounding.h"
#include "FileUtils.h"
#include "BoundingAABB.h"
#include "DynamicObject.h"

DoorCollision::DoorCollision(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: StaticObject(device, deviceContext)
{
	_modelName = "DoorCollision";
	_comNames._protoTypeTag = TEXT("ProtoTypeDoorCol");
}

DoorCollision::DoorCollision(const DoorCollision& rhs)
	: StaticObject(rhs)
{

}

HRESULT DoorCollision::InitializePrototype()
{

	return S_OK;
}

HRESULT DoorCollision::Initialize(void* pArg)
{
	if (FAILED(ReadyComponents()))
		return E_FAIL;

	if (nullptr != pArg)
		_transform->SetWorldMatrix(static_cast<ComponentNames*>(pArg)->_saveWorldMatrix);

	return S_OK;
}

void DoorCollision::PriorityTick(const _float& timeDelta)
{
}

void DoorCollision::Tick(const _float& timeDelta)
{
	_pCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());
}

void DoorCollision::LateTick(const _float& timeDelta)
{
	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

HRESULT DoorCollision::Render()
{
#ifdef _DEBUG
	_pCollider->Render();
#endif // _DEBUG


	return S_OK;
}

void DoorCollision::OnCollisionEnter(Collider* pOther)
{
}

void DoorCollision::OnCollisionStay(Collider* pOther)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	Vec3 vPlayerCenter = static_cast<Bounding_Sphere*>(pOther->GetBounding())->GetBounding()->Center;
	Vec3 vthisCenter = static_cast<BoundingAABB*>(_pCollider->GetBounding())->GetBounding()->Center;

	Vec3 vFinalCenter = vPlayerCenter - vthisCenter;
	Vec3 extents = 0.5f * Vec3(::fabs(vFinalCenter.x), ::fabs(vFinalCenter.y), ::fabs(vFinalCenter.z));

	if (extents.x >= extents.y && extents.x >= extents.z)
		dynamic_cast<DynamicObject*>(pOther->GetOwner())->SetOpen(true);
	else if (extents.y >= extents.x && extents.y >= extents.z)
		dynamic_cast<DynamicObject*>(pOther->GetOwner())->SetOpen(true);
	else
		dynamic_cast<DynamicObject*>(pOther->GetOwner())->SetOpen(true);


	RELEASE_INSTANCE(GameInstance);
}

void DoorCollision::OnCollisionExit(Collider* pOther)
{

}

HRESULT DoorCollision::ReadyComponents()
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
		aabbDesc.vCenter = Vec3(0.f, 120.f, 0.f);
		aabbDesc.vExtents = Vec3(80.f, 120.f, 5.f);
		aabbDesc.pOwner = this;
	}

	if (FAILED(__super::AddComponent(level, TEXT("ProtoTypeAABBCollider"),
		TEXT("ComponentCollider"), reinterpret_cast<Component**>(&_pCollider), &aabbDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

DoorCollision* DoorCollision::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	DoorCollision* pInstance = new DoorCollision(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : DoorCollision");
		Safe_Release<DoorCollision*>(pInstance);
	}

	return pInstance;
}

GameObject* DoorCollision::Clone(void* pArg)
{
	DoorCollision* pInstance = new DoorCollision(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : DoorCollision");
		Safe_Release<DoorCollision*>(pInstance);
	}

	return pInstance;
}

void DoorCollision::Free()
{
	__super::Free();

	Safe_Release<VIBufferRect*>(_pVIBufferRect);
}

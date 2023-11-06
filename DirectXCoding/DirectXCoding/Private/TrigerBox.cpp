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

	TrigerOccur(timeDelta);

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
	//GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	//if (pOther->GetOwner()->GetObjectType() == OBJECT_TYPE::PLAYER && _id == 236)
	//{

	//	Vec3 vPlayerCenter = static_cast<BoundingAABB*>(pOther->GetBounding())->GetBounding()->Center;
	//	Vec3 vthisCenter = static_cast<BoundingAABB*>(_pCollider->GetBounding())->GetBounding()->Center;


	//	Vec3 vFinalCenter = vPlayerCenter - vthisCenter;

	//	Vec3 vPlayerExtents = static_cast<BoundingAABB*>(pOther->GetBounding())->GetBounding()->Extents;
	//	Vec3 vThisExtents = static_cast<BoundingAABB*>(_pCollider->GetBounding())->GetBounding()->Extents;

	//	Vec3 extents = 0.5f * Vec3(::fabs(vFinalCenter.x), ::fabs(vFinalCenter.y), ::fabs(vFinalCenter.z));

	//	if (extents.x >= extents.y && extents.x >= extents.z)
	//	{
	//		// 충돌이 X 축에서 발생.

	//		// TODO -> Left or Right

	//		if (vPlayerCenter.x > vthisCenter.x)
	//		{
	//			_float vFinalExtents = fabs((vPlayerExtents.x + vThisExtents.x)) - fabs(vFinalCenter.x);

	//			Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
	//			vPos.x += vFinalExtents;

	//			pOther->GetOwner()->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
	//		}
	//		else
	//		{
	//			_float vFinalExtents = fabs((vPlayerExtents.x + vThisExtents.x)) - fabs(vFinalCenter.x);

	//			Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
	//			vPos.x -= vFinalExtents;

	//			pOther->GetOwner()->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
	//		}
	//	}
	//	else if (extents.y >= extents.x && extents.y >= extents.z)
	//	{
	//		if (vPlayerCenter.y > vthisCenter.y)
	//		{
	//			if (_id == 205)
	//			{
	//				_float vFinalExtents = fabs((vPlayerExtents.x + vThisExtents.x)) - fabs(vFinalCenter.x);

	//				Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
	//				vPos.x -= vFinalExtents;

	//				pOther->GetOwner()->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
	//			}
	//			else
	//			{
	//				_float vFinalExtents = fabs((vPlayerExtents.z + vThisExtents.z)) - fabs(vFinalCenter.z);

	//				Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
	//				vPos.z -= vFinalExtents;

	//				pOther->GetOwner()->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
	//			}

	//		}
	//		else
	//		{
	//			if (_id == 205)
	//			{
	//				_float vFinalExtents = fabs((vPlayerExtents.x + vThisExtents.x)) - fabs(vFinalCenter.x);

	//				Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
	//				vPos.x += vFinalExtents;

	//				pOther->GetOwner()->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
	//			}
	//			else
	//			{
	//				_float vFinalExtents = fabs((vPlayerExtents.z + vThisExtents.z)) - fabs(vFinalCenter.z);

	//				Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
	//				vPos.z += vFinalExtents;

	//				pOther->GetOwner()->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
	//			}

	//		}
	//	}
	//	else
	//	{
	//		// Z축에서 발생. 

	//		// TODO -> Back or Front.
	//		if (vPlayerCenter.z > vthisCenter.z)
	//		{
	//			_float vFinalExtents = fabs((vPlayerExtents.z + vThisExtents.z)) - fabs(vFinalCenter.z);

	//			Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
	//			vPos.z += vFinalExtents;

	//			pOther->GetOwner()->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
	//		}
	//		else
	//		{
	//			_float vFinalExtents = fabs((vPlayerExtents.z + vThisExtents.z)) - fabs(vFinalCenter.z);

	//			Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
	//			vPos.z -= vFinalExtents;

	//			pOther->GetOwner()->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
	//		}
	//	}
	//}


	//RELEASE_INSTANCE(GameInstance);
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

void TrigerBox::LerpSoundPlayer(_float& fVolume, _float& fDistance, _float fMaxDistance, GameInstance* pGameInstance)
{
	const _float fMaxVolume = 0.3f;
	const _float fMinVolume = 0.f;

	Vec4 vThisPos = _transform->GetState(Transform::STATE::POSITION);

	uint32 iCurrentLevel = pGameInstance->GetCurrentLevelIndex();

	Transform* pPlayerTransform = dynamic_cast<Transform*>(pGameInstance->GetComponent(iCurrentLevel, LAYER_TAG::LAYER_PLAYER, TEXT("ComponentTransform"), "Player"));

	Vec4 vPlayerPos = pPlayerTransform->GetState(Transform::STATE::POSITION);

	Vec4 vDir = vPlayerPos - vThisPos;
	fDistance = vDir.Length();

	fVolume = fMaxVolume - (fDistance / fMaxDistance) * (fMaxVolume - fMinVolume);

	if (fVolume <= 0.f)
		fVolume = 0.f;
}

void TrigerBox::TrigerOccur(const _float& timeDelta)
{
	if (_id == 230 && true == _bTrigerOn)
	{

		if (_iTurnCount < 120)
		{
			++_iTurnCount;
			dynamic_cast<EventMainDoor*>(_pTargetObject)->GetTransform()->Turn(Vec4(0.f, 1.f, 0.f, 1.f), timeDelta);
		}
	}


	if (_id == 231 && true == _bTrigerOn)
	{
		_fEventSound += timeDelta;

		if (_fEventSound <= 2.0f)
		{

			if (_fEventSound >= 1.6f)
			{
				GameInstance* pGameInstance = GET_INSTANCE(GameInstance);


				if (false == _bEventState)
				{
					pGameInstance->StopSound(SOUND_SCARE2);
					pGameInstance->StopSound(SOUND_SCARE3);
				}

				pGameInstance->PlaySound(TEXT("Low Bell.wav"), SOUND_SCARE2, 1.f);
				pGameInstance->PlaySound(TEXT("LampBreak.wav"), SOUND_SCARE3, 1.f);
				_bEventState = true;


				RELEASE_INSTANCE(GameInstance);
			}
		}
	}

	if (_id == 236 && true == _bTrigerOn)
	{

		if (_fEventSound <= 2.3f)
		{
			_fEventSound += timeDelta;

			if (_fEventSound >= 2.f)
			{
				GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

				if (false == _bEventState)
					pGameInstance->StopSound(SOUND_SCARE2);

				pGameInstance->PlaySound(TEXT("Very Low Growling.wav"), SOUND_SCARE2, 1.f);
				_bEventState = true;


				RELEASE_INSTANCE(GameInstance);
			}
		}
	}

	if (_id == 237 || _id == 238 || _id == 239)
	{
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

		_float fVolume = 0.f;
		_float fDistance = 5.f;

		LerpSoundPlayer(fVolume, fDistance, 2.5f, pGameInstance);

		if (fDistance <= 5.f)
			pGameInstance->PlaySoundLoop(TEXT("kapaet.wav"), SOUND_ENVIRONMENT2, fVolume);

		RELEASE_INSTANCE(GameInstance);
	}

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

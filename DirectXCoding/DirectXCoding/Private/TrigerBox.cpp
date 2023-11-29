#include "pch.h"
#include "TrigerBox.h"

#include "GameInstance.h"
#include "Bounding.h"
#include "FileUtils.h"
#include "BoundingAABB.h"
#include "DynamicObject.h"
#include "EventMainDoor.h"

#include "PlayerBody.h"
#include "Player.h"
#include "Monster.h"
#include "MonsterLight.h"
#include "PlayerBody.h"

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

#ifdef _DEBUG
	_render->AddDebug(_pCollider);
#endif // _DEBUG


}

HRESULT TrigerBox::Render()
{
	return S_OK;
}

void TrigerBox::OnCollisionEnter(Collider* pOther)
{
}

void TrigerBox::OnCollisionStay(Collider* pOther)
{
	if (pOther->GetOwner()->GetObjectType() == OBJECT_TYPE::PLAYER && _id == 256)
	{
		GameObject* pPart = static_cast<Player*>(pOther->GetOwner())->GetPlyaerPart()[Player::PART_BODY];
		const _bool& IsLight = static_cast<PlayerBody*>(pPart)->IsObtainingLight();

		if (false == IsLight)
		{
			Player* pPlayer = static_cast<Player*>(pOther->GetOwner());
			pPlayer->SetCollDynamic(true);
			pPlayer->SetEventText(TEXT("Need a Flash"));


			Vec3 vPlayerCenter = static_cast<BoundingAABB*>(pOther->GetBounding())->GetBounding()->Center;
			Vec3 vthisCenter = static_cast<BoundingAABB*>(_pCollider->GetBounding())->GetBounding()->Center;


			Vec3 vFinalCenter = vPlayerCenter - vthisCenter;

			Vec3 vPlayerExtents = static_cast<BoundingAABB*>(pOther->GetBounding())->GetBounding()->Extents;
			Vec3 vThisExtents = static_cast<BoundingAABB*>(_pCollider->GetBounding())->GetBounding()->Extents;

			Vec3 extents = 0.5f * Vec3(::fabs(vFinalCenter.x), ::fabs(vFinalCenter.y), ::fabs(vFinalCenter.z));

			if (extents.x >= extents.y && extents.x >= extents.z)
			{
				// 충돌이 X 축에서 발생.

				// TODO -> Left or Right

				if (vPlayerCenter.x > vthisCenter.x)
				{
					_float vFinalExtents = fabs((vPlayerExtents.x + vThisExtents.x)) - fabs(vFinalCenter.x);

					Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
					vPos.x += vFinalExtents;

					pOther->GetOwner()->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
				}
				else
				{
					_float vFinalExtents = fabs((vPlayerExtents.x + vThisExtents.x)) - fabs(vFinalCenter.x);

					Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
					vPos.x -= vFinalExtents;

					pOther->GetOwner()->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
				}
			}
			else if (extents.y >= extents.x && extents.y >= extents.z)
			{
				if (vPlayerCenter.y > vthisCenter.y)
				{
				}
				else
				{

				}
			}
			else
			{
				// Z축에서 발생. 

				// TODO -> Back or Front.
				if (vPlayerCenter.z > vthisCenter.z)
				{
					_float vFinalExtents = fabs((vPlayerExtents.z + vThisExtents.z)) - fabs(vFinalCenter.z);

					Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
					vPos.z += vFinalExtents;

					pOther->GetOwner()->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
				}
				else
				{
					_float vFinalExtents = fabs((vPlayerExtents.z + vThisExtents.z)) - fabs(vFinalCenter.z);

					Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
					vPos.z -= vFinalExtents;

					pOther->GetOwner()->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
				}
			}
		}
	}
}

void TrigerBox::OnCollisionExit(Collider* pOther)
{
	if (256 == _id)
	{
		Player* pPlayer = static_cast<Player*>(pOther->GetOwner());
		pPlayer->SetCollDynamic(false);
	}
}

void TrigerBox::EventTarget()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	vector<GameObject*>* pGameList = pGameInstance->GetCurrentObjectList(LAYER_TAG::LAYER_DYNAMIC);

	if (_id == 232)
	{
		auto iter = find_if(pGameList->begin(), pGameList->end(), [&](GameObject* pObject)
			{
				if (pObject->GetIdNumber() == 197)
					return true;

				return false;
			});

		if (nullptr == (*iter))
		{
			RELEASE_INSTANCE(GameInstance);
			return;
		}

		_pTargetObject = *iter;
	}

	RELEASE_INSTANCE(GameInstance);
}

void TrigerBox::LerpSoundPlayer(_float& fVolume, _float& fDistance, _float fMaxDistance, GameInstance* pGameInstance, _float fMaxSound)
{
	const _float fMinVolume = 0.f;

	Vec4 vThisPos = _transform->GetState(Transform::STATE::POSITION);

	uint32 iCurrentLevel = pGameInstance->GetCurrentLevelIndex();

	Transform* pPlayerTransform = static_cast<Transform*>(pGameInstance->GetComponent(iCurrentLevel, LAYER_TAG::LAYER_PLAYER, TEXT("ComponentTransform"), "Player"));

	Vec4 vPlayerPos = pPlayerTransform->GetState(Transform::STATE::POSITION);

	Vec4 vDir = vPlayerPos - vThisPos;
	fDistance = vDir.Length();

	fVolume = fMaxSound - (fDistance / fMaxDistance) * (fMaxSound - fMinVolume);

	if (fVolume <= 0.f)
		fVolume = 0.f;
}

void TrigerBox::TrigerOccur(const _float& timeDelta)
{
	if (_id == 232 && true == _bTrigerOn)
	{

		if (_iTurnCount < 120)
		{
			++_iTurnCount;
			static_cast<EventMainDoor*>(_pTargetObject)->GetTransform()->Turn(Vec4(0.f, 1.f, 0.f, 1.f), timeDelta);
		}
	}


	if (_id == 233 && true == _bTrigerOn)
	{
		_fEventSound += timeDelta;

		if (_fEventSound <= 2.0f)
		{

			if (_fEventSound >= 1.f)
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

				uint32 iTurnOffLight[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
				pGameInstance->SelectTurnOffLight(iTurnOffLight, 9);


				RELEASE_INSTANCE(GameInstance);
			}
		}
	}

	if (_id == 234 && true == _bTrigerOn && false == _bEventState)
	{
		LandObject::LANDOBJET_DESC LandObjectDesc = {};

		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

		Transform* pTransform = static_cast<Transform*>(pGameInstance->GetComponent(static_cast<uint32>(LEVEL::GAME), LAYER_TAG::LAYER_STATIC, TEXT("ComponentTransform"), "2stBottom", 0));
		BinaryNavi* pNavi = static_cast<BinaryNavi*>(pGameInstance->GetComponent(static_cast<uint32>(LEVEL::GAME), LAYER_TAG::LAYER_STATIC, TEXT("ComponentNavigation"), "2stBottom", 0));

		vector<Cell*>& vecCells = pNavi->GetCell();

		LandObjectDesc.pCells = &vecCells;
		LandObjectDesc.pCellTransform = pTransform;

		Monster* pGameObject = static_cast<Monster*>(pGameInstance->CloneGameObject(TEXT("ProtoTypeDanceMonster"), &LandObjectDesc));
		pGameObject->GetTransform()->SetState(Transform::STATE::POSITION, Vec4(4.086, 0.f, -9.718, 1.f));
		pGameObject->GetNavigation()->SetCurrentIndex(289);
		pGameObject->GetStateMachine()->SetState(State::STATE::RUN);
		pGameObject->GetTransform()->SetSpeedPerSec(5.f);

		pGameInstance->CreateObject(pGameObject, LAYER_TAG::LAYER_MONSTER);
		RELEASE_INSTANCE(GameInstance);

		_bEventState = true;
	}

	if (_id == 236 && true == _bTrigerOn && false == _bEventState)
	{
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

		if (false == _bEventState)
			pGameInstance->StopSound(SOUND_MONSTER2);

		pGameInstance->PlaySound(TEXT("Scream.wav"), SOUND_MONSTER2, 1.f);

		uint32 pLight[4] = { 7, 8, 9, 10 };
		pGameInstance->SelectTurnOffLight(pLight, 4);

		Player* pPlayer = static_cast<Player*>(pGameInstance->GetLayerObjectTag(LAYER_TAG::LAYER_PLAYER, "Player"));
		pPlayer->SetScare(true);

		_bEventState = true;

		RELEASE_INSTANCE(GameInstance);
	}

	if (_id == 238 && true == _bTrigerOn)
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

	if (_id == 239 || _id == 240 || _id == 241)
	{
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

		_float fVolume = 0.f;
		_float fDistance = 0.f;

		LerpSoundPlayer(fVolume, fDistance, 3.2f, pGameInstance);

		if (fDistance <= 3.2f)
			pGameInstance->PlaySoundLoop(TEXT("kapaet.wav"), SOUND_ENVIRONMENT2, fVolume);

		RELEASE_INSTANCE(GameInstance);
	}

	if (_id == 242 && true == _bTrigerOn)
	{

		if (false == _bEventState)
		{
			GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

			pGameInstance->StopAll();
			pGameInstance->PlaySoundLoop(TEXT("Amb_Deep_impacts_stress.wav"), SOUND_SCARE1, 0.8f);
			pGameInstance->PlaySoundLoop(TEXT("bass #30425.wav"), SOUND_SCARE2, 0.3f);

			RELEASE_INSTANCE(GameInstance);

			_bEventState = true;
		}

	}

	if (_id == 243 && true == _bTrigerOn && false == _bEventState)
	{

		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

		uint32 iLevel = pGameInstance->GetCurrentLevelIndex();
		Player* pPlayer = static_cast<Player*>(pGameInstance->GetLayerObjectTag(LAYER_TAG::LAYER_PLAYER, "Player"));
		if (nullptr == pPlayer)
			return;

		_bool obtain = static_cast<PlayerBody*>(pPlayer->GetPlyaerPart()[Player::PART::PART_BODY])->IsObtainingLight();

		if (true == obtain)
		{
			pGameInstance->StopAll();
			pGameInstance->PlaySoundLoop(TEXT("bass.wav"), SOUND_SCARE1, 0.2f);
			_bEventState = true;
		}

		RELEASE_INSTANCE(GameInstance);
	}

	if (_id == 244 && true == _bTrigerOn && false == _bEventState)
	{
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

		uint32 iLevel = pGameInstance->GetCurrentLevelIndex();
		Player* pPlayer = static_cast<Player*>(pGameInstance->GetLayerObjectTag(LAYER_TAG::LAYER_PLAYER, "Player"));
		if (nullptr == pPlayer)
			return;

		_bool obtain = static_cast<PlayerBody*>(pPlayer->GetPlyaerPart()[Player::PART::PART_BODY])->IsObtainingLight();

		if (true == obtain)
		{
			pGameInstance->StopSound(SOUND_SCARE2);
			pGameInstance->PlaySound(TEXT("scary pad 2_4.wav"), SOUND_SCARE2, 0.8f);


			_bEventState = true;
		}


		RELEASE_INSTANCE(GameInstance);
	}

	if (_id == 245 && false == _bEventState)
	{
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

		vector<GameObject*>& vecTriger = pGameInstance->GetLayerObject(LAYER_TAG::LAYER_TRIGER);

		auto iter = find_if(vecTriger.begin(), vecTriger.end(), [&](GameObject* pObj) {
			if (pObj->GetIdNumber() == 253)
				return true;

			return false;
			});

		if (nullptr != *iter)
		{
			if (true == static_cast<TrigerBox*>(*iter)->_bLastScene)
			{
				_bEventState = true;
				pGameInstance->StopSound(SOUND_ENVIRONMENT4);
				RELEASE_INSTANCE(GameInstance);

				return;
			}
		}

		_float fVolume = 0.f;
		_float fDistance = 0.f;
		
		LerpSoundPlayer(fVolume, fDistance, 6.f, pGameInstance);

		if (fDistance <= 6.f)
			pGameInstance->PlaySoundLoop(TEXT("dihanie.wav"), SOUND_ENVIRONMENT4, fVolume);



		RELEASE_INSTANCE(GameInstance);
	}


	if (_id == 247 && false == _bEventState && true == _bTrigerOn)
	{
		LandObject::LANDOBJET_DESC LandObjectDesc = {};

		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

		Transform* pTransform = static_cast<Transform*>(pGameInstance->GetComponent(static_cast<uint32>(LEVEL::GAME), LAYER_TAG::LAYER_STATIC, TEXT("ComponentTransform"), "2stBottom", 0));
		BinaryNavi* pNavi = static_cast<BinaryNavi*>(pGameInstance->GetComponent(static_cast<uint32>(LEVEL::GAME), LAYER_TAG::LAYER_STATIC, TEXT("ComponentNavigation"), "2stBottom", 0));

		vector<Cell*>& vecCells = pNavi->GetCell();

		LandObjectDesc.pCells = &vecCells;
		LandObjectDesc.pCellTransform = pTransform;

		GameObject* pPlayer = pGameInstance->GetLayerObjectTag(LAYER_TAG::LAYER_PLAYER, "Player");

		Monster* pGameObject = static_cast<Monster*>(pGameInstance->CloneGameObject(TEXT("ProtoTypeDanceMonster"), &LandObjectDesc));
		pGameObject->GetTransform()->SetState(Transform::STATE::POSITION, Vec4(20.7711, -3.058f, -1.9880, 1.f));
		pGameObject->GetTransform()->FixRotation(Vec4(0.f, 1.f, 0.f, 0.f), ::XMConvertToRadians(90.f));
		pGameObject->GetNavigation()->SetCurrentIndex(465);
		pGameObject->GetStateMachine()->SetState(State::STATE::RUN);
		pGameObject->SetTarget(pPlayer);
		pGameObject->SetOnWater(true);
		pGameObject->GetTransform()->SetSpeedPerSec(1.5f);
		static_cast<MonsterLight*>(pGameObject->GetMonsterPart()[Monster::PART_LIGHT])->GetOwnLight()->GetLightDesc()->bEnable = false;

		pGameInstance->CreateObject(pGameObject, LAYER_TAG::LAYER_MONSTER);
		RELEASE_INSTANCE(GameInstance);

		_bEventState = true;
	}

	if (_id == 248 && false == _bEventState && true == _bTrigerOn)
	{
		_bEventState = true;

		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

		pGameInstance->PlaySoundLoop(TEXT("High Violin Note.wav"), SOUND_SCARE2, 1.f);

		RELEASE_INSTANCE(GameInstance);
	}

	if (_id == 249 && false == _bEventState && true == _bTrigerOn)
	{
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

		_bEventState = true;

		pGameInstance->StopAll();

		RELEASE_INSTANCE(GameInstance);
	}

	if (_id == 250 && false == _bEventState && true == _bTrigerOn)
	{
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);


		vector<GameObject*>& pObjList = pGameInstance->GetLayerObject(LAYER_TAG::LAYER_DYNAMIC);

		auto iter = find_if(pObjList.begin(), pObjList.end(), [&](GameObject* pObj) {
			if (pObj->GetIdNumber() == 187)
				return true;

			return false;
			});

		if (nullptr != *iter)
		{
			_bool Isopen = dynamic_cast<DynamicObject*>(*iter)->GetIsOpen();

			if (true == Isopen)
			{
				pGameInstance->PlaySoundLoop(TEXT("High Violin Note.wav"), SOUND_SCARE2, 1.f);

				_bEventState = true;
			}
		}

		RELEASE_INSTANCE(GameInstance);
	}

	if (_id == 251 && false == _bEventState && true == _bTrigerOn)
	{
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

		// 그로울링 + scary
		pGameInstance->StopSound(SOUND_SCARE1);
		pGameInstance->PlaySound(TEXT("scary pad 2_9.wav"), SOUND_SCARE1, 1.f);
		pGameInstance->PlaySoundLoop(TEXT("Very Low Growling.wav"), SOUND_SCARE3, 1.f);

		_bEventState = true;

		RELEASE_INSTANCE(GameInstance);
	}

	if (_id == 252)
	{
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

		vector<GameObject*>& pObjList = pGameInstance->GetLayerObject(LAYER_TAG::LAYER_TRIGER);

		auto iter = find_if(pObjList.begin(), pObjList.end(), [&](GameObject* pObj) {
			if (pObj->GetIdNumber() == 251)
				return true;

			return false;
			});


		auto iter2 = find_if(pObjList.begin(), pObjList.end(), [&](GameObject* pObj) {
			if (pObj->GetIdNumber() == 253)
				return true;

			return false;
			});

		if (nullptr != *iter && nullptr != *iter2)
		{
			_bool IsTrigetOn = static_cast<TrigerBox*>(*iter)->GetTriger();
			_bool IsTrigerOn = static_cast<TrigerBox*>(*iter2)->GetTriger();

			if (true == IsTrigetOn && false == IsTrigerOn)
			{
				_float fVolume = 0.f;
				_float fDistance = 0.f;

				LerpSoundPlayer(fVolume, fDistance, 15.f, pGameInstance, 0.05f);

				if (fDistance <= 10.2f)
					pGameInstance->PlaySoundLoop(TEXT("MonsterCrying.wav"), SOUND_SCARE4, fVolume);
			}
		}


		RELEASE_INSTANCE(GameInstance);
	}


	if (_id == 253 && false == _bEventState && true == _bTrigerOn)
	{
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

		pGameInstance->StopAll();
		pGameInstance->PlaySoundLoop(TEXT("highlightSound.wav"), SOUND_ENVIRONMENT6, 0.8f);


		LandObject::LANDOBJET_DESC LandObjectDesc = {};

		Transform* pTransform = static_cast<Transform*>(pGameInstance->GetComponent(static_cast<uint32>(LEVEL::GAME), LAYER_TAG::LAYER_STATIC, TEXT("ComponentTransform"), "2stBottom", 0));
		BinaryNavi* pNavi = static_cast<BinaryNavi*>(pGameInstance->GetComponent(static_cast<uint32>(LEVEL::GAME), LAYER_TAG::LAYER_STATIC, TEXT("ComponentNavigation"), "2stBottom", 0));

		vector<Cell*>& vecCells = pNavi->GetCell();

		LandObjectDesc.pCells = &vecCells;
		LandObjectDesc.pCellTransform = pTransform;

		GameObject* pPlayer = pGameInstance->GetLayerObjectTag(LAYER_TAG::LAYER_PLAYER, "Player");

		Monster* pGameObject = static_cast<Monster*>(pGameInstance->CloneGameObject(TEXT("ProtoTypeDanceMonster"), &LandObjectDesc));
		pGameObject->GetTransform()->SetState(Transform::STATE::POSITION, Vec4(20.7711, -3.058f, -1.9880, 1.f));
		pGameObject->GetNavigation()->SetCurrentIndex(465);
		pGameObject->GetStateMachine()->SetState(State::STATE::RUN);
		pGameObject->SetTarget(pPlayer);
		pGameObject->GetTransform()->SetSpeedPerSec(1.f);
		static_cast<MonsterLight*>(pGameObject->GetMonsterPart()[Monster::PART_LIGHT])->GetOwnLight()->GetLightDesc()->bEnable = false;

		pGameInstance->CreateObject(pGameObject, LAYER_TAG::LAYER_MONSTER);


		pGameObject = static_cast<Monster*>(pGameInstance->CloneGameObject(TEXT("ProtoTypeDanceMonster"), &LandObjectDesc));
		pGameObject->GetTransform()->SetState(Transform::STATE::POSITION, Vec4(11.8908, -3.058f, -16.2216, 1.f));
		pGameObject->GetNavigation()->SetCurrentIndex(566);
		pGameObject->GetStateMachine()->SetState(State::STATE::IDLE);
		pGameObject->SetTarget(pPlayer);
		pGameObject->GetTransform()->SetSpeedPerSec(1.5f);

		pGameInstance->CreateObject(pGameObject, LAYER_TAG::LAYER_MONSTER);


		pGameObject = static_cast<Monster*>(pGameInstance->CloneGameObject(TEXT("ProtoTypeDanceMonster"), &LandObjectDesc));
		pGameObject->GetTransform()->SetState(Transform::STATE::POSITION, Vec4(12.2680, -3.058f, -6.9177, 1.f));
		pGameObject->GetNavigation()->SetCurrentIndex(495);
		pGameObject->GetStateMachine()->SetState(State::STATE::IDLE);
		pGameObject->SetTarget(pPlayer);
		pGameObject->GetTransform()->SetSpeedPerSec(1.f);

		pGameInstance->CreateObject(pGameObject, LAYER_TAG::LAYER_MONSTER);

		_bEventState = true;

		RELEASE_INSTANCE(GameInstance);
	}
	else if (_id == 253 && true == _bTrigerOn && _bLastScene == false)
	{
		// 모든 라이트가 감소됐다가 다시 켜져야함. 비네트로 처리
		Renderer::DEFERRED_DESC& deferredOption = _render->GetDeferredOption();


		Vec2 vCurrent = Vec2(deferredOption.fFadeRange, 0.f);
		Vec2 vResult;

		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

		uint32 iMonsterSize = pGameInstance->GetLayerObject(LAYER_TAG::LAYER_MONSTER).size();

		if (iMonsterSize <= 0)
		{
			if (0.05f >= _fMaxBlinkRange.Length() - vCurrent.Length())
				vResult = Vec2::SmoothStep(vCurrent, _fMaxBlinkRange, timeDelta * 6.f);
			else
			{
				deferredOption.fFadeRange = _fMaxBlinkRange.x;
				pGameInstance->StopAll();
				_bLastScene = true;
			}
		}
		else
		{
			if (true == _bToMax)
			{
				if (0.05f <= _fMaxBlinkRange.Length() - vCurrent.Length())
					vResult = Vec2::SmoothStep(vCurrent, _fMaxBlinkRange, timeDelta * 6.f);
				else
				{
					_bToMax = false;
					_bToMin = true;
				}
			}
			else if (true == _bToMin)
			{
				if (0.05f <= vCurrent.Length() - _fMinBlinkRange.Length())
					vResult = Vec2::SmoothStep(vCurrent, _fMinBlinkRange, timeDelta * 6.f);
				else
				{
					_bToMin = false;
					_bToMax = true;
				}
			}

			deferredOption.fFadeRange = vResult.x;
		}
		
		RELEASE_INSTANCE(GameInstance);
	}
	else if (_id == 254 && false == _bEventState)
	{

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

#include "pch.h"
#include "Monster.h"
#include "Surefire.h"
#include "BodyCam.h"
#include "GameInstance.h"
#include "BinaryNavi.h"
#include "BinaryAnimation.h"
#include "BinaryBone.h"
#include "MonsterWalk.h"
#include "MonsterRun.h"
#include "MonsterDance.h"
#include "TrigerBox.h"
#include "Cell.h"
#include "MonsterBody.h"
#include "MonsterLight.h"
#include "MonsterAxe.h"

uint32 Monster::_iMonsterCount = 0;

Monster::Monster(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: LandObject(device, deviceContext, OBJECT_TYPE::MONSTER)
{
	_objectType = OBJECT_TYPE::MONSTER;
	_modelName = "Monster";
	_modelPath = L"Monster";
}

Monster::Monster(const Monster& rhs)
	: LandObject(rhs)
{

}

HRESULT Monster::InitializePrototype()
{
	return S_OK;
}

HRESULT Monster::Initialize(void* pArg)
{

	++_iMonsterCount;
	_iMonsterID = _iMonsterCount;


	__super::Initialize(pArg);

	if (FAILED(ReadyComponents()))
		return E_FAIL;

	if (FAILED(ReadyMonsterPart()))
		return E_FAIL;

	State* pState = MonsterWalk::Create(_device, _deviceContext, this);
	_pStateMachine->AddState(State::STATE::IDLE, pState);
	pState = MonsterRun::Create(_device, _deviceContext, this);
	_pStateMachine->AddState(State::STATE::RUN, pState);
	pState = MonsterDance::Create(_device, _deviceContext, this);
	_pStateMachine->AddState(State::STATE::DANCE, pState);

	vector<BinaryAnimation*>& vecAnim = static_cast<MonsterBody*>(_pMonsterPart[PART_BODY])->GetBinaryModelComponent()->GetBinaryAnimation();
	vector<BinaryBone*>& pVecBone = static_cast<MonsterBody*>(_pMonsterPart[PART_BODY])->GetBinaryModelComponent()->GetBinaryBones();

	_pAnimator->AddAnimation(State::STATE::IDLE, vecAnim[0], &pVecBone, true, 2.5f);
	_pAnimator->AddAnimation(State::STATE::RUN, vecAnim[1], &pVecBone, true, 2.f);
	_pAnimator->AddAnimation(State::STATE::DANCE, vecAnim[2], &pVecBone, true, 2.f);



	_pStateMachine->SetAnimator(_pAnimator);
	_pStateMachine->SetState(State::STATE::IDLE);
	_transform->SetState(Transform::STATE::POSITION, ::XMVectorSet(11.f, 0.f, -42.5f, 1.f));



	return S_OK;
}

void Monster::Tick(const _float& fTimeDelta)
{
	if (_enabled)
		return;


	XMVECTOR vPosition = __super::SetUp_OnCell(_transform->GetState(Transform::STATE::POSITION), _pNavigation->GetCurrentIndex());
	_transform->SetState(Transform::STATE::POSITION, vPosition);
	_pStateMachine->UpdateStateMachine(fTimeDelta);


	if (1 == _iMonsterID)
		_transform->Forward(fTimeDelta, _pNavigation);

	_pCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());
	_pAssistCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());

	for (auto& pPart : _pMonsterPart)
		if (nullptr != pPart)
			pPart->Tick(fTimeDelta);

}

void Monster::LateTick(const _float& fTimeDelta)
{
	_pStateMachine->UpdateLateStateMachine(fTimeDelta);

	for (auto& pPart : _pMonsterPart)
		if (nullptr != pPart)
			pPart->LateTick(fTimeDelta);


	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG
	_render->AddDebug(_pCollider);
	_render->AddDebug(_pAssistCollider);;
	_render->AddDebug(_pNavigation);
#endif // _DEBUG
}

HRESULT Monster::Render()
{
	for (auto& pPart : _pMonsterPart)
	{
		if (nullptr != pPart)
			pPart->Render();
	}

	return S_OK;
}

void Monster::OnCollisionEnter(Collider* pOther)
{
	if (pOther->GetOwner()->GetModelName() == "PlayerBullet")
		--_iLife;
}

void Monster::OnCollisionStay(Collider* pOther)
{
	if (pOther->GetOwner()->GetModelName() == "TrigerBox")
		TrigerBoxEvent(pOther);
}

void Monster::OnCollisionExit(Collider* pOther)
{

}

void Monster::OnAssistCollisionEnter(Collider* pOther)
{
	if (pOther->GetOwner()->GetObjectType() == OBJECT_TYPE::PLAYER && _iMonsterID == 2)
	{
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
		
		if (false == _bRunOnWater && 0 < _iLife)
		{
			pGameInstance->StopSound(SOUND_MONSTER);
			pGameInstance->StopSound(SOUND_MONSTER2);
			pGameInstance->PlaySound(TEXT("Scream.wav"), SOUND_MONSTER2, 1.f);

			LIGHT_DESC* pLightDesc = static_cast<MonsterLight*>(_pMonsterPart[PART_LIGHT])->GetOwnLight()->GetLightDesc();
			pLightDesc->Diffuse = Color(1.f, 0.525f, 0.347f, 1.f);

			_transform->SetSpeedPerSec(2.5f);
		}

		_pTargetObject = pOther->GetOwner();
		_bRunOnWater = true;
		_bOnWater = false;

		RELEASE_INSTANCE(GameInstance);
	}
}

void Monster::OnAssistCollisionStay(Collider* pOther)
{
}

void Monster::OnAssistCollisionExit(Collider* pOther)
{
}

void Monster::TrigerBoxEvent(Collider* pOther)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	uint32 id = pOther->GetOwner()->GetIdNumber();

	if (id == 236)
	{
		static_cast<TrigerBox*>(pOther->GetOwner())->TrigerSet(true);
		_bDeadDelay = true;
	}

	RELEASE_INSTANCE(GameInstance);
}

void Monster::MoveAstar(const _float& fTimeDelta)
{
	if (_bestList.size() > 1)
	{
		TargetStare(_bestList.front()->GetPosition(), fTimeDelta);
		if (_bestList.front()->IsIn(_transform->GetState(Transform::STATE::POSITION), _transform->GetWorldMatrixCaculator()))
			_bestList.pop_front();
	}
	else
	{
		Vec3 vPos = _transform->GetState(Transform::STATE::POSITION);
		TargetStare(_vDestination, fTimeDelta);
	}
}

void Monster::TargetStare(XMVECTOR vGoal, const _float& fTimeDelta)
{
	Vec3 vDir = vGoal - _transform->GetState(Transform::STATE::POSITION);

	Vec3 vLook = _transform->GetState(Transform::STATE::LOOK);
	vDir.Normalize();
	vDir.y = 0.f;

	_float fRotMax = 10.f;
	_float fRotNormal = 5.f;

	Vec4 RotDir;

	if (3.f < acosf(vDir.Dot(vLook)))
		RotDir = Vec3::Lerp(vLook, vDir, fRotMax * fTimeDelta);
	else
		RotDir = Vec3::Lerp(vLook, vDir, fRotNormal * fTimeDelta);

	_transform->SetLook(RotDir);
}

HRESULT Monster::ReadyComponents()
{

	GameInstance* gameInstance = GET_INSTANCE(GameInstance);
	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == gameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);

	/* Renderer Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentRenderer"),
		TEXT("ComponentRenderer"), reinterpret_cast<Component**>(&_render))))
		return E_FAIL;

	/* Transform Component */
	Transform::TRANSFORM_DESC transformDesc;
	transformDesc.speedPerSec = 0.5f;
	transformDesc.rotationRadianPerSec = ::XMConvertToRadians(90.f);

		if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform), &transformDesc)))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeStateMachine"),
		TEXT("ComponentStateMachine"), reinterpret_cast<Component**>(&_pStateMachine))))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeAnimator"),
		TEXT("ComponentAnimator"), reinterpret_cast<Component**>(&_pAnimator))))
		return E_FAIL;

	Navigation::NAVIGATION_DESC NavigationDesc;
	NavigationDesc._iCurrentIndex = 3;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeNavigation"),
		TEXT("ComponentNavigation"), reinterpret_cast<Component**>(&_pNavigation), &NavigationDesc)))
		return E_FAIL;

	BoundingAABB::BOUNDING_AABB_DESC aabbDesc;
	{
		aabbDesc.pOwner = this;
		aabbDesc.vExtents = Vec3(0.3f, 0.8, 0.5f);
		aabbDesc.vCenter = Vec3(0.f, aabbDesc.vExtents.y, 0.f);
	}

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeAABBCollider"),
		TEXT("ComponentCollider"), reinterpret_cast<Component**>(&_pCollider), &aabbDesc)))
		return E_FAIL;

	Bounding_Frustum::BOUNDING_FRUSTUM_DESC frustumDesc;
	{
		frustumDesc.pOwner = this;
		frustumDesc.vCenter = Vec3(0.f, 1.f, 0.f);
		frustumDesc.vOrientation = Vec4(0.f, 0.f, 0.f, 1.f);
		frustumDesc.fLeftSlope = -0.1f;
		frustumDesc.fRightSlope = 0.1f;
		frustumDesc.fTopSlope = 0.1f;
		frustumDesc.fBottomSlope = -0.1f;
		frustumDesc.fNear = 0.1f;
		frustumDesc.fFar = 5.f;
	}

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeAssistFrustumCollider"),
		TEXT("ComponentAssistCollider"), reinterpret_cast<Component**>(&_pAssistCollider), &frustumDesc)))
		return E_FAIL;

	if (_iMonsterID == 1 || _iMonsterID == 2 || _iMonsterCount == 3 || _iMonsterCount == 5)
	{
		if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeMonsterDiffuse2"),
			TEXT("ComponentDiffuseTexture"), reinterpret_cast<Component**>(&_pTexture[MONSTER_DIFFUSE]))))
			return E_FAIL;

		if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeMonsterNormal2"),
			TEXT("ComponentNormalTexture"), reinterpret_cast<Component**>(&_pTexture[MONSTER_NORMAL]))))
			return E_FAIL;
	}



	RELEASE_INSTANCE(GameInstance);


	return S_OK;
}

HRESULT Monster::BindShaderResuorces()
{
	GameInstance* gameInstance = GameInstance::GetInstance();
	Safe_AddRef<GameInstance*>(gameInstance);


	if (FAILED(_transform->BindShaderResources(_shader, "W")))
		return E_FAIL;

	if (FAILED(gameInstance->BindTransformToShader(_shader, "V", CameraHelper::TRANSFORMSTATE::D3DTS_VIEW)))
		return E_FAIL;
	if (FAILED(gameInstance->BindTransformToShader(_shader, "P", CameraHelper::TRANSFORMSTATE::D3DTS_PROJ)))
		return E_FAIL;

	Safe_Release<GameInstance*>(gameInstance);

	return S_OK;
}

HRESULT Monster::ReadyMonsterPart()
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	GameObject* pMonsterPart = nullptr;

	// Monster Body
	MonsterPartObject::PART_DESC PartDesc;
	PartDesc.pParentTransform = _transform;
	PartDesc.pDissolveTime = &_fLifeTime;
	PartDesc.pTexture[MONSTER_DIFFUSE] = _pTexture[MONSTER_DIFFUSE];
	PartDesc.pTexture[MONSTER_NORMAL] = _pTexture[MONSTER_NORMAL];

	pMonsterPart = gameInstance->CloneGameObject(TEXT("ProtoTypeDanceMonsterBody"), &PartDesc);
	if (nullptr == pMonsterPart)
		return E_FAIL;
	_pMonsterPart.push_back(pMonsterPart);


	// TODO MonsterID가 2번째 인 친구한테만 달자.

	if (2 == _iMonsterID || 3 == _iMonsterID)
	{
		MonsterLight::MONSTER_FLASH MonsterLightDesc;
		MonsterLightDesc.pParentTransform = _transform;
		MonsterLightDesc.pSocketMatrix = static_cast<MonsterBody*>(_pMonsterPart[PART_BODY])->GetSocketBoneMatrix("mixamorig5:Spine1");
		MonsterLightDesc.mSocketPivot = static_cast<MonsterBody*>(_pMonsterPart[PART_BODY])->GetSocketPivotMatrix();
		MonsterLightDesc.pDissolveTime = &_fLifeTime;

		pMonsterPart = gameInstance->CloneGameObject(TEXT("ProtoTypeDanceMonsterLight"), &MonsterLightDesc);
		if (nullptr == pMonsterPart)
			return E_FAIL;
		_pMonsterPart.push_back(pMonsterPart);
	}


	MonsterAxe::MONSTER_AXE AxeDesc;
	AxeDesc.pDissolveTime = &_fLifeTime;
	AxeDesc.pParentTransform = _transform;
	AxeDesc.pSocketMatrix = static_cast<MonsterBody*>(_pMonsterPart[PART_BODY])->GetSocketBoneMatrix("mixamorig5:RightHand");
	AxeDesc.mSocketPivot = static_cast<MonsterBody*>(_pMonsterPart[PART_BODY])->GetSocketPivotMatrix();

	pMonsterPart = gameInstance->CloneGameObject(TEXT("ProtoTypeAxe"), &AxeDesc);
	if (nullptr == pMonsterPart)
		return E_FAIL;
	_pMonsterPart.push_back(pMonsterPart);


	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}


Monster* Monster::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Monster* pMonster = new Monster(device, deviceContext);

	if (FAILED(pMonster->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : Monster");
		Safe_Release<Monster*>(pMonster);
	}

	return pMonster;
}

GameObject* Monster::Clone(void* argument)
{
	Monster* pMonster = new Monster(*this);

	if (FAILED(pMonster->Initialize(argument)))
	{
		MSG_BOX("Failed to Cloned : Monster");
		Safe_Release<Monster*>(pMonster);
	}

	return pMonster;
}

void Monster::Free()
{
	__super::Free();

	for (auto& pPart : _pMonsterPart)
		Safe_Release<GameObject*>(pPart);

	_pMonsterPart.clear();

	Safe_Release<Shader*>(_shader);
	Safe_Release<Renderer*>(_render);
	Safe_Release<StateMachine*>(_pStateMachine);
	Safe_Release<Animator*>(_pAnimator);
}

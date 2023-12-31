#include "pch.h"
#include "Player.h"
#include "PlayerBody.h"
#include "Surefire.h"
#include "BodyCam.h"
#include "GameInstance.h"
#include "BinaryNavi.h"
#include "PlayerWalk.h"
#include "PlayerFire.h"
#include "PlayerReload.h"
#include "BinaryAnimation.h"
#include "BinaryBone.h"
#include "DynamicObject.h"

Player::Player(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: LandObject(device, deviceContext, OBJECT_TYPE::PLAYER)
{
	_objectType = OBJECT_TYPE::PLAYER;
	_modelName = "Player";
	_modelPath = L"Player";
}

Player::Player(const Player& rhs)
	: LandObject(rhs)
{
	
}

HRESULT Player::InitializePrototype()
{
	return S_OK;
}

HRESULT Player::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	if (FAILED(ReadyComponents()))
		return E_FAIL;

	if (FAILED(ReadyPlayerPart()))
		return E_FAIL;

	State* pState = PlayerWalk::Create(_device, _deviceContext, this);
	_pStateMachine->AddState(State::STATE::IDLE, pState);
	pState = PlayerFire::Create(_device, _deviceContext, this);
	_pStateMachine->AddState(State::STATE::SHOOT, pState);
	pState = PlayerReload::Create(_device, _deviceContext, this);
	_pStateMachine->AddState(State::STATE::RELOAD, pState);
	
	vector<BinaryAnimation*>& vecAnim = 
		static_cast<PlayerBody*>(m_pPlayerPart[PART_BODY])->GetBinaryModelComponent()->GetBinaryAnimation();
	vector<BinaryBone*>& pVecBone = static_cast<PlayerBody*>(m_pPlayerPart[PART_BODY])->GetBinaryModelComponent()->GetBinaryBones();

	_pAnimator->AddAnimation(State::STATE::SHOOT, vecAnim[0], &pVecBone);
	_pAnimator->AddAnimation(State::STATE::RELOAD, vecAnim[1], &pVecBone);
	_pAnimator->AddAnimation(State::STATE::IDLE, vecAnim[2], &pVecBone, true);


	_pStateMachine->SetAnimator(_pAnimator);
	_pStateMachine->SetState(State::STATE::IDLE);
	_transform->SetState(Transform::STATE::POSITION, ::XMVectorSet(11.f, 0.f, -45.5f, 1.f));


	return S_OK;
}

void Player::Tick(const _float& fTimeDelta)
{
	if (_enabled)
		return;

	KeyInput(fTimeDelta);
	_pStateMachine->UpdateStateMachine(fTimeDelta);

	_pCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());

	for (auto& pPart : m_pPlayerPart)
	{
		if (nullptr != pPart)
			pPart->Tick(fTimeDelta);
	}

}

void Player::LateTick(const _float& fTimeDelta)
{
	_pStateMachine->UpdateLateStateMachine(fTimeDelta);

	
	for (auto& pPart : m_pPlayerPart)
	{
		if (nullptr != pPart)
			pPart->LateTick(fTimeDelta);
	}

	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

HRESULT Player::Render()
{
	if (_enabled)
		return S_OK;


	if (FAILED(BindShaderResuorces()))
		return E_FAIL;


	for (auto& pPart : m_pPlayerPart)
	{
		if (nullptr != pPart)
			pPart->Render();
	}

#ifdef _DEBUG
	_pCollider->Render();
#endif // _DEBUG

#ifdef _DEBUG
	_pNavigation->Render();
#endif // _DEBUG


	return S_OK;
}

void Player::OnCollisionEnter(Collider* pOther)
{
	

}

void Player::OnCollisionStay(Collider* pOther)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	if (pOther->GetOwner()->GetObjectType() == OBJECT_TYPE::DYNAMIC)
	{
		if (pOther->GetOwner()->GetModelName() == "OldSteelGridMainWithKey")
		{
			if (pGameInstance->keyDown(DIK_E))
			{
				if(pOther->GetOwner()->GetIdNumber() != 163 && pOther->GetOwner()->GetIdNumber() != 170)
					dynamic_cast<DynamicObject*>(pOther->GetOwner())->SetRotate(true);
				else
				{
					if(true == _bObtatinKey)
						dynamic_cast<DynamicObject*>(pOther->GetOwner())->SetRotate(true);
				}
			}
		}
		else if (pOther->GetOwner()->GetModelName() == "BasementWoodDoorMain")
		{
			if (pGameInstance->keyDown(DIK_E))
				dynamic_cast<DynamicObject*>(pOther->GetOwner())->SetRotate(true);
		}
		else if (pOther->GetOwner()->GetModelName() == "Gun")
		{
			if (pGameInstance->keyDown(DIK_E))
			{
				dynamic_cast<PlayerBody*>(m_pPlayerPart[PART::PART_BODY])->SetObtainGun(true);
				pOther->GetOwner()->SetDead(true);
			}
		}
		else if (pOther->GetOwner()->GetModelName() == "Surefire")
		{
			if (pGameInstance->keyDown(DIK_E))
			{
				dynamic_cast<Surefire*>(m_pPlayerPart[PART::PART_SURFIRE])->SetObtainLight(true);
				pOther->GetOwner()->SetDead(true);
			}
		}
		else if (pOther->GetOwner()->GetModelName() == "Key")
		{
			if (pGameInstance->keyDown(DIK_E))
			{
				pOther->GetOwner()->SetDead(true);
				SetObtainKey(true);
			}
		}
	}

	RELEASE_INSTANCE(GameInstance);
}

void Player::OnCollisionExit(Collider* pOther)
{

}

void Player::KeyInput(const _float& timeDelta)
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	POINT		pt{ g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

	ClientToScreen(g_hWnd, &pt);
	SetCursorPos(pt.x, pt.y);

	_long mouseMove = 0l;

	if (mouseMove = gameInstance->Get_DIMouseMove(DIMM::DIMM_X))
		_transform->Turn(::XMVectorSet(0.f, 1.f, 0.f, 0.f), mouseMove * 0.1f * timeDelta);

	if (mouseMove = gameInstance->Get_DIMouseMove(DIMM::DIMM_Y))
		_transform->Turn(_transform->GetState(Transform::STATE::RIGHT), mouseMove * 0.1f * timeDelta);

	XMVECTOR vPosition = __super::SetUp_OnCell(_transform->GetState(Transform::STATE::POSITION), _pNavigation->GetCurrentIndex());

	_transform->SetState(Transform::STATE::POSITION, vPosition);

	RELEASE_INSTANCE(GameInstance);
}

HRESULT Player::ReadyComponents()
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
	transformDesc.speedPerSec = 2.f;
	transformDesc.rotationRadianPerSec = ::XMConvertToRadians(90.f);

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform), &transformDesc)))
		return E_FAIL;

	Navigation::NAVIGATION_DESC NavigationDesc;
	NavigationDesc._iCurrentIndex = 3;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeNavigation"),
		TEXT("ComponentNavigation"), reinterpret_cast<Component**>(&_pNavigation), &NavigationDesc)))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeStateMachine"),
		TEXT("ComponentStateMachine"), reinterpret_cast<Component**>(&_pStateMachine))))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeAnimator"),
		TEXT("ComponentAnimator"), reinterpret_cast<Component**>(&_pAnimator))))
		return E_FAIL;


	BoundingAABB::BOUNDING_AABB_DESC aabbDesc;
	{
		aabbDesc.pOwner = this;
		aabbDesc.vCenter = Vec3(0.f, 0.f, 0.f);
		aabbDesc.vExtents = Vec3(0.3f, 0.5f, 0.5f);
	}

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeAABBCollider"),
		TEXT("ComponentCollider"), reinterpret_cast<Component**>(&_pCollider), &aabbDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT Player::BindShaderResuorces()
{


	return S_OK;

}

HRESULT Player::ReadyPlayerPart()
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	GameObject* pPlayerPart = nullptr;

	// Player Body
	PartObject::PART_DESC PartDesc;
	PartDesc.pParentTransform = _transform;

	pPlayerPart = gameInstance->CloneGameObject(TEXT("ProtoTypeGameObjectPlayerBody"), &PartDesc);
	if (nullptr == pPlayerPart)
		return E_FAIL;
	m_pPlayerPart.push_back(pPlayerPart);


	// SureFire
	Surefire::FLASHLIGHT_DESC surefireDesc;
	surefireDesc.pParentTransform = _transform;
	surefireDesc.SocketMatrix = static_cast<PlayerBody*>(m_pPlayerPart[PART_BODY])->Get_SocketBonePtr("gun_root");
	surefireDesc.SocketPivot = static_cast<PlayerBody*>(m_pPlayerPart[PART_BODY])->Get_SocketPivotMatrix();


	pPlayerPart = gameInstance->CloneGameObject(TEXT("ProtoTypeGameObjectPlayerSurefire"), &surefireDesc);
	if (nullptr == pPlayerPart)
		return E_FAIL;
	m_pPlayerPart.push_back(pPlayerPart);


	BodyCam::BODYCAM_DESC BodyCamDesc;
	BodyCamDesc.pParentTransform = _transform;
	BodyCamDesc.SocketMatrix = static_cast<PlayerBody*>(m_pPlayerPart[PART_BODY])->Get_SocketBonePtr("Head_Cam");
	BodyCamDesc.SocketPivot = static_cast<PlayerBody*>(m_pPlayerPart[PART_BODY])->Get_SocketPivotMatrix();
	BodyCamDesc.vEye = Vec4(0.f, 10.f, -8.f, 1.f);
	BodyCamDesc.vAt = Vec4(0.f, 0.f, 0.f, 1.f);
	BodyCamDesc.fFov = ::XMConvertToRadians(70.f);
	BodyCamDesc.fAspect = g_iWinSizeX / static_cast<_float>(g_iWinSizeY);
	BodyCamDesc.fNear = 0.001f;
	BodyCamDesc.fFar = 50.f;


	pPlayerPart = gameInstance->CloneGameObject(TEXT("ProtoTypeGameObjectBodyCam"), &BodyCamDesc);
	if (nullptr == pPlayerPart)
		return E_FAIL;
	m_pPlayerPart.push_back(pPlayerPart);


	RELEASE_INSTANCE(GameInstance);


	return S_OK;
}

Player* Player::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Player* player = new Player(device, deviceContext);

	if (FAILED(player->InitializePrototype()))
	{
		MSG_BOX("Create Player Failed : Prototype ");
		Safe_Release<Player*>(player);
		return nullptr;
	}

	return player;
}

GameObject* Player::Clone(void* argument)
{
	Player* player = new Player(*this);

	if (FAILED(player->Initialize(argument)))
	{
		MSG_BOX("Create Player Failed : Cloned ");
		Safe_Release<Player*>(player);
		return nullptr;
	}

	return player;
}

void Player::Free()
{
	__super::Free();

	for (auto& pPart : m_pPlayerPart)
		Safe_Release<GameObject*>(pPart);

	m_pPlayerPart.clear();

	Safe_Release<Shader*>(_shader);
	Safe_Release<Renderer*>(_render);
	Safe_Release<StateMachine*>(_pStateMachine);
	Safe_Release<Animator*>(_pAnimator);
}

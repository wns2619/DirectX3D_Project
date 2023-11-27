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
#include "DynamicObjectGroup.h"
#include "TrigerBox.h"

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

	_pAnimator->AddAnimation(State::STATE::RELOAD, vecAnim[0], &pVecBone, false, 1.85f);
	_pAnimator->AddAnimation(State::STATE::IDLE, vecAnim[1], &pVecBone, true);
	_pAnimator->AddAnimation(State::STATE::SHOOT, vecAnim[2], &pVecBone, false, 2.f);


	_pStateMachine->SetAnimator(_pAnimator);
	_pStateMachine->SetState(State::STATE::IDLE);
	_transform->SetState(Transform::STATE::POSITION, ::XMVectorSet(11.f, 0.f, -45.5f, 1.f));

	_bHeightCorrection = true;


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

#ifdef _DEBUG
	_render->AddDebug(_pCollider);
	_render->AddDebug(_pNavigation);
#endif // _DEBUG

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
				if(pOther->GetOwner()->GetIdNumber() != 189 && pOther->GetOwner()->GetIdNumber() != 196)
					static_cast<DynamicObject*>(pOther->GetOwner())->SetRotate(true);
				else
				{
					if(true == _bObtatinKey)
						static_cast<DynamicObject*>(pOther->GetOwner())->SetRotate(true);
				}
			}
		}
		else if (pOther->GetOwner()->GetModelName() == "BasementWoodDoorMain")
		{
			if (pGameInstance->keyDown(DIK_E))
				static_cast<DynamicObject*>(pOther->GetOwner())->SetRotate(true);
		}
		else if (pOther->GetOwner()->GetModelName() == "Gun")
		{
			if (pGameInstance->keyDown(DIK_E))
			{
				static_cast<PlayerBody*>(m_pPlayerPart[PART::PART_BODY])->SetObtainGun(true);

				pGameInstance->StopSound(SOUND_PICKUP);
				pGameInstance->PlaySound(TEXT("PickUp.wav"), SOUND_PICKUP, 1.f);

				pGameInstance->DeleteObject(pOther->GetOwner());
			}
		}
		else if (pOther->GetOwner()->GetModelName() == "surefire")
		{
			if (pGameInstance->keyDown(DIK_E))
			{
				static_cast<PlayerBody*>(m_pPlayerPart[PART::PART_BODY])->SetObtainLight(true);

				uint32 pLightNum[1] = { 15 };
				pGameInstance->SelectTurnOffLight(pLightNum, 1);
				OtherLight* pLight = pGameInstance->FindLightFromID(24);
				pLight->GetLightDesc()->bEnable = false;

				pGameInstance->StopAll();
				pGameInstance->PlaySound(TEXT("PickUp2.wav"), SOUND_PICKUP, 1.f);

				pGameInstance->DeleteObject(pOther->GetOwner());
			}
		}
		else if (pOther->GetOwner()->GetModelName() == "Key")
		{
			if (pGameInstance->keyDown(DIK_E))
			{
				SetObtainKey(true);

				pGameInstance->StopAll();
				pGameInstance->PlaySound(TEXT("PickUp2.wav"), SOUND_PICKUP, 1.f);

				pGameInstance->DeleteObject(pOther->GetOwner());
			}
		}
		else if (pOther->GetOwner()->GetModelName() == "ValveHandler")
		{
			if (pGameInstance->keyDown(DIK_E))
			{
				if (false == static_cast<Valve*>(pOther->GetOwner())->GetIsOpen() &&
					false == static_cast<Valve*>(pOther->GetOwner())->GetIsRotatate())
				{
					static_cast<Valve*>(pOther->GetOwner())->SetRotate(true);
					pGameInstance->StopSound(SOUND_ENVIRONMENT5);
					pGameInstance->PlaySound(TEXT("valve (mp3cut.net).wav"), SOUND_ENVIRONMENT5, 0.2f);
				}
			}
		}
	}

	if (pOther->GetOwner()->GetObjectType() == OBJECT_TYPE::STATIC)
	{
		if(pOther->GetOwner()->GetModelName() == "TrigerBox")
			TrigerBoxEvent(pOther);
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

	if (gameInstance->Get_DIMouseState(DIMK_RBUTTON))
	{
		_float fFov = static_cast<BodyCam*>(m_pPlayerPart[PART::PART_CAMERA])->PlayerCameraFov();
		
		if (::XMConvertToRadians(42.5f) <= fFov)
		{
			static_cast<BodyCam*>(m_pPlayerPart[PART::PART_CAMERA])->AddPlayerCameraFov(::XMConvertToRadians(-1.f));

			Vec4 vPos = static_cast<PlayerBody*>(m_pPlayerPart[PART::PART_BODY])->GetTransform()->GetState(Transform::STATE::POSITION);
			vPos.x += -0.006f;
			vPos.y += 0.002f;

			static_cast<PlayerBody*>(m_pPlayerPart[PART::PART_BODY])->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
			static_cast<PlayerBody*>(m_pPlayerPart[PART::PART_BODY])->GetTransform()->Turn(Vec4(0.f, 1.f, 0.f, 1.f), timeDelta);


			Vec4 vCameraPos = static_cast<BodyCam*>(m_pPlayerPart[PART::PART_CAMERA])->GetTransform()->GetState(Transform::STATE::POSITION);
			vCameraPos.x -= 0.0005f;

			static_cast<BodyCam*>(m_pPlayerPart[PART::PART_CAMERA])->GetTransform()->SetState(Transform::STATE::POSITION, vCameraPos);
			
		}
		else
			_bIsJoom = true;

	}
	else
	{
		_float fFov = static_cast<BodyCam*>(m_pPlayerPart[PART::PART_CAMERA])->PlayerCameraFov();

		if (::XMConvertToRadians(70.f) > fFov)
		{
			static_cast<BodyCam*>(m_pPlayerPart[PART::PART_CAMERA])->AddPlayerCameraFov(::XMConvertToRadians(1.f));

			Vec4 vPos = static_cast<PlayerBody*>(m_pPlayerPart[PART::PART_BODY])->GetTransform()->GetState(Transform::STATE::POSITION);

			vPos.x -= -0.006f;
			vPos.y -= 0.002f;

			static_cast<PlayerBody*>(m_pPlayerPart[PART::PART_BODY])->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
			static_cast<PlayerBody*>(m_pPlayerPart[PART::PART_BODY])->GetTransform()->Turn(Vec4(0.f, -1.f, 0.f, 1.f), timeDelta);


			Vec4 vCameraPos = static_cast<BodyCam*>(m_pPlayerPart[PART::PART_CAMERA])->GetTransform()->GetState(Transform::STATE::POSITION);
			vCameraPos.x += 0.0005f;

			static_cast<BodyCam*>(m_pPlayerPart[PART::PART_CAMERA])->GetTransform()->SetState(Transform::STATE::POSITION, vCameraPos);

		}
		else
			_bIsJoom = false;
	}


	if (true == _bScare)
	{
		// TODO : Ä«¸Þ¶ó ÁÜÀÎ ÁÜ ¾Æ¿ô. ¸î ÃÊ°£ ½ÃÅ°°í ´Ù½Ã ¿ø»ó º¹±Í. 
		_float fFov;

		if (true == _bOriginalFovStore)
		{
			_fOriginalFov = static_cast<BodyCam*>(m_pPlayerPart[PART::PART_CAMERA])->PlayerCameraFov();
			fFov = _fOriginalFov;
			_bOriginalFovStore = false;
		}
		else
			fFov = static_cast<BodyCam*>(m_pPlayerPart[PART::PART_CAMERA])->PlayerCameraFov();

		if (_iPeriod == 2)
			_bScare = false;


		if (_iPeriod < 2)
		{
			if (0 < _iPulseTick)
				static_cast<BodyCam*>(m_pPlayerPart[PART::PART_CAMERA])->AddPlayerCameraFov(::XMConvertToRadians(-2.f));
			else
				static_cast<BodyCam*>(m_pPlayerPart[PART::PART_CAMERA])->AddPlayerCameraFov(::XMConvertToRadians(2.f));
		}
		--_iPulseTick;

		if (-10 == _iPulseTick && _iPeriod < 2)
		{
			_iPulseTick = 11;
			++_iPeriod;
		}
	}


	XMVECTOR vPosition = __super::SetUp_OnCell(_transform->GetState(Transform::STATE::POSITION), _pNavigation->GetCurrentIndex());

	_transform->SetState(Transform::STATE::POSITION, vPosition);

	RELEASE_INSTANCE(GameInstance);
}

void Player::TrigerBoxEvent(Collider* pOther)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	uint32 id = pOther->GetOwner()->GetIdNumber();

	if (id != 234 && id != 238 && id != 243 && id != 244 && id != 246 && id != 236 && id != 249 && id != 250
		&& id != 251 && id != 252 && id != 254 && id != 255)
		static_cast<TrigerBox*>(pOther->GetOwner())->TrigerSet(true);
	else if (id == 234)
	{
		// TODO : Monster appeared
		const _bool& Isobtain = static_cast<PlayerBody*>(m_pPlayerPart[PART::PART_BODY])->IsObtainingLight();

		if (true == Isobtain)
			static_cast<TrigerBox*>(pOther->GetOwner())->TrigerSet(true);
	}
	else if (id == 238)
	{
		if (pGameInstance->keyDown(DIK_E))
		{
			static_cast<TrigerBox*>(pOther->GetOwner())->TrigerSet(true);
			{
				pGameInstance->PlaySound(TEXT("sveton.wav"), SOUND_SCARE1, 1.f);
				OtherLight* pLight = pGameInstance->FindLightFromID(4);

				pLight->GetLightDesc()->Diffuse = Color(1.f, 0.f, 0.f, 1.f);
				pGameInstance->AllTurnOnLight();
			}
		}
	}
	else if (id == 243)
	{
		const _bool& Isobtain = static_cast<PlayerBody*>(m_pPlayerPart[PART::PART_BODY])->IsObtainingLight();

		if (true == Isobtain)
			static_cast<TrigerBox*>(pOther->GetOwner())->TrigerSet(true);
	}
	else if (id == 244)
	{
		const _bool& Isobtain = static_cast<PlayerBody*>(m_pPlayerPart[PART::PART_BODY])->IsObtainingLight();

		if (true == Isobtain)
			static_cast<TrigerBox*>(pOther->GetOwner())->TrigerSet(true);
	}
	else if (id == 246)
	{
		if (false == _OnWater)
		{
			pGameInstance->StopAll();
			pGameInstance->PlayBGM(TEXT("gool.wav"), 0.1f);
		}
		_OnWater = true;
	}
	else if (id == 249)
	{
		vector<GameObject*>& pObjList = pGameInstance->GetLayerObject(LAYER_TAG::LAYER_DYNAMIC);

		auto iter = find_if(pObjList.begin(), pObjList.end(), [&](GameObject* pObj) {
			if (pObj->GetIdNumber() == 187)
				return true;

			return false;
			});

		if (nullptr != *iter)
		{
			_bool Isopen = static_cast<DynamicObject*>(*iter)->GetIsOpen();

			if (true == Isopen)
				static_cast<TrigerBox*>(pOther->GetOwner())->TrigerSet(true);
		}
	}
	else if (id == 250)
	{
		vector<GameObject*>& pObjList = pGameInstance->GetLayerObject(LAYER_TAG::LAYER_DYNAMIC);

		auto iter = find_if(pObjList.begin(), pObjList.end(), [&](GameObject* pObj) {
			if (pObj->GetIdNumber() == 187)
				return true;

			return false;
			});

		if (nullptr != *iter)
		{
			_bool Isopen = static_cast<DynamicObject*>(*iter)->GetIsOpen();

			if (true == Isopen)
				static_cast<TrigerBox*>(pOther->GetOwner())->TrigerSet(true);
		}
	}
	else if (id == 251)
	{
		vector<GameObject*>& pObjList = pGameInstance->GetLayerObject(LAYER_TAG::LAYER_DYNAMIC);

		auto iter = find_if(pObjList.begin(), pObjList.end(), [&](GameObject* pObj) {
			if (pObj->GetIdNumber() == 187)
				return true;

			return false;
			});

		if (nullptr != *iter)
		{
			_bool Isopen = static_cast<DynamicObject*>(*iter)->GetIsOpen();

			if (true == Isopen)
				static_cast<TrigerBox*>(pOther->GetOwner())->TrigerSet(true);
		}
	}
	else if (id == 252)
	{

	}
	else if (id == 253)
	{

	}
	else if (id == 254)
	{

	}
	else if (id == 255)
	{

	}

	RELEASE_INSTANCE(GameInstance);
}

void Player::ScareCamera()
{
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
	transformDesc.speedPerSec = 1.5f;
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
	PartDesc.pLifeTime = &_fLifeTime;

	pPlayerPart = gameInstance->CloneGameObject(TEXT("ProtoTypeGameObjectPlayerBody"), &PartDesc);
	if (nullptr == pPlayerPart)
		return E_FAIL;
	m_pPlayerPart.push_back(pPlayerPart);





	BodyCam::BODYCAM_DESC BodyCamDesc;
	BodyCamDesc.pParentTransform = _transform;
	BodyCamDesc.pLifeTime = &_fLifeTime;
	BodyCamDesc.SocketMatrix = static_cast<PlayerBody*>(m_pPlayerPart[PART_BODY])->Get_SocketBonePtr("Head_Cam");
	BodyCamDesc.SocketPivot = static_cast<PlayerBody*>(m_pPlayerPart[PART_BODY])->Get_SocketPivotMatrix();
	BodyCamDesc.vEye = Vec4(0.f, 10.f, 8.f, 1.f);
	BodyCamDesc.vAt = Vec4(0.f, 0.f, 0.f, 1.f);
	BodyCamDesc.fFov = ::XMConvertToRadians(70.f);
	BodyCamDesc.fAspect = g_iWinSizeX / static_cast<_float>(g_iWinSizeY);
	BodyCamDesc.fNear = 0.001f;
	BodyCamDesc.fFar = 50.f;


	pPlayerPart = gameInstance->CloneGameObject(TEXT("ProtoTypeGameObjectBodyCam"), &BodyCamDesc);
	if (nullptr == pPlayerPart)
		return E_FAIL;
	m_pPlayerPart.push_back(pPlayerPart);


		// SureFire
	Surefire::FLASHLIGHT_DESC surefireDesc;
	surefireDesc.pParentTransform = _transform;
	surefireDesc.pLifeTime = &_fLifeTime;
	surefireDesc.SocketMatrix = static_cast<PlayerBody*>(m_pPlayerPart[PART_BODY])->Get_SocketBonePtr("gun_root");
	surefireDesc.SocketPivot = static_cast<PlayerBody*>(m_pPlayerPart[PART_BODY])->Get_SocketPivotMatrix();


	pPlayerPart = gameInstance->CloneGameObject(TEXT("ProtoTypeGameObjectPlayerSurefire"), &surefireDesc);
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

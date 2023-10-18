#include "pch.h"
#include "Player.h"
#include "PlayerBody.h"
#include "Surefire.h"
#include "GameInstance.h"

Player::Player(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: GameObject(device, deviceContext, OBJECT_TYPE::PLAYER)
{
	_objectType = OBJECT_TYPE::PLAYER;
	_modelName = "Player";
	_modelPath = L"Player";
}

Player::Player(const Player& rhs)
	: GameObject(rhs)
{
	
}

HRESULT Player::InitializePrototype()
{
	return S_OK;
}

HRESULT Player::Initialize(void* pArg)
{
	if (FAILED(ReadyComponents()))
		return E_FAIL;

	if (FAILED(ReadyPlayerPart()))
		return E_FAIL;

	animationcount = 2;
	static_cast<PlayerBody*>(m_pPlayerPart[PART_BODY])->StartAnimation(2, true);

	return S_OK;
}

void Player::Tick(const _float& fTimeDelta)
{
	if (_enabled)
		return;

	KeyInput(fTimeDelta);
}

void Player::LateTick(const _float& fTimeDelta)
{

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
	_pNavigation->Render();
#endif // _DEBUG



	return S_OK;
}

void Player::KeyInput(const _float& timeDelta)
{
	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	if (gameInstance->Get_DIKeyState(DIK_Z) & 0x80)
		_transform->Forward(timeDelta);

	if (gameInstance->Get_DIKeyState(DIK_Q) & 0x80)
		_transform->Turn(Vec4(0.f, 1.f, 0.f, 0.f), timeDelta * -1.f);

	if (gameInstance->Get_DIKeyState(DIK_E) & 0x80)
		_transform->Turn(Vec4(0.f, 1.f, 0.f, 0.f), timeDelta);

	if (gameInstance->KeyPressing(DIK_UP))
	{
		_transform->Forward(timeDelta, _pNavigation);

		/*if (animationcount < 2)
			++animationcount;*/

		//static_cast<PlayerBody*>(m_pPlayerPart[PART_BODY])->Set_AnimationIndex(animationcount, true);
	}

	if (gameInstance->KeyPressing(DIK_DOWN))
	{
		_transform->Backward(timeDelta, _pNavigation);

		//_transform->Backward(timeDelta);
		//if (animationcount > 0)
		//	--animationcount;

		//static_cast<PlayerBody*>(m_pPlayerPart[PART_BODY])->Set_AnimationIndex(animationcount, true);
	}

	for (auto& pPart : m_pPlayerPart)
	{
		if (nullptr != pPart)
			pPart->Tick(timeDelta);
	}

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
	transformDesc.speedPerSec = 5.f;
	transformDesc.rotationRadianPerSec = ::XMConvertToRadians(90.f);

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform), &transformDesc)))
		return E_FAIL;

	Navigation::NAVIGATION_DESC NavigationDesc;
	NavigationDesc._iCurrentIndex = 0;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeNavigation"),
		TEXT("ComponentNavigation"), reinterpret_cast<Component**>(&_pNavigation), &NavigationDesc)))
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
	Safe_Release<Navigation*>(_pNavigation);
}

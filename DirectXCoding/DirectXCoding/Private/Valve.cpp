#include "pch.h"
#include "Valve.h"

#include "GameInstance.h"
#include "Bounding_Sphere.h"
#include "ValveUI.h"

Valve::Valve(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: DynamicObject(device, deviceContext, DYNAMIC_TYPE::VALVE)
{

}

Valve::Valve(const Valve& rhs)
	: DynamicObject(rhs)
{
}

HRESULT Valve::InitializePrototype()
{
	return S_OK;
}

HRESULT Valve::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyCollider()))
		return E_FAIL;

	if (FAILED(ReadyValveUI()))
		return E_FAIL;

	if (nullptr != pArg)
		_transform->SetWorldMatrix(static_cast<ComponentNames*>(pArg)->_saveWorldMatrix);

	_iRotationTick = 100;

	return S_OK;
}

void Valve::Tick(const _float& timeDelta)
{
	if (_enabled)
		return;

	if (true == _bIsRotation)
	{
		--_iRotationTick;

		if (_id == 185)
			_transform->Turn(Vec4(1.f, 0.f, 0.f, 1.f), timeDelta);
		else if (_id == 186)
			_transform->Turn(Vec4(-1.f, 0.f, 0.f, 1.f), timeDelta);
		else
			_transform->Turn(Vec4(0.f, 0.f, 1.f, 1.f), timeDelta);


	
		if (-99 == _iRotationTick)
		{
			_iRotationTick = 100;
			_bIsRotation = false;

			_iRotationCount++;

			if (_iRotationCount == 2)
			{
				if (_id == 185)
				{
					GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

					pGameInstance->StopSound(SOUND_SCARE2);
					pGameInstance->PlaySound(TEXT("scary pad 2_9.wav"), SOUND_SCARE2, 1.f);

					RELEASE_INSTANCE(GameInstance);
				}

			}


			if (_iRotationCount == 3)
				_bIsOpen = true;
		}
		
	}


	if (_id == 186)
	{
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

		_float fVolume = 0.f;
		_float fDistance = 0.f;

		LerpSoundPlayer(fVolume, fDistance, 3.5f, pGameInstance);
		
		if (fDistance <= 3.5f)
			pGameInstance->PlaySoundLoop(TEXT("myhi.wav"), SOUND_ENVIRONMENT3, fVolume);

		RELEASE_INSTANCE(GameInstance);
	}

	_pCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());


	if (true == _bOnUI)
	{
		for (auto& pPart : _valvePart)
			pPart->Tick(timeDelta);
	}

}

void Valve::LateTick(const _float& timeDelta)
{
	__super::LateTick(timeDelta);

	if (true == _bOnUI)
	{
		for (auto& pPart : _valvePart)
			pPart->LateTick(timeDelta);
	}


#ifdef _DEBUG
	_render->AddDebug(_pCollider);
#endif // _DEBUG

}

HRESULT Valve::Render()
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

		if (FAILED(_binaryModel->BindMaterialTexture(_shader, "NormalMap", i, TextureType_NORMALS)))
			return E_FAIL;


		if (FAILED(_shader->Begin(0)))
			return E_FAIL;

		if (FAILED(_binaryModel->Render(i)))
			return E_FAIL;
	}


	if (true == _bOnUI)
	{
		for (auto& pPart : _valvePart)
			pPart->Render();
	}


	return S_OK;

}

HRESULT Valve::ReadyCollider()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == pGameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);


	Bounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc;
	{
		sphereDesc.vCenter = Vec3(0.f, 0.f, 0.f);
		sphereDesc.fRadius = 35.f;
		sphereDesc.pOwner = this;
	}

	if (FAILED(__super::AddComponent(level, TEXT("ProtoTypeSphereCollider"),
		TEXT("ComponentCollider"), reinterpret_cast<Component**>(&_pCollider), &sphereDesc)))
		return E_FAIL;

	Transform::TRANSFORM_DESC transformDesc;
	transformDesc.rotationRadianPerSec = ::XMConvertToRadians(90.f);


	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform), &transformDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);
}

void Valve::OnCollisionEnter(Collider* pOther)
{

}

void Valve::OnCollisionStay(Collider* pOther)
{
	if (OBJECT_TYPE::PLAYER == pOther->GetOwner()->GetObjectType())
		_bOnUI = true;
}

void Valve::OnCollisionExit(Collider* pOther)
{
	if (OBJECT_TYPE::PLAYER == pOther->GetOwner()->GetObjectType())
		_bOnUI = false;
}

void Valve::LerpSoundPlayer(_float& fVolume, _float& fDistance, _float fMaxDistance, GameInstance* pGameInstance)
{
	const _float fMaxVolume = 0.1f;
	const _float fMinVolume = 0.f;

	Vec4 vThisPos = _transform->GetState(Transform::STATE::POSITION);

	uint32 iCurrentLevel = pGameInstance->GetCurrentLevelIndex();

	Transform* pPlayerTransform = static_cast<Transform*>(pGameInstance->GetComponent(iCurrentLevel, LAYER_TAG::LAYER_PLAYER, TEXT("ComponentTransform"), "Player"));

	Vec4 vPlayerPos = pPlayerTransform->GetState(Transform::STATE::POSITION);

	Vec4 vDir = vPlayerPos - vThisPos;
	fDistance = vDir.Length();

	fVolume = fMaxVolume - (fDistance / fMaxDistance) * (fMaxVolume - fMinVolume);

	if (fVolume <= 0.f)
		fVolume = 0.f;
}

HRESULT Valve::ReadyValveUI()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	GameObject* pValvePart = nullptr;

	ValveUI::PART_DESC PartDesc;
	PartDesc.pParentTransform = _transform;
	PartDesc.iID = _id;

	pValvePart = pGameInstance->CloneGameObject(TEXT("ProtoTypeValveUI"), &PartDesc);
	if (nullptr == pValvePart)
		return E_FAIL;

	_valvePart.push_back(pValvePart);


	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

Valve* Valve::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Valve* pInstance = new Valve(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Create to Failed : Valve");
		Safe_Release<Valve*>(pInstance);
	}

	return pInstance;
}

GameObject* Valve::Clone(void* argument)
{
	Valve* pInstance = new Valve(*this);

	if (FAILED(pInstance->Initialize(argument)))
	{
		MSG_BOX("Create to Failed : BreakDoor");
		Safe_Release<Valve*>(pInstance);
	}

	return pInstance;
}

void Valve::Free()
{
	__super::Free();

	for (auto& pPart : _valvePart)
		Safe_Release<GameObject*>(pPart);

	_valvePart.clear();
}

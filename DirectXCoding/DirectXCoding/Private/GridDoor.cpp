#include "pch.h"
#include "GridDoor.h"

#include "GameInstance.h"
#include "Player.h"

GridDoor::GridDoor(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: DynamicObject(device, deviceContext, DYNAMIC_TYPE::GRID_DOOR)
{

}

GridDoor::GridDoor(const GridDoor& rhs)
	: DynamicObject(rhs)
{
}

HRESULT GridDoor::InitializePrototype()
{
	return S_OK;
}

HRESULT GridDoor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyCollider()))
		return E_FAIL;

	if (nullptr != pArg)
		_transform->SetWorldMatrix(static_cast<ComponentNames*>(pArg)->_saveWorldMatrix);

	return S_OK;
}

void GridDoor::Tick(const _float& timeDelta)
{
	if (_enabled)
		return;

	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
	Player* pPlayer = static_cast<Player*>(pGameInstance->GetLayerObjectTag(LAYER_TAG::LAYER_PLAYER, "Player"));
	_bool keyObtain = false;

	if (nullptr != pPlayer)
		keyObtain = pPlayer->GetObtainKey();

	// 여기서 뭐가 
	if (_id != 196 && true == _bIsRotation)
	{
		if(_id != 189)
			_transform->Turn(Vec4(0.f, -1.f, 0.f, 1.f), timeDelta);
		if (_id == 189 && true == keyObtain)
			_transform->Turn(Vec4(0.f, -1.f, 0.f, 1.f), timeDelta);
	}
	else if (_id == 196 && true == _bIsRotation)
	{
		if(true == keyObtain)
			_transform->Turn(Vec4(0.f, 1.f, 0.f, 1.f), timeDelta);
	}

	if (_bIsOpen == false)
	{
		_pCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());
		_pAssistCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());
	}

	RELEASE_INSTANCE(GameInstance);
}

void GridDoor::LateTick(const _float& timeDelta)
{
	__super::LateTick(timeDelta);

#ifdef _DEBUG
	if (_bIsOpen == false)
	{
		_render->AddDebug(_pCollider);
		_render->AddDebug(_pAssistCollider);
	}
#endif // _DEBUG
}

HRESULT GridDoor::Render()
{
	if (_enabled)
		return S_OK;


	if (FAILED(__super::BindShaderResource()))
		return E_FAIL;


	uint32 numMeshes = _binaryModel->GetNumMeshes();


	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
	Player* pPlayer = static_cast<Player*>(pGameInstance->GetLayerObjectTag(LAYER_TAG::LAYER_PLAYER, "Player"));
	_bool keyObtain = false;

	if (nullptr != pPlayer)
		keyObtain = pPlayer->GetObtainKey();

	for (size_t i = 0; i < numMeshes; i++)
	{
		if (FAILED(_binaryModel->BindMaterialTexture(_shader, "DiffuseMap", i, TextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(_binaryModel->BindMaterialTexture(_shader, "NormalMap", i, TextureType_NORMALS)))
			return E_FAIL;


		if (FAILED(_shader->Begin(0)))
			return E_FAIL;

		if (_id != 189 && _id != 196)
		{
			if (FAILED(_binaryModel->Render(i)))
				return E_FAIL;
		}
		else if (_id == 189 || _id == 196)
		{
			if (true == _bGridDoorKey)
			{
				if (FAILED(_binaryModel->Render(i)))
					return E_FAIL;
			}
			else if(i <= 0 && false == _bGridDoorKey)
				if(FAILED(_binaryModel->Render(i)))
					return E_FAIL;
		}

	}

	RELEASE_INSTANCE(GameInstance);

	return S_OK;

}

void GridDoor::OnCollisionEnter(Collider* pOther)
{

}

void GridDoor::OnCollisionStay(Collider* pOther)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	if (pOther->GetOwner()->GetObjectType() == OBJECT_TYPE::PLAYER)
	{
		if (pGameInstance->keyDown(DIK_E))
		{
			Player* pPlayer = static_cast<Player*>(pOther->GetOwner());
			_bGridDoorKey = pPlayer->GetObtainKey();

			if ((_id == 189 || _id == 196) && false == _bGridDoorKey)
			{
				pGameInstance->StopSound(SOUND_ENVIRONMENT5);
				pGameInstance->PlaySound(TEXT("closeddoor2.wav"), SOUND_ENVIRONMENT5, 1.f);
				pPlayer->SetCollDynamic(true);
				pPlayer->SetEventText(TEXT("Closed"));
			}
			else if((_id == 189 || _id == 196) && true == _bGridDoorKey)
			{
				pGameInstance->StopSound(SOUND_ENVIRONMENT5);
				pGameInstance->PlaySound(TEXT("unlock.wav"), SOUND_ENVIRONMENT5, 1.f);
			}
		}
	}

	if (false == _bIsRotation && _bIsOpen == false)
	{
		if (this->_id != 188 && this->_id != 195 && pOther->GetOwner()->GetObjectType() == OBJECT_TYPE::PLAYER)
		{

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
		else if(_id == 188 || _id == 195)
		{
			Vec3 vPlayerCenter = static_cast<BoundingAABB*>(pOther->GetBounding())->GetBounding()->Center;
			Vec3 vThisCenter = static_cast<Bounding_Sphere*>(_pCollider->GetBounding())->GetBounding()->Center;

			Vec3 vFinalCenter = vPlayerCenter - vThisCenter;

			_float vThisExtents = static_cast<Bounding_Sphere*>(_pCollider->GetBounding())->GetBounding()->Radius;
			Vec3 vPlayerExtents = static_cast<BoundingAABB*>(pOther->GetBounding())->GetBounding()->Extents;

 			Vec3 extents = 0.5f * Vec3(::fabs(vFinalCenter.x), ::fabs(vFinalCenter.y), ::fabs(vFinalCenter.z));

			if (extents.x >= extents.y && extents.x >= extents.z)
			{
				if (vPlayerCenter.x > vThisCenter.x)
				{
					_float vFinalExtents = fabs((vPlayerExtents.x + vThisExtents)) - fabs(vFinalCenter.x);

					Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
					vPos.x += vFinalExtents;

					pOther->GetOwner()->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
				}
				else
				{
					_float vFinalExtents = fabs((vPlayerExtents.x + vThisExtents)) - fabs(vFinalCenter.x);

					Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
					vPos.x -= vFinalExtents;

					pOther->GetOwner()->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
				}
			}
			else if (extents.y >= extents.x && extents.y >= extents.z)
			{

			}
			else
			{
				if (vPlayerCenter.z > vThisCenter.z)
				{
					_float vFinalExtents = fabs((vPlayerExtents.z + vThisExtents)) - fabs(vFinalCenter.z);

					Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
					vPos.z += vFinalExtents;

					pOther->GetOwner()->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
				}
				else
				{
					_float vFinalExtents = fabs((vPlayerExtents.z + vThisExtents)) - fabs(vFinalCenter.z);

					Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
					vPos.z -= vFinalExtents;

					pOther->GetOwner()->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
				}

			}
		}
	}
	

	RELEASE_INSTANCE(GameInstance);
}

void GridDoor::OnCollisionExit(Collider* pOther)
{
	if (pOther->GetOwner()->GetObjectType() == OBJECT_TYPE::PLAYER)
	{
		// F UI 사라지게.
		Player* pPlayer = static_cast<Player*>(pOther->GetOwner());
		pPlayer->SetCollDynamic(false);
	}
}

HRESULT GridDoor::ReadyCollider()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == pGameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);

	BoundingAABB::BOUNDING_AABB_DESC aabbDesc;
	{
		aabbDesc.vCenter = Vec3(45.f, 110.f, 0.f);
		aabbDesc.vExtents = Vec3(37.5f, 100.f, 5.f);
		aabbDesc.pOwner = this;
	}

	if (_id == 189)
	{
		Bounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc;
		sphereDesc.vCenter = Vec3(45.f, 110.f, 0.f);
		sphereDesc.fRadius = 20.f;
		sphereDesc.pOwner = this;

		if (FAILED(__super::AddComponent(level, TEXT("ProtoTypeSphereCollider"),
			TEXT("ComponentCollider"), reinterpret_cast<Component**>(&_pCollider), &sphereDesc)))
			return E_FAIL;
	}
	else if(_id == 196)
	{
		Bounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc;
		sphereDesc.vCenter = Vec3(45.f, 110.f, 0.f);
		sphereDesc.fRadius = 20.f;
		sphereDesc.pOwner = this;

		if (FAILED(__super::AddComponent(level, TEXT("ProtoTypeSphereCollider"),
			TEXT("ComponentCollider"), reinterpret_cast<Component**>(&_pCollider), &sphereDesc)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(__super::AddComponent(level, TEXT("ProtoTypeAABBCollider"),
			TEXT("ComponentCollider"), reinterpret_cast<Component**>(&_pCollider), &aabbDesc)))
			return E_FAIL;
	}

	Bounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc;
	{
		sphereDesc.fRadius = 5.f;
		sphereDesc.vCenter = Vec3(45.f, sphereDesc.fRadius * 10, 0.f);
		sphereDesc.pOwner = this;
	}

	if (_id == 189)
	{
		sphereDesc.fRadius = 20.f;
		sphereDesc.vCenter = Vec3(15.f, sphereDesc.fRadius * 10, 0.f);
	}

	if (_id == 196)
	{
		sphereDesc.fRadius = 20.f;
		sphereDesc.vCenter = Vec3(30.f, sphereDesc.fRadius * 10, 0.f);
	}

 	if (FAILED(__super::AddComponent(level, TEXT("ProtoTypeAssistSphereCollider"),
		TEXT("ComponentSphereCollider"), reinterpret_cast<Component**>(&_pAssistCollider), &sphereDesc)))
		return E_FAIL;

	Transform::TRANSFORM_DESC transDesc;
	{
		transDesc.speedPerSec = 0.f;
		transDesc.rotationRadianPerSec = ::XMConvertToRadians(180.f);
	}

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform), &transDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

GridDoor* GridDoor::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	GridDoor* pInstance = new GridDoor(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Create to Failed : GridDoor");
		Safe_Release<GridDoor*>(pInstance);
	}

	return pInstance;
}

GameObject* GridDoor::Clone(void* argument)
{
	GridDoor* pInstance = new GridDoor(*this);

	if (FAILED(pInstance->Initialize(argument)))
	{
		MSG_BOX("Create to Failed : GridDoor");
		Safe_Release<GridDoor*>(pInstance);
	}

	return pInstance;
}

void GridDoor::Free()
{
	__super::Free();
}

#include "pch.h"
#include "BreakDoor.h"

#include "GameInstance.h"
#include "BoundingOBB.h"
#include "WallPainting.h"

BreakDoor::BreakDoor(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: DynamicObject(device, deviceContext, DYNAMIC_TYPE::BREAK_DOOR)
{

}

BreakDoor::BreakDoor(const BreakDoor& rhs)
	: DynamicObject(rhs)
{

}

HRESULT BreakDoor::InitializePrototype()
{
	return S_OK;
}

HRESULT BreakDoor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyCollider()))
		return E_FAIL;

	if (nullptr != pArg)
		_transform->SetWorldMatrix(static_cast<ComponentNames*>(pArg)->_saveWorldMatrix);

	return S_OK;
}

void BreakDoor::Tick(const _float& timeDelta)
{
	if (_enabled)
		return;

	if (true == _bIsBreak)
	{
		if (false == _bIsOpen)
		{
			_vPrevPosition = static_cast<Bounding_Sphere*>(_pAssistCollider->GetBounding())->GetBounding()->Center;
			_transform->Turn(Vec4(0.f, 1.f, 0.f, 1.f), timeDelta);
		}
	}

	_pCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());
	_pAssistCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());
}

void BreakDoor::LateTick(const _float& timeDelta)
{
	__super::LateTick(timeDelta);

#ifdef _DEBUG
	_render->AddDebug(_pAssistCollider);
	_render->AddDebug(_pCollider);
#endif // _DEBUG

}

HRESULT BreakDoor::Render()
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

		if (true == _bIsBreak && i == 0)
			continue;

		if (FAILED(_binaryModel->Render(i)))
			return E_FAIL;
	}

	return S_OK;
}

void BreakDoor::OnCollisionEnter(Collider* pOther)
{
}

void BreakDoor::OnCollisionStay(Collider* pOther)
{
	if (_bIsBreak == true)
	{
		if (pOther->GetOwner()->GetObjectType() == OBJECT_TYPE::PLAYER && _id == 173)
		{
			GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

			if (false == _bChageMoment)
			{
				_fTime += pGameInstance->ComputeTimeDelta(TEXT("Timer_60")) * 130.f;

				if (_fTime >= 0.65f)
				{
					pGameInstance->StopAll();
					pGameInstance->PlayBGM(TEXT("bass #30425.wav"), 0.15f);
					pGameInstance->StopSound(SOUND_SCARE1);
					pGameInstance->PlaySound(TEXT("scary pad 2_12.wav"), SOUND_SCARE1, 0.8f);

					_bChageMoment = true;
				}
			}
			RELEASE_INSTANCE(GameInstance);
		}
	}


	if (false == _bIsOpen)
	{
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
		
		if (pOther->GetOwner()->GetObjectType() == OBJECT_TYPE::PLAYER)
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

		RELEASE_INSTANCE(GameInstance);
	}


}

void BreakDoor::OnCollisionExit(Collider* pOther)
{
}

void BreakDoor::OnAssistCollisionEnter(Collider* pOther)
{
	if (pOther->GetOwner()->GetModelName() == "PlayerBullet")
	{
		_bIsBreak = true;
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

		pGameInstance->DeleteObject(pOther->GetOwner());

		RELEASE_INSTANCE(GameInstance);
	}
}

void BreakDoor::OnAssistCollisionStay(Collider* pOther)
{
	if (pOther->GetOwner()->GetModelName() == "PlayerBullet")
		_bIsBreak = true;
}

void BreakDoor::OnAssistCollisionExit(Collider* pOther)
{
}

HRESULT BreakDoor::ReadyCollider()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == pGameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);


	BoundingAABB::BOUNDING_AABB_DESC aabbDesc;
	{
		aabbDesc.vCenter = Vec3(-45.f, 0.f, 0.f);
		aabbDesc.vExtents = Vec3(40.f, 100.f, 7.5f);
		aabbDesc.pOwner = this;
	}

	if (_id == 184)
	{
		aabbDesc.vCenter = Vec3(0.f, 0.f, 45.f);
		aabbDesc.vExtents = Vec3(7.5f, 100.f, 40.f);
	}

	if (FAILED(__super::AddComponent(level, TEXT("ProtoTypeAABBCollider"),
		TEXT("ComponentCollider"), reinterpret_cast<Component**>(&_pCollider), &aabbDesc)))
		return E_FAIL;

	Bounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc;
	{
		sphereDesc.fRadius = 15.f;
		sphereDesc.vCenter = Vec3(0.f, -15.f, -77.f);
		sphereDesc.pOwner = this;
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

HRESULT BreakDoor::ReadyDoorArt()
{
	return S_OK;
}

BreakDoor* BreakDoor::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	BreakDoor* pInstance = new BreakDoor(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Create to Failed : BreakDoor");
		Safe_Release<BreakDoor*>(pInstance);
	}

	return pInstance;
}

GameObject* BreakDoor::Clone(void* argument)
{
	BreakDoor* pInstance = new BreakDoor(*this);

	if (FAILED(pInstance->Initialize(argument)))
	{
		MSG_BOX("Create to Failed : BreakDoor");
		Safe_Release<BreakDoor*>(pInstance);
	}

	return pInstance;
}

void BreakDoor::Free()
{
	__super::Free();
}

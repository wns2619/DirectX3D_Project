#include "pch.h"
#include "OldWoodDoor.h"

#include "GameInstance.h"
#include "BoundingOBB.h"

OldWoodDoor::OldWoodDoor(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: DynamicObject(device, deviceContext, DYNAMIC_TYPE::OLD_DOOR)
{

}

OldWoodDoor::OldWoodDoor(const OldWoodDoor& rhs)
	: DynamicObject(rhs)
{
}

HRESULT OldWoodDoor::InitializePrototype()
{
	return S_OK;
}

HRESULT OldWoodDoor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyCollider()))
		return E_FAIL;

	if (nullptr != pArg)
		_transform->SetWorldMatrix(static_cast<ComponentNames*>(pArg)->_saveWorldMatrix);

	return S_OK;
}

void OldWoodDoor::Tick(const _float& timeDelta)
{
	if (_enabled)
		return;

	if (true == _bIsRotation)
		_transform->Turn(Vec4(0.f, 1.f, 0.f, 1.f), timeDelta);

	if (_bIsOpen == false)
	{
		_pCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());
		_pAssistCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());
	}
}

void OldWoodDoor::LateTick(const _float& timeDelta)
{
	__super::LateTick(timeDelta);

#ifdef _DEBUG
	_render->AddDebug(_pCollider);
	_render->AddDebug(_pAssistCollider);
#endif // _DEBUG
}

HRESULT OldWoodDoor::Render()
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


	return S_OK;

}

void OldWoodDoor::OnCollisionEnter(Collider* pOther)
{
}

void OldWoodDoor::OnCollisionStay(Collider* pOther)
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	if (false == _bIsRotation && _bIsOpen == false)
	{
		if (pOther->GetOwner()->GetObjectType() == OBJECT_TYPE::PLAYER)
		{

			Vec3 vPlayerCenter = static_cast<BoundingAABB*>(pOther->GetBounding())->GetBounding()->Center;
			Vec3 vthisCenter = static_cast<BoundingAABB*>(_pCollider->GetBounding())->GetBounding()->Center;


			Vec3 vFinalCenter = vPlayerCenter - vthisCenter;

			Vec3 vPlayerExtents = static_cast<BoundingAABB*>(pOther->GetBounding())->GetBounding()->Extents;
			Vec3 vThisExtents = static_cast<BoundingAABB*>(_pCollider->GetBounding())->GetBounding()->Extents;

			Vec3 extents = 0.5f * Vec3(::fabs(vFinalCenter.x), ::fabs(vFinalCenter.y), ::fabs(vFinalCenter.z));

			_float xOverlap = extents.x - fabs(vFinalCenter.x);
			_float yOverlap = extents.y - fabs(vFinalCenter.y);
			_float zOverlap = extents.z - fabs(vFinalCenter.z);

			_float minOverlap = std::min(std::min(xOverlap, yOverlap), zOverlap);

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


	RELEASE_INSTANCE(GameInstance);
}

void OldWoodDoor::OnCollisionExit(Collider* pOther)
{
}

HRESULT OldWoodDoor::ReadyCollider()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);


	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == pGameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);

	BoundingAABB::BOUNDING_AABB_DESC aabbDesc;
	{
		aabbDesc.vCenter = Vec3(40.f, 100.f, 0.f);
		aabbDesc.vExtents = Vec3(37.5f, 100.f, 7.5f);
		aabbDesc.pOwner = this;
	}

	//if (206 == _id)
	//{
	//	aabbDesc.vCenter = Vec3(0.f, 100.f, 50.f);
	//	aabbDesc.vExtents = Vec3(7.5f, 100.f, 47.5f);
	//}

	if (FAILED(__super::AddComponent(level, TEXT("ProtoTypeAABBCollider"),
		TEXT("ComponentCollider"), reinterpret_cast<Component**>(&_pCollider), &aabbDesc)))
		return E_FAIL;

	Bounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc;
	{
		sphereDesc.fRadius = 15.f;
		sphereDesc.vCenter = Vec3(35.f, sphereDesc.fRadius * 10, 0.f);
		sphereDesc.pOwner = this;
	}

	/*if (206 == _id)
		sphereDesc.vCenter = Vec3(55.f, sphereDesc.fRadius * 10, 0.f);*/

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

OldWoodDoor* OldWoodDoor::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	OldWoodDoor* pInstance = new OldWoodDoor(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Create to Failed : OldWoodDoor");
		Safe_Release<OldWoodDoor*>(pInstance);
	}

	return pInstance;
}

GameObject* OldWoodDoor::Clone(void* argument)
{
	OldWoodDoor* pInstance = new OldWoodDoor(*this);

	if (FAILED(pInstance->Initialize(argument)))
	{
		MSG_BOX("Create to Failed : OldWoodDoor");
		Safe_Release<OldWoodDoor*>(pInstance);
	}

	return pInstance;
}

void OldWoodDoor::Free()
{
	__super::Free();
}

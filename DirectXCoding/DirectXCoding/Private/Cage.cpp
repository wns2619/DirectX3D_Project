#include "pch.h"
#include "Cage.h"

#include "GameInstance.h"
#include "BoundingOBB.h"
#include "Valve.h"

Cage::Cage(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: DynamicObject(device, deviceContext, DYNAMIC_TYPE::CAGE)
{
}

Cage::Cage(const Cage& rhs)
	: DynamicObject(rhs)
{
}

HRESULT Cage::InitializePrototype()
{
	return S_OK;
}

HRESULT Cage::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyCollider()))
		return E_FAIL;

	if (nullptr != pArg)
		_transform->SetWorldMatrix(static_cast<ComponentNames*>(pArg)->_saveWorldMatrix);

	ValveTargetSet();

	return S_OK;
}

void Cage::Tick(const _float& timeDelta)
{
	if (_enabled)
		return;

	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	uint32 targetRotationCount = dynamic_cast<Valve*>(_pTargetObject)->GetRotationCount();
	_bool targetRotation = dynamic_cast<Valve*>(_pTargetObject)->GetIsRotatate();

	if (true == targetRotation && targetRotationCount + 1 <= 3)
	{
		Vec4 vPos = _transform->GetState(Transform::STATE::POSITION);
		vPos.y += 0.0035f;

		_transform->SetState(Transform::STATE::POSITION, vPos);
	}


	_pCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());

	RELEASE_INSTANCE(GameInstance);
}

void Cage::LateTick(const _float& timeDelta)
{
	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);

#ifdef _DEBUG
	_render->AddDebug(_pCollider);
#endif // _DEBUG
}

HRESULT Cage::Render()
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

		if (FAILED(_shader->Begin(0)))
			return E_FAIL;

		if (FAILED(_binaryModel->Render(i)))
			return E_FAIL;
	}

	return S_OK;
	
}

HRESULT Cage::ReadyCollider()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == pGameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);

	BoundingAABB::BOUNDING_AABB_DESC aabbDesc;
	{
		aabbDesc.vExtents = Vec3(80.f, 120.f, 5.f);
		aabbDesc.vCenter = Vec3(0.f, aabbDesc.vExtents.y, 0.f);
		aabbDesc.pOwner = this;
	}
	
	if (_id == 205)
		aabbDesc.vExtents = Vec3(5.f, 120.f, 80.f);

	if (FAILED(__super::AddComponent(level, TEXT("ProtoTypeAABBCollider"),
		TEXT("ComponentCollider"), reinterpret_cast<Component**>(&_pCollider), &aabbDesc)))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform))))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);
}

void Cage::OnCollisionEnter(Collider* pOther)
{
}

void Cage::OnCollisionStay(Collider* pOther)
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
				if (_id == 205)
				{
					_float vFinalExtents = fabs((vPlayerExtents.x + vThisExtents.x)) - fabs(vFinalCenter.x);

					Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
					vPos.x -= vFinalExtents;

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
			else
			{
				if (_id == 205)
				{
					_float vFinalExtents = fabs((vPlayerExtents.x + vThisExtents.x)) - fabs(vFinalCenter.x);

					Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
					vPos.x += vFinalExtents;

					pOther->GetOwner()->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
				}
				else
				{
					_float vFinalExtents = fabs((vPlayerExtents.z + vThisExtents.z)) - fabs(vFinalCenter.z);

					Vec4 vPos = pOther->GetOwner()->GetTransform()->GetState(Transform::STATE::POSITION);
					vPos.z += vFinalExtents;

					pOther->GetOwner()->GetTransform()->SetState(Transform::STATE::POSITION, vPos);
				}

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

void Cage::OnCollisionExit(Collider* pOther)
{
}

void Cage::ValveTargetSet()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	vector<GameObject*>* pGameList = pGameInstance->GetCurrentObjectList(LAYER_TAG::LAYER_DYNAMIC);

	if (_id == 205)
	{
		auto iter = find_if(pGameList->begin(), pGameList->end(), [&](GameObject* pObject)
			{
				if (pObject->GetIdNumber() == 184)
					return true;

				return false;
			});

		if (nullptr == (*iter))
			return;

		_pTargetObject = *iter;
	}
	else if (_id == 206)
	{
		auto iter = find_if(pGameList->begin(), pGameList->end(), [&](GameObject* pObject)
			{
				if (pObject->GetIdNumber() == 185)
					return true;

				return false;
			});

		if (nullptr == (*iter))
			return;

		_pTargetObject = *iter;
	}
	else if (_id == 207)
	{
		auto iter = find_if(pGameList->begin(), pGameList->end(), [&](GameObject* pObject)
			{
				if (pObject->GetIdNumber() == 186)
					return true;

				return false;
			});

		if (nullptr == (*iter))
			return;

		_pTargetObject = *iter;
	}
	else if (_id == 208)
	{
		auto iter = find_if(pGameList->begin(), pGameList->end(), [&](GameObject* pObject)
			{
				if (pObject->GetIdNumber() == 187)
					return true;

				return false;
			});

		if (nullptr == (*iter))
			return;

		_pTargetObject = *iter;
	}

	RELEASE_INSTANCE(GameInstance);

}

Cage* Cage::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Cage* pInstance = new Cage(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Create to Failed : Cage");
		Safe_Release<Cage*>(pInstance);
	}

	return pInstance;
}

GameObject* Cage::Clone(void* argument)
{
	Cage* pInstance = new Cage(*this);

	if (FAILED(pInstance->Initialize(argument)))
	{
		MSG_BOX("Create to Failed : Cage");
		Safe_Release<Cage*>(pInstance);
	}

	return pInstance;
}

void Cage::Free()
{
	__super::Free();
}

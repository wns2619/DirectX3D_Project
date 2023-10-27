#include "pch.h"
#include "GridDoor.h"

#include "GameInstance.h"
#include "BoundingOBB.h"
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

	XMVECTOR vLook = _transform->GetState(Transform::STATE::LOOK);

	vPrepareVector[0] = vLook;
	Matrix matY = Matrix::CreateRotationY(::XMConvertToRadians(90.f));
	vPrepareVector[1] = ::XMVector3TransformCoord(vLook, matY);
	matY = Matrix::CreateRotationY(::XMConvertToRadians(-90.f));
	vPrepareVector[2] = ::XMVector3TransformCoord(vLook, matY);

	return S_OK;
}

void GridDoor::Tick(const _float& timeDelta)
{
	if (_enabled)
		return;

	_pCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());
}

void GridDoor::LateTick(const _float& timeDelta)
{
	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
}

HRESULT GridDoor::Render()
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

#ifdef _DEBUG
	_pCollider->Render();
#endif // _DEBUG



	return S_OK;

}

void GridDoor::OnCollisionEnter(Collider* pOther)
{

}

void GridDoor::OnCollisionStay(Collider* pOther)
{
	if (pOther->GetOwner()->GetObjectType() == OBJECT_TYPE::PLAYER)
	{
		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

		// 플레이어의 Navi가 필요?
		BinaryNavi* pNavi = dynamic_cast<Player*>(pOther->GetOwner())->GetNavigation();
		_float timeDelta = pGameInstance->ComputeTimeDelta(TEXT("Timer_60"));

		RELEASE_INSTANCE(GameInstance);

		Vec3 vCenter = static_cast<BoundingOBB*>(pOther->GetBounding())->GetBounding()->Center;
		Vec3 vthisCenter = static_cast<BoundingOBB*>(_pCollider->GetBounding())->GetBounding()->Center;

		Vec3 vFinalCenter = vCenter - vthisCenter;
		Vec3 extents = 0.5f * Vec3(::fabs(vFinalCenter.x), ::fabs(vFinalCenter.y), ::fabs(vFinalCenter.z));

		if (extents.x >= extents.y && extents.x >= extents.z)
		{
			// 충돌이 X 축에서 발생.

			// TODO -> Left or Right
			int a = 0;

			if (vCenter.x > vthisCenter.x)
			{
				// 충돌 객체 X 좌표가 충돌체 X 좌표보다 크다면 충돌은 충돌체 왼쪽에서 오른쪽으로 발생한 것.
				a = 0;

			}
			else
			{
				a = 0;
			}
		}
		else if (extents.y >= extents.x && extents.y >= extents.z)
		{
			// Y축 발생.

			// TODO -> Top or Bottom

			int b = 0;

			if (vCenter.y > vthisCenter.y)
			{

			}
			else
			{

			}
		}
		else
		{
			// Z축에서 발생. 
			int c = 0;

			// TODO -> Back or Front.
			if (vCenter.z > vthisCenter.z)
			{
				Vec3 vLook = _transform->GetState(Transform::STATE::LOOK);

				_float test1 = vLook.Dot(vPrepareVector[2]);

				if(0 < vLook.Dot(vPrepareVector[2]))
					_transform->Turn(Vec4(0.f, -1.f, 0.f, 1.f), timeDelta);
			}
			else
			{
				c = 0;
				// y축 회전하는데 플레이어 z축 방향으로 .
				Vec3 vLook = _transform->GetState(Transform::STATE::LOOK);

				_float test1 = vLook.Dot(vPrepareVector[1]);

				if (0 > vLook.Dot(vPrepareVector[1]))
				{
					_transform->Turn(Vec4(0.f, 1.f, 0.f, 1.f), timeDelta);
				}
			}
		}

		//pOther->GetOwner()->_transform.
	}

	// 설정된 각에서 -90 ~ 90 이상 못 가게 .
}

void GridDoor::OnCollisionExit(Collider* pOther)
{

}

HRESULT GridDoor::ReadyCollider()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	BoundingOBB::BOUNDING_OBB_DESC obbDesc;
	{
		obbDesc.vCenter = Vec3(45.f, 110.f, 0.f);
		obbDesc.vExtents = Vec3(37.5f, 100.f, 5.f);
		obbDesc.vRotation = Quaternion(0.f, 0.f, 0.f, 1.f); 
		obbDesc.pOwner = this;
	}

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME),TEXT("ProtoTypeOBBCollider"),
		TEXT("ComponentCollider"), reinterpret_cast<Component**>(&_pCollider), &obbDesc)))
		return E_FAIL;

	Transform::TRANSFORM_DESC transDesc;
	{
		transDesc.speedPerSec = 2.f;
		transDesc.rotationRadianPerSec = ::XMConvertToRadians(720.f);
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

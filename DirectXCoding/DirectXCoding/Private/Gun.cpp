#include "pch.h"
#include "Gun.h"

#include "GameInstance.h"
#include "Bounding_Sphere.h"

Gun::Gun(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: DynamicObject(device, deviceContext, DYNAMIC_TYPE::GUN)
{
}

Gun::Gun(const Gun& rhs)
	: DynamicObject(rhs)
{

}

HRESULT Gun::InitializePrototype()
{
	return S_OK;
}

HRESULT Gun::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyCollider()))
		return E_FAIL;

	if (nullptr != pArg)
		_transform->SetWorldMatrix(static_cast<ComponentNames*>(pArg)->_saveWorldMatrix);

	return S_OK;
}

void Gun::Tick(const _float& timeDelta)
{
	if (_IsDead)
		return;

	_pCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());
}

void Gun::LateTick(const _float& timeDelta)
{
	if (_IsDead)
		return;

	__super::LateTick(timeDelta);

#ifdef _DEBUG
	_render->AddDebug(_pCollider);
#endif // _DEBUG

}

HRESULT Gun::Render()
{
	if (_enabled)
		return S_OK;

	if (_IsDead)
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

HRESULT Gun::ReadyCollider()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == pGameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);

	Bounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc;
	{
		sphereDesc.vCenter = Vec3(0.2f, 0.f, 0.f);
		sphereDesc.fRadius = 0.3f;
		sphereDesc.pOwner = this;
	}

	if (FAILED(__super::AddComponent(level, TEXT("ProtoTypeSphereCollider"),
		TEXT("ComponentCollider"), reinterpret_cast<Component**>(&_pCollider), &sphereDesc)))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform))))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

void Gun::OnCollisionEnter(Collider* pOther)
{
	int a = 0;
}

void Gun::OnCollisionStay(Collider* pOther)
{
	int a = 0;
}

void Gun::OnCollisionExit(Collider* pOther)
{
	int a = 0;
}

Gun* Gun::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Gun* pInstance = new Gun(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Create : Gun");
		Safe_Release<Gun*>(pInstance);
	}

	return pInstance;
}

GameObject* Gun::Clone(void* pArg)
{
	Gun* pInstance = new Gun(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : Gun");
		Safe_Release<Gun*>(pInstance);
	}

	return pInstance;
}

void Gun::Free()
{
	__super::Free();
}

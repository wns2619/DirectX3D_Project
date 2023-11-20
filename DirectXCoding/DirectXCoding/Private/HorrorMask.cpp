#include "pch.h"
#include "HorrorMask.h"

#include "GameInstance.h"
#include "Bounding_Sphere.h"

HorrorMask::HorrorMask(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: DynamicObject(device, deviceContext, DYNAMIC_TYPE::MASK)
{

}

HorrorMask::HorrorMask(const HorrorMask& rhs)
	: DynamicObject(rhs)
{

}

HRESULT HorrorMask::InitializePrototype()
{
	return S_OK;
}

HRESULT HorrorMask::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(ReadyCollider()))
		return E_FAIL;

	if (nullptr != pArg)
		_transform->SetWorldMatrix(static_cast<ComponentNames*>(pArg)->_saveWorldMatrix);

	return S_OK;
}

void HorrorMask::Tick(const _float& timeDelta)
{
	if (_enabled)
		return;

	_pCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());
}

void HorrorMask::LateTick(const _float& timeDelta)
{
	__super::LateTick(timeDelta);

#ifdef _DEBUG
	_render->AddDebug(_pCollider);
#endif // _DEBUG


}

HRESULT HorrorMask::Render()
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

HRESULT HorrorMask::ReadyCollider()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	uint32 level = static_cast<uint32>(LEVEL::GAME);

	if (static_cast<uint32>(LEVEL::EDIT) == pGameInstance->GetCurrentLevelIndex())
		level = static_cast<uint32>(LEVEL::EDIT);

	Bounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc;
	{
		sphereDesc.vCenter = Vec3(0.f, 0.f, 0.f);
		sphereDesc.fRadius = 20.f;
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

HorrorMask* HorrorMask::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	HorrorMask* pInstance = new HorrorMask(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Create to Failed : HorrorMask");
		Safe_Release<HorrorMask*>(pInstance);
	}

	return pInstance;
}

GameObject* HorrorMask::Clone(void* argument)
{
	HorrorMask* pInstance = new HorrorMask(*this);

	if (FAILED(pInstance->Initialize(argument)))
	{
		MSG_BOX("Create to Failed : HorrorMask");
		Safe_Release<HorrorMask*>(pInstance);
	}

	return pInstance;
}

void HorrorMask::Free()
{
	__super::Free();
}

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

	_pCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());
}

void OldWoodDoor::LateTick(const _float& timeDelta)
{
	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
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

HRESULT OldWoodDoor::ReadyCollider()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	BoundingOBB::BOUNDING_OBB_DESC obbDesc;
	{
		obbDesc.vCenter = Vec3(40.f, 100.f, 0.f);
		obbDesc.vExtents = Vec3(37.5f, 100.f, 7.5f);
		obbDesc.vRotation = Quaternion(0.f, 0.f, 0.f, 1.f);
		obbDesc.pOwner = this;
	}

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeOBBCollider"),
		TEXT("ComponentCollider"), reinterpret_cast<Component**>(&_pCollider), &obbDesc)))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform))))
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
		MSG_BOX("Create to Failed : BreakDoor");
		Safe_Release<OldWoodDoor*>(pInstance);
	}

	return pInstance;
}

void OldWoodDoor::Free()
{
	__super::Free();
}

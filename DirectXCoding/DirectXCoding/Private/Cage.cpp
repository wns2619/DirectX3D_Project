#include "pch.h"
#include "Cage.h"

#include "GameInstance.h"
#include "BoundingAABB.h"

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

	return S_OK;
}

void Cage::Tick(const _float& timeDelta)
{
	if (_enabled)
		return;

	_pCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());
}

void Cage::LateTick(const _float& timeDelta)
{
	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
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

#ifdef _DEBUG
	_pCollider->Render();
#endif // _DEBUG

	return S_OK;

}

HRESULT Cage::ReadyCollider()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	BoundingAABB::BOUNDING_AABB_DESC aabbDesc;
	{
		aabbDesc.vCenter = Vec3(0.f, 120.f, 0.f);
		aabbDesc.vExtents = Vec3(80.f, 120.f, 5.f);
		aabbDesc.pOwner = this;
	}

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeAABBColider"),
		TEXT("ComponentCollider"), reinterpret_cast<Component**>(&_pCollider), &aabbDesc)))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform))))
		return E_FAIL;

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

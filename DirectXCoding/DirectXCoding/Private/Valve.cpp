#include "pch.h"
#include "Valve.h"

#include "GameInstance.h"
#include "Bounding_Sphere.h"

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

	if (nullptr != pArg)
		_transform->SetWorldMatrix(static_cast<ComponentNames*>(pArg)->_saveWorldMatrix);

	return S_OK;
}

void Valve::Tick(const _float& timeDelta)
{
	if (_enabled)
		return;

	_pCollider->GetBounding()->Update(_transform->GetWorldMatrixCaculator());
}

void Valve::LateTick(const _float& timeDelta)
{
	if (!_enabled)
		_render->AddRenderGroup(Renderer::RENDERGROUP::NONBLEND, this);
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

HRESULT Valve::ReadyCollider()
{
	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	Bounding_Sphere::BOUNDING_SPHERE_DESC sphereDesc;
	{
		sphereDesc.vCenter = Vec3(0.f, 0.f, 0.f);
		sphereDesc.fRadius = 35.f;
		sphereDesc.pOwner = this;
	}

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeSphereColider"),
		TEXT("ComponentCollider"), reinterpret_cast<Component**>(&_pCollider), &sphereDesc)))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform))))
		return E_FAIL;

	RELEASE_INSTANCE(GameInstance);
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
}

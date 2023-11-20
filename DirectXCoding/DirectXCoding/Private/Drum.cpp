#include "pch.h"
#include "Drum.h"

#include "GameInstance.h"

Drum::Drum(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: DynamicObject(device, deviceContext, DYNAMIC_TYPE::DRUM)
{

}

Drum::Drum(const Drum& rhs)
	: DynamicObject(rhs)
{
}

HRESULT Drum::InitializePrototype()
{
	return S_OK;
}

HRESULT Drum::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform))))
		return E_FAIL;

	if (nullptr != pArg)
		_transform->SetWorldMatrix(static_cast<ComponentNames*>(pArg)->_saveWorldMatrix);

	return S_OK;
}

void Drum::Tick(const _float& timeDelta)
{
	if (_enabled)
		return;
}

void Drum::LateTick(const _float& timeDelta)
{
	__super::LateTick(timeDelta);;
}

HRESULT Drum::Render()
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

HRESULT Drum::ReadyCollider()
{


	return S_OK;
}

Drum* Drum::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Drum* pInstance = new Drum(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Create to Failed : Drum");
		Safe_Release<Drum*>(pInstance);
	}

	return pInstance;
}

GameObject* Drum::Clone(void* argument)
{
	Drum* pInstance = new Drum(*this);

	if (FAILED(pInstance->Initialize(argument)))
	{
		MSG_BOX("Create to Failed : BreakDoor");
		Safe_Release<Drum*>(pInstance);
	}

	return pInstance;
}

void Drum::Free()
{
	__super::Free();
}

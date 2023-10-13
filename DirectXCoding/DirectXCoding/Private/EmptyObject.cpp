#include "pch.h"
#include "EmptyObject.h"

EmptyObject::EmptyObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: GameObject(device, deviceContext, OBJECT_TYPE::STATIC)
{
	// Empty¿´´Ù°¡ 
}

EmptyObject::EmptyObject(const EmptyObject& rhs)
	: GameObject(rhs)
{
}

HRESULT EmptyObject::InitializePrototype()
{
	return S_OK;
}

HRESULT EmptyObject::Initialize(void* pArg)
{
	return S_OK;
}

void EmptyObject::Tick(const _float& timeDelta)
{
}

void EmptyObject::LateTick(const _float& timeDelta)
{
}

HRESULT EmptyObject::Render()
{
	return S_OK;
}

EmptyObject* EmptyObject::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	EmptyObject* Instance = new EmptyObject(device, deviceContext);

	if (FAILED(Instance->InitializePrototype()))
	{
		MSG_BOX("Create Failed EmptyObject");
		Safe_Release<EmptyObject*>(Instance);
	}

	return Instance;
}

GameObject* EmptyObject::Clone(void* argument)
{
	return nullptr;
}

void EmptyObject::Free()
{
}

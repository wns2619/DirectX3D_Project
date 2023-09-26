#include "pch.h"
#include "ColliderBase.h"

ColliderBase::ColliderBase(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Component(device, deviceContext)
{
}

ColliderBase::ColliderBase(const ColliderBase& rhs)
	: Component(rhs), _colliderType(rhs._colliderType)
{
}

Component* ColliderBase::Clone(void* argument)
{
	return nullptr;
}

void ColliderBase::Free()
{
	__super::Free();
}

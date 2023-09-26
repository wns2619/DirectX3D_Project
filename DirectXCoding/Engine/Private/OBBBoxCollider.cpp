#include "pch.h"
#include "OBBBoxCollider.h"
#include "AABBBoxCollider.h"
#include "SphereCollider.h"

OBBBoxCollider::OBBBoxCollider(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Collider(device, deviceContext, COLLIDER_TYPE::OBB)
{
	_colliderType = COLLIDER_TYPE::OBB;
}

OBBBoxCollider::OBBBoxCollider(const OBBBoxCollider& rhs)
	: Collider(rhs)
{
}

HRESULT OBBBoxCollider::InitializePrototype()
{
	return S_OK;
}

HRESULT OBBBoxCollider::Initialize(void* argument)
{
	return S_OK;
}

HRESULT OBBBoxCollider::Tick(const _float& timeDelta)
{
	return S_OK;
}

HRESULT OBBBoxCollider::Render()
{
	return S_OK;
}

_bool OBBBoxCollider::Intersects(Ray& ray, OUT _float& distance)
{
	return _boundingBox.Intersects(ray.position, ray.direction, OUT distance);
}

_bool OBBBoxCollider::Intersects(Collider* other)
{
	COLLIDER_TYPE type = other->GetColliderType();

	switch (type)
	{
	case COLLIDER_TYPE::SPHERE:
		return _boundingBox.Intersects(dynamic_cast<SphereCollider*>(other)->GetBoundSphere());
	case COLLIDER_TYPE::AABB:
		return _boundingBox.Intersects(dynamic_cast<AABBBoxCollider*>(other)->GetBoundBox());
	case COLLIDER_TYPE::OBB:
		return _boundingBox.Intersects(dynamic_cast<OBBBoxCollider*>(other)->GetBoundingBox());
	}

	return false;
}

void OBBBoxCollider::OnCollisionEnter(Collider* other)
{
}

void OBBBoxCollider::OnCollisionStay(Collider* other)
{
}

void OBBBoxCollider::OnCollisionExit(Collider* other)
{
}

OBBBoxCollider* OBBBoxCollider::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	OBBBoxCollider* OBB = new OBBBoxCollider(device, deviceContext);

	if (FAILED(OBB->InitializePrototype()))
	{
		MSG_BOX("OBB Create Failed : Proto");
		Safe_Release<OBBBoxCollider*>(OBB);
		return nullptr;
	}

	return OBB;
}

Component* OBBBoxCollider::Clone(void* argument)
{
	OBBBoxCollider* OBB = new OBBBoxCollider(*this);

	if (FAILED(OBB->Initialize(argument)))
	{
		MSG_BOX("OBB Create Failed : Clone");
		Safe_Release<OBBBoxCollider*>(OBB);
		return nullptr;
	}

	return OBB;
}

void OBBBoxCollider::Free()
{
	__super::Free();
}

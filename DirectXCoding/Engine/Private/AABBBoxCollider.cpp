#include "pch.h"
#include "AABBBoxCollider.h"
#include "SphereCollider.h"
#include "OBBBoxCollider.h"

AABBBoxCollider::AABBBoxCollider(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Collider(device, deviceContext, COLLIDER_TYPE::AABB)
{
	_colliderType = COLLIDER_TYPE::AABB;
}

AABBBoxCollider::AABBBoxCollider(const AABBBoxCollider& rhs)
	: Collider(rhs)
{

}

HRESULT AABBBoxCollider::InitializePrototype()
{
	return S_OK;
}

HRESULT AABBBoxCollider::Initialize(void* argument)
{
	return S_OK;
}

HRESULT AABBBoxCollider::Tick(const _float& timeDelta)
{
	return S_OK;
}

HRESULT AABBBoxCollider::Render()
{
	return S_OK;
}

_bool AABBBoxCollider::Intersects(Ray& ray, OUT _float& distance)
{
	return _boundingBox.Intersects(ray.position, ray.direction, OUT distance);
}

_bool AABBBoxCollider::Intersects(Collider* other)
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

void AABBBoxCollider::OnCollisionEnter(Collider* other)
{
}

void AABBBoxCollider::OnCollisionStay(Collider* other)
{
}

void AABBBoxCollider::OnCollisionExit(Collider* other)
{
}

AABBBoxCollider* AABBBoxCollider::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	AABBBoxCollider* ABB = new AABBBoxCollider(device, deviceContext);

	if (FAILED(ABB->InitializePrototype()))
	{
		MSG_BOX("ABB Create Failed : Proto");
		Safe_Release<AABBBoxCollider*>(ABB);
		return nullptr;
	}

	return ABB;
}

Component* AABBBoxCollider::Clone(void* argument)
{
	AABBBoxCollider* ABB = new AABBBoxCollider(*this);

	if (FAILED(ABB->Initialize(argument)))
	{
		MSG_BOX("ABB Create Failed : Clone");
		Safe_Release<AABBBoxCollider*>(ABB);
		return nullptr;
	}

	return ABB;
}

void AABBBoxCollider::Free()
{
	__super::Free();
}

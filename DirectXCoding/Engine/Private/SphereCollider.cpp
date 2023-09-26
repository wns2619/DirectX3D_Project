#include "pch.h"
#include "SphereCollider.h"
#include "AABBBoxCollider.h"
#include "OBBBoxCollider.h"

SphereCollider::SphereCollider(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Collider(device, deviceContext, COLLIDER_TYPE::SPHERE)
{
	_colliderType = COLLIDER_TYPE::SPHERE;
}

SphereCollider::SphereCollider(const SphereCollider& rhs)
	: Collider(rhs)
{

}

HRESULT SphereCollider::InitializePrototype()
{
	return S_OK;
}

HRESULT SphereCollider::Initialize(void* argument)
{
	return S_OK;
}

HRESULT SphereCollider::Tick(const _float& timeDelta)
{
	//_boundingSphere.Center = GetGameObject()->GetTransform()->GetPosition();

	//Vec3 scale = GetGameObject()->GetTransform()->GetScale();
	//_boundingSphere.Radius = _radius * max(max(scale.x, scale.y), scale.z);

	return S_OK;
}

HRESULT SphereCollider::Render()
{
	return S_OK;
}

_bool SphereCollider::Intersects(Ray& ray, OUT _float& distance)
{
	return _boundingSphere.Intersects(ray.position, ray.direction, OUT distance);
}

_bool SphereCollider::Intersects(Collider* other)
{
	COLLIDER_TYPE type = other->GetColliderType();

	switch (type)
	{
	case COLLIDER_TYPE::SPHERE:
		return _boundingSphere.Intersects(dynamic_cast<SphereCollider*>(other)->GetBoundSphere());
	case COLLIDER_TYPE::AABB:
		return _boundingSphere.Intersects(dynamic_cast<AABBBoxCollider*>(other)->GetBoundBox());
	case COLLIDER_TYPE::OBB:
		return _boundingSphere.Intersects(dynamic_cast<OBBBoxCollider*>(other)->GetBoundingBox());
	}

	return false;
}

void SphereCollider::OnCollisionEnter(Collider* other)
{
}

void SphereCollider::OnCollisionStay(Collider* other)
{
}

void SphereCollider::OnCollisionExit(Collider* other)
{
}

SphereCollider* SphereCollider::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	SphereCollider* sphere = new SphereCollider(device, deviceContext);

	if (FAILED(sphere->InitializePrototype()))
	{
		MSG_BOX("ABB Create Failed : Clone");
		Safe_Release<SphereCollider*>(sphere);
		return nullptr;
	}

	return sphere;
}

Component* SphereCollider::Clone(void* argument)
{
	SphereCollider* sphere = new SphereCollider(*this);

	if (FAILED(sphere->Initialize(argument)))
	{
		MSG_BOX("ABB Create Failed : Clone");
		Safe_Release<SphereCollider*>(sphere);
		return nullptr;
	}

	return sphere;
}

void SphereCollider::Free()
{
	__super::Free();
}

#pragma once
#include "Component.h"

BEGIN(Engine)

enum class COLLIDER_TYPE { SPHERE, AABB, OBB, COLLIDER_END };

class ColliderBase abstract : public Component
{
protected:
	explicit ColliderBase(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit ColliderBase(const ColliderBase& rhs);
	virtual ~ColliderBase();

public:
	virtual _bool Intersects(Ray& ray, _float& distance) = 0;
	virtual _bool Intersects(ColliderBase& other) = 0;

	COLLIDER_TYPE GetColliderType() { return _colliderType; }

protected:
	COLLIDER_TYPE _colliderType = COLLIDER_TYPE::COLLIDER_END;

public:
	virtual void Free() override;
};


END


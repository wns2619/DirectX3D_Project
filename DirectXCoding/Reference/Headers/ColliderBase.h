#pragma once
#include "Component.h"

BEGIN(Engine)

enum class COLLIDER_TYPE { SPHERE, AABB, OBB, COLLIDER_END };

class ENGINE_DLL ColliderBase : public Component
{
protected:
	explicit ColliderBase(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit ColliderBase(const ColliderBase& rhs);
	virtual ~ColliderBase() = default;

public:
	virtual _bool Intersects(Ray& ray, _float& distance) { return false; };
	virtual _bool Intersects(ColliderBase* other) { return false; };

	COLLIDER_TYPE GetColliderType() { return _colliderType; }

protected:
	COLLIDER_TYPE _colliderType = COLLIDER_TYPE::COLLIDER_END;

public:
	virtual Component* Clone(void* argument);
	virtual void Free() override;
};


END


#pragma once
#include "Component.h"

BEGIN(Engine)

class GameObject;

enum class COLLIDER_TYPE { SPHERE, AABB, OBB, COLLIDER_END, };


class ENGINE_DLL Collider abstract : public Component
{
protected:
	explicit Collider(ID3D11Device* device, ID3D11DeviceContext* deviceContext, COLLIDER_TYPE type);
	explicit Collider(const Collider& rhs);
	virtual ~Collider() = default;
public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* argument);

	virtual HRESULT Tick(const _float& timeDelta);
	virtual HRESULT Render();

	virtual _bool Intersects(Ray& ray, OUT _float& distance) = 0;
	virtual _bool Intersects(Collider* other) = 0;

public:
	virtual void OnCollisionEnter(Collider* other);
	virtual void OnCollisionStay(Collider* other);
	virtual void OnCollisionExit(Collider* other);

public:
	COLLIDER_TYPE GetColliderType() { return _colliderType; }

protected:
	COLLIDER_TYPE _colliderType = COLLIDER_TYPE::COLLIDER_END;

	GameObject* _owner = nullptr;
	int32 _ColCount = 0;

	// Com //
	ID3D11Buffer* _vertexBuffer = nullptr;
	ID3D11Buffer* _indexBuffer	= nullptr;

public:
	virtual Component* Clone(void* argument) = 0;
	virtual void Free() override;
};

END
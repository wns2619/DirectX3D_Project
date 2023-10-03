#pragma once

#include "Component.h"
#include "Transform.h"

BEGIN(Engine)

const _float GRAVITY = 0.982f;

class ENGINE_DLL Physics final : public Component 
{
public:
	struct Physics_Desc
	{
		Vec3 _velocity;
		Vec3 _oldVelocity;
		Vec3 _acceleration;
		Vec3 _deceleration;

		_float _accelMultiplier = 1.f;
		_float _accelSpeedUpMultiplier = 1.f;
		_float _decelMultiplier = 1.f;
		_float _maxSpeed;
		_float _mass;

		BoundingBox* _aabb = nullptr;
	};
private:
	explicit Physics(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Physics(const Physics& rhs);
	virtual ~Physics() = default;

public:
	virtual HRESULT InitializePrototype()	   override;
	virtual HRESULT Initialize(void* argument) override;

	void updatePosition(Transform* trans, const _float& timeDelta);
public:
	const Physics_Desc& GetPhysicsDesc() { return _physicsDesc; }

public:
	void addForce(Vec3 force, Transform* trans, const _float& timeDelta);
	void addForceDir(Transform::Direction dir, Transform* trans, const _float& timeDelta, _float multiplier = 1.f);
	void addForceDir(XMVECTOR dir, Transform* trans, const _float& timeDelta, _float multiplier = 1.f);
	void addGravity(Transform* trans, const _float& timeDelta);
	void addYDecel(Transform* trans, const _float& timeDelta);
	
	_float decelrate(_float source, _float target, _float smoothing, const _double& timeDelta);
private:
	Physics_Desc _physicsDesc = {};

public:
	static Physics* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual Component* Clone(void* argument) override;
	virtual void Free() override;
};


END


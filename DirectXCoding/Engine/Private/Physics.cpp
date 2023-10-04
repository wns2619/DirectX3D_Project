#include "pch.h"
#include "Physics.h"

Physics::Physics(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Component(device, deviceContext, COMPONENT_TYPE::PHYSICS)
{

}

Physics::Physics(const Physics& rhs)
	: Component(rhs), _physicsDesc(rhs._physicsDesc)
{
	//if (_physicsDesc._aabb)
	//	Safe_Delete<BoundingBox*>(_physicsDesc._aabb);

	//if (rhs._physicsDesc._aabb != nullptr)
	//	_physicsDesc._aabb = new BoundingBox(*rhs._physicsDesc._aabb);
}

HRESULT Physics::InitializePrototype()
{
	return S_OK;
}

HRESULT Physics::Initialize(void* argument)
{
	if (argument != nullptr)
	{
		Physics_Desc* desc = static_cast<Physics_Desc*>(argument);
		::memmove(&_physicsDesc, desc, sizeof(Physics_Desc));

		//_physicsDesc._aabb = new BoundingBox(*desc->_aabb);
	}

	return S_OK;
}

void Physics::updatePosition(Transform* trans, const _float& timeDelta)
{
	_physicsDesc._velocity.x = _physicsDesc._velocity.x - (5.f * _physicsDesc._velocity.x * timeDelta);
	_physicsDesc._velocity.y = _physicsDesc._velocity.y - (5.f * _physicsDesc._velocity.y * timeDelta);
	_physicsDesc._velocity.z = _physicsDesc._velocity.z - (5.f * _physicsDesc._velocity.z * timeDelta);

	trans->SetState(Transform::STATE::POSITION, ::XMVectorAdd(trans->GetState(Transform::STATE::POSITION), ::XMLoadFloat3(&_physicsDesc._velocity) * timeDelta));
	//trans->UpdateDirVectors();
}

void Physics::addForce(Vec3 force, Transform* trans, const _float& timeDelta)
{
	_physicsDesc._velocity = 
		Vec3(_physicsDesc._velocity.x + force.x, _physicsDesc._velocity.y + force.y, _physicsDesc._velocity.z + force.z);

	updatePosition(trans, timeDelta);
}

void Physics::addForceDir(Transform::Direction dir, Transform* trans, const _float& timeDelta, _float multiplier)
{
	XMVECTOR finalForce = ::XMVectorZero();

	switch (dir)
	{
	case Engine::Transform::Direction::FORWARD:
		finalForce = ::XMVectorScale(trans->GetState(Transform::STATE::LOOK), _physicsDesc._acceleration.z * _physicsDesc._accelMultiplier * multiplier);
		break;
	case Engine::Transform::Direction::BACKWARD:
		finalForce -= ::XMVectorScale(trans->GetState(Transform::STATE::LOOK), _physicsDesc._acceleration.z * _physicsDesc._accelMultiplier * multiplier);
		break;
	case Engine::Transform::Direction::LEFT:
		finalForce -= ::XMVectorScale(trans->GetState(Transform::STATE::RIGHT), _physicsDesc._acceleration.x * _physicsDesc._accelMultiplier * multiplier);
		break;
	case Engine::Transform::Direction::RIGHT:
		finalForce = ::XMVectorScale(trans->GetState(Transform::STATE::RIGHT), _physicsDesc._acceleration.x * _physicsDesc._accelMultiplier * multiplier);
		break;
	case Engine::Transform::Direction::UP:
		finalForce -= ::XMVectorScale(trans->GetState(Transform::STATE::UP), _physicsDesc._acceleration.y * _physicsDesc._accelMultiplier * multiplier);
		break;
	case Engine::Transform::Direction::DOWN:
		finalForce = ::XMVectorScale(trans->GetState(Transform::STATE::UP), _physicsDesc._acceleration.y * _physicsDesc._accelMultiplier * multiplier);
		break;
	default:
		MSG_BOX("Direction not Found");
		break;
	}

	finalForce += ::XMLoadFloat3(&_physicsDesc._velocity);
	_physicsDesc._velocity = finalForce;

	updatePosition(trans, timeDelta);
}

void Physics::addForceDir(XMVECTOR dir, Transform* trans, const _float& timeDelta, _float multiplier)
{
	XMVECTOR finalForce = ::XMVectorZero();

	finalForce = ::XMVectorScale(dir, _physicsDesc._acceleration.x * _physicsDesc._accelMultiplier * multiplier);

	Vec3 finalForceV3;
	::XMStoreFloat3(&finalForceV3, finalForce);

	_physicsDesc._velocity =
		Vec3(_physicsDesc._velocity.x + finalForceV3.x, 
			_physicsDesc._velocity.y + finalForceV3.y, 
			_physicsDesc._velocity.z + finalForceV3.z);

	updatePosition(trans, timeDelta);
}

void Physics::addGravity(Transform* trans, const _float& timeDelta)
{
	_physicsDesc._velocity.y += _physicsDesc._mass * -GRAVITY * timeDelta;

	updatePosition(trans, timeDelta);
}

void Physics::addYDecel(Transform* trans, const _float& timeDelta)
{
	_physicsDesc._velocity.y *= _physicsDesc._deceleration.y * timeDelta;

	updatePosition(trans, timeDelta);
}

_float Physics::decelrate(_float source, _float target, _float smoothing, const _double& timeDelta)
{
	return lerpF(source, target, static_cast<_float>(1.0 - pow(smoothing, timeDelta)));
}

Physics* Physics::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Physics* physics = new Physics(device, deviceContext);

	if (FAILED(physics->InitializePrototype()))
	{
		MSG_BOX("Create Failed Prototype : physics");
		Safe_Release<Physics*>(physics);
		return nullptr;
	}

	return physics;
}

Component* Physics::Clone(void* argument)
{
	Physics* physics = new Physics(*this);

	if (FAILED(physics->Initialize(argument)))
	{
		MSG_BOX("Create Failed Cloned : physics");
		Safe_Release<Physics*>(physics);
		return nullptr;
	}

	return physics;
}

void Physics::Free()
{
	__super::Free();

	Safe_Delete<BoundingBox*>(_physicsDesc._aabb);
}

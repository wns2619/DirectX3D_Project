#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL SphereCollider final : public Collider
{
private:
	explicit SphereCollider(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit SphereCollider(const SphereCollider& rhs);
	virtual ~SphereCollider() = default;

public:
	virtual HRESULT InitializePrototype()		override;
	virtual HRESULT Initialize(void* argument)	override;

	virtual HRESULT Tick(const _float& timeDelta)	override;
	virtual HRESULT Render()						override;

	virtual _bool Intersects(Ray& ray, OUT _float& distance) override;
	virtual _bool Intersects(Collider* other)				 override;

public:
	BoundingSphere& GetBoundSphere() { return _boundingSphere; }

public:
	virtual void OnCollisionEnter(Collider* other) override;
	virtual void OnCollisionStay(Collider* other)  override;
	virtual void OnCollisionExit(Collider* other)  override;

private:
	_float _radius = 1.f;
	BoundingSphere _boundingSphere;

public:
	static SphereCollider* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual Component* Clone(void* argument) override;
	virtual void Free() override;
};

END


#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL OBBBoxCollider final : public Collider
{
private:
	explicit OBBBoxCollider(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit OBBBoxCollider(const OBBBoxCollider& rhs);
	virtual ~OBBBoxCollider() = default;

public:
	virtual HRESULT InitializePrototype()		override;
	virtual HRESULT Initialize(void* argument)	override;

	virtual HRESULT Tick(const _float& timeDelta)	override;
	virtual HRESULT Render()						override;

	virtual _bool Intersects(Ray& ray, OUT _float& distance) override;
	virtual _bool Intersects(Collider* other)				 override;

public:
	BoundingOrientedBox& GetBoundingBox() { return _boundingBox; }

public:
	virtual void OnCollisionEnter(Collider* other) override;
	virtual void OnCollisionStay(Collider* other)  override;
	virtual void OnCollisionExit(Collider* other)  override;

private:
	BoundingOrientedBox _boundingBox;

public:
	static OBBBoxCollider* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual Component* Clone(void* argument) override;
	virtual void Free() override;
};

END

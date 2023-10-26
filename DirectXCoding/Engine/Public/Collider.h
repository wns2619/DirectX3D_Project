#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL Collider final : public Component
{
public:
	enum COLLIDER_TYPE { AABB, OBB, SPHERE, TYPE_END };

private:
	explicit Collider(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Collider(const Collider& rhs);
	virtual ~Collider() = default;
public:
	virtual HRESULT InitializePrototype(COLLIDER_TYPE eType);
	virtual HRESULT Initialize(void* argument);

#ifdef _DEBUG
	virtual HRESULT Render();
#endif

	virtual _bool Intersects(Ray& ray, OUT _float& distance);
	virtual _bool Intersects(Collider* other);

public:
	virtual void OnCollisionEnter(Collider* other);
	virtual void OnCollisionStay(Collider* other);
	virtual void OnCollisionExit(Collider* other);

public:
	class Bounding* GetBounding() { return _pBounding; }

private:
	COLLIDER_TYPE _eColliderType = COLLIDER_TYPE::TYPE_END;
	class Bounding* _pBounding = nullptr;

#ifdef _DEBUG
private:
	/* Vertex, IndexBuffer를 생성하고 그리는 기능을 제공해주는 객체*/
	PrimitiveBatch<DirectX::VertexPositionColor>* _pBatch = nullptr;
	BasicEffect* _pEffect = nullptr;


#endif // _DEBUG


public:
	static Collider* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, COLLIDER_TYPE eType);
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};

END
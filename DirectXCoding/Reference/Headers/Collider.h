#pragma once
#include "Component.h"

BEGIN(Engine)

union COLLIDER_ID
{
	struct
	{
		uint32 LeftID;
		uint32 RightID;
	};
	uint64 ID;
};

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
public:
	_bool IsCollisition(Collider* pTargetCol);

public:
	virtual void OnCollisionEnter(Collider* other);
	virtual void OnCollisionStay(Collider* other);
	virtual void OnCollisionExit(Collider* other);

public:
	class Bounding* GetBounding() { return _pBounding; }
	class GameObject* GetOwner() { return _pOwner; }
	COLLIDER_TYPE& GetColliderType() { return _eColliderType; }
	uint32 GetID() { return _iID; }
private:
	COLLIDER_TYPE _eColliderType = COLLIDER_TYPE::TYPE_END;
	class Bounding* _pBounding = nullptr;
	class GameObject* _pOwner = nullptr;

	uint32 _iID = 0;
	static uint32 _giNextID;

	int32 _iCol = 0;

#ifdef _DEBUG
private:
	/* Vertex, IndexBuffer를 생성하고 그리는 기능을 제공해주는 객체*/
	PrimitiveBatch<DirectX::VertexPositionColor>* _pBatch = nullptr;
	BasicEffect* _pEffect = nullptr;
	ID3D11InputLayout* _pInputLayOut = nullptr;


#endif // _DEBUG


public:
	static Collider* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, COLLIDER_TYPE eType);
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};

END
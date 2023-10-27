#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL Bounding abstract : public Base
{
public:
	struct BOUNDING_DESC
	{
		Vec3 vCenter;
		class GameObject* pOwner = nullptr;
	};

protected:
	Bounding();
	virtual ~Bounding() = default;

public:
	virtual HRESULT Initialize(BOUNDING_DESC* pDesc);
	virtual void Update(FXMMATRIX mTransformMatrix) = 0;

public:
	virtual _bool IsCollision(Collider::COLLIDER_TYPE eType, Bounding* pBounding) = 0;

public:
	void SetColl(_bool Col) { _IsColl = Col; }
	_bool GetColl() { return _IsColl; }

protected:
	_bool _IsColl = false;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) = 0;
#endif // _DEBUG

public:
	virtual void Free() override;
};

END


#pragma once

#include "Bounding.h"

BEGIN(Engine)

class ENGINE_DLL Bounding_Frustum final : public Bounding
{
public:
	struct BOUNDING_FRUSTUM_DESC : public Bounding::BOUNDING_DESC
	{
		// center(Origin) + owner
		_float fLeftSlope = 0.f;
		_float fRightSlope = 0.f;
		_float fTopSlope = 0.f;
		_float fBottomSlope = 0.f;
		_float fNear = 0.f;
		_float fFar = 0.f;

		Vec4 vOrientation;
	};

private:
	Bounding_Frustum();
	virtual ~Bounding_Frustum() = default;

public:
	virtual HRESULT Initialize(BOUNDING_DESC* pDesc) override;
	virtual void Update(FXMMATRIX TransformMatrix) override;

public:
	const BoundingFrustum* GetBounding() const { return _pFrustum; }

public:
	virtual _bool IsCollision(Collider::COLLIDER_TYPE eType, Bounding* pBounding);

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
#endif // _DEBUG


private:
	BoundingFrustum* _pFrustum_Original = nullptr;
	BoundingFrustum* _pFrustum = nullptr;

public:
	static Bounding_Frustum* Create(BOUNDING_DESC* pDesc);
	virtual void Free() override;
};

END


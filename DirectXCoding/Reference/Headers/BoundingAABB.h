#pragma once

#include "Bounding.h"

BEGIN(Engine)

class ENGINE_DLL BoundingAABB final : public Bounding
{
public:
	struct BOUNDING_AABB_DESC : public Bounding::BOUNDING_DESC
	{
		Vec3 vExtents;
	};

private:
	BoundingAABB();
	virtual ~BoundingAABB() = default;

public:
	virtual HRESULT Initialize(BOUNDING_DESC* pDesc) override;
	virtual void Update(FXMMATRIX TransformMatrix) override;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
#endif // _DEBUG

private:
	BoundingBox* _pAABB_Original = nullptr;
	BoundingBox* _pAABB = nullptr;

public:
	static BoundingAABB* Create(BOUNDING_DESC* pDesc);
	virtual void Free() override;

};

END


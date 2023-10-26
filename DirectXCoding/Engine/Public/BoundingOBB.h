#pragma once

#include "Bounding.h"

BEGIN(Engine)

class ENGINE_DLL BoundingOBB final : public Bounding
{
public:
	struct BOUNDING_OBB_DESC : public Bounding::BOUNDING_DESC
	{
		Vec3 vExtents;
		Vec4 vRotation;
	};

private:
	BoundingOBB();
	virtual ~BoundingOBB() = default;

public:
	virtual HRESULT Initialize(BOUNDING_DESC* pDesc) override;
	virtual void Update(FXMMATRIX TransformMatrix) override;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
#endif // _DEBUG

private:
	BoundingOrientedBox* _pOBB_Original = nullptr;
	BoundingOrientedBox* _pOBB = nullptr;

public:
	static BoundingOBB* Create(BOUNDING_DESC* pDesc);
	virtual void Free() override;

};

END


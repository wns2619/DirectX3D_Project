#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL Bounding abstract : public Base
{
public:
	struct BOUNDING_DESC
	{
		Vec3 vCenter;
	};

protected:
	Bounding();
	virtual ~Bounding() = default;

public:
	virtual HRESULT Initialize(BOUNDING_DESC* pDesc);
	virtual void Update(FXMMATRIX mTransformMatrix) = 0;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) = 0;
#endif // _DEBUG

public:
	virtual void Free() override;
};

END


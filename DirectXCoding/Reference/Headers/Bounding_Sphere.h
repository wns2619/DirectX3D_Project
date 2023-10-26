#pragma once

#include "Bounding.h"

BEGIN(Engine)

class ENGINE_DLL Bounding_Sphere final : public Bounding
{
public:
	struct BOUNDING_SPHERE_DESC : public Bounding::BOUNDING_DESC
	{
		_float fRadius;
	};

private:
	Bounding_Sphere();
	virtual ~Bounding_Sphere() = default;

public:
	virtual HRESULT Initialize(BOUNDING_DESC* pDesc) override;
	virtual void Update(FXMMATRIX TransformMatrix) override;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
#endif // _DEBUG

private:
	BoundingSphere* _pSphere_Original = nullptr;
	BoundingSphere* _pSphere = nullptr;

public:
	static Bounding_Sphere* Create(BOUNDING_DESC* pDesc);
	virtual void Free() override;

};

END


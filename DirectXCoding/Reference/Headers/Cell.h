#pragma once

#include "Base.h"

BEGIN(Engine)

class Cell final : public Base
{
public:
	enum POINTS { POINT_A, POINT_B, POINT_C, POINT_END,};
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };

private:
	explicit Cell(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~Cell() = default;

public:
	HRESULT Initialize(const Vec3* pPoints, uint32 iIndex);

private:
	ID3D11Device* _device = nullptr;
	ID3D11DeviceContext* _deviceContext = nullptr;

private:
	uint32 _iIndex = {};
	Vec3 _vPoints[POINT_END];
	Vec3 _vNormals[LINE_END];

public:
	static Cell* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const Vec3* pPoints, uint32 iIndex);
	virtual void Free() override;
};

END


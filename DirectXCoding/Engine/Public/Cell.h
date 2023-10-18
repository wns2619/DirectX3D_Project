#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL Cell final : public Base
{
public:
	enum POINTS { POINT_A, POINT_B, POINT_C, POINT_END,};
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };

private:
	explicit Cell(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~Cell() = default;

public:
	const Vec3* GetPoint(POINTS ePoint) const { return &_vPoints_Original[ePoint]; }
	void SetUp_Neighbor(LINE eLine, Cell* pCell) { _neightborIndices[eLine] = pCell->_iIndex; }

public:
	HRESULT Initialize(const Vec3* pPoints, uint32 iIndex);
	void Update(FXMMATRIX worldMatrix);
	_bool ComparePoints(const Vec3* pSourcePoint, const Vec3* pDestPoint);
	_bool IsOut(FXMVECTOR vPoint, FXMMATRIX worldMatrix, int32* pNeighborIndex);
	_bool IsSilde(XMVECTOR& vPoint, FXMVECTOR vLook, FXMMATRIX worldMatrix);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif // !_DEBUG


private:
	ID3D11Device* _device = nullptr;
	ID3D11DeviceContext* _deviceContext = nullptr;
#ifdef _DEBUG
private:
	class VIBufferCell* _viBuffer = nullptr;
#endif // _DEBUG


private:
	uint32 _iIndex = {};
	Vec3 _vPoints_Original[POINT_END];
	// 정점의 원래 위치.

	Vec3 _vPoints_InWorld[POINT_END];
	// 월드로 변환한 정점의 위치.
	Vec3 _vNormals[LINE_END];

	int32 _neightborIndices[LINE_END] = { -1,-1,-1 };

public:
	static Cell* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const Vec3* pPoints, uint32 iIndex);
	virtual void Free() override;
};

END


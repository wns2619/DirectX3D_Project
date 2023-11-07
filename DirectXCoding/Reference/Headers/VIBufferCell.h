#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL VIBufferCell final : public VIBuffer
{
private:
	explicit VIBufferCell(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit VIBufferCell(const VIBufferCell& rhs);
	virtual ~VIBufferCell() = default;

public:
	virtual HRESULT InitializePrototype(const Vec3* pPoints);
	virtual HRESULT Initialize(void* pArg) override;

public:
	XMVECTOR SetUp_OnCell(class Transform* cellTransform, FXMVECTOR vWorldPos, _bool bCorrection);

public:
	static VIBufferCell* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const Vec3* pPoints);
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};


END


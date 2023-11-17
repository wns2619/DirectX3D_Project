#pragma once

#include "VIBufferInstancing.h"

BEGIN(Engine)

class ENGINE_DLL VIBuffer_RectInstance final : public VIBufferInstancing
{
private:
	explicit VIBuffer_RectInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit VIBuffer_RectInstance(const VIBuffer_RectInstance& rhs);
	virtual ~VIBuffer_RectInstance() = default;

public:
	virtual HRESULT	InitializePrototype(uint32 iNumInstance);
	virtual HRESULT	Initialize(void* argument)  override;
	virtual HRESULT Update(_float fTimeDelta)   override;

public:
	static VIBuffer_RectInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, uint32 iNumInstance);
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};

END


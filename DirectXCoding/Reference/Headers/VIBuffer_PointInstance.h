#pragma once

#include "VIBufferInstancing.h"

BEGIN(Engine)

class ENGINE_DLL VIBuffer_PointInstance final : public VIBufferInstancing
{
private:
	explicit VIBuffer_PointInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit VIBuffer_PointInstance(const VIBuffer_PointInstance& rhs);
	virtual ~VIBuffer_PointInstance() = default;

public:
	virtual HRESULT InitializePrototype(const INSTANCE_DESC& InstanceDesc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Update(_float timeDelta) override;

public:
	static VIBuffer_PointInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const INSTANCE_DESC& InstanceDesc);
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};

END


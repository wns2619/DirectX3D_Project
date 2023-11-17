#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL VIBufferInstancing abstract : public VIBuffer
{
protected:
	explicit VIBufferInstancing(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit VIBufferInstancing(const VIBufferInstancing& rhs);
	virtual ~VIBufferInstancing() = default;

public:
	virtual HRESULT	InitializePrototype()		override;
	virtual HRESULT	Initialize(void* argument)  override;
	virtual HRESULT Update(_float fTimeDelta) { return S_OK; }
	virtual HRESULT	Render();

protected:
	uint32 _iStrideInstance = 0;
	uint32 _iNumInstance = 0;
	uint32 _iNumIndicesPerInstance = 0;
	ID3D11Buffer* _pVBInstance = nullptr;

public:
	virtual Component* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END


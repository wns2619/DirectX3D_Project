#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL VIBufferInstancing abstract : public VIBuffer
{
public:
	struct INSTANCE_DESC
	{
		Vec3 vCenter;
		Vec3 vRange;
		_float fSpeedMin = 0.f, fSpeedMax = 0.f;
		_float fScaleMin, fScaleMax;
		_float fLifeTimeMin, fLifeTimeMax;
		uint32 iNumInstance = 0;
	};
protected:
	explicit VIBufferInstancing(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit VIBufferInstancing(const VIBufferInstancing& rhs);
	virtual ~VIBufferInstancing() = default;

public:
	virtual HRESULT	InitializePrototype(const INSTANCE_DESC& InstanceDesc);
	virtual HRESULT	Initialize(void* argument)  override;
	virtual HRESULT Update(_float fTimeDelta) { return S_OK; }
	virtual HRESULT	Render();

protected:
	uint32 _iStrideInstance = 0;
	uint32 _iNumInstance = 0;
	uint32 _iNumIndicesPerInstance = 0;
	ID3D11Buffer* _pVBInstance = nullptr;

	VTXINSTANCE* _pInstanceVertices = nullptr;

	_float* _pSpeeds = nullptr;
	_float* _pLifeTimes = nullptr;
	_float* _pTimeAccs = nullptr;

public:
	virtual Component* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END


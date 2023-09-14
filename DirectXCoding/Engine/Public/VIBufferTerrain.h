#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL VIBufferTerrain final : public VIBuffer
{
private:
	explicit VIBufferTerrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit VIBufferTerrain(const VIBufferTerrain& rhs);
	virtual ~VIBufferTerrain() = default;

public:
	virtual HRESULT InitializePrototype(const wstring& heightMapPath);
	virtual HRESULT	Initialize(void* argument) override;

private:
	_ulong _numVerticesX = 0;
	_ulong _numVerticesZ = 0;

public:
	static VIBufferTerrain* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wstring& heightMapPath);
	virtual Component* Clone(void* argument) override;
	virtual void Free() override;
};

END
#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class Transform;

class ENGINE_DLL VIBufferTerrain final : public VIBuffer
{
public:
	struct TERRAIN_DESC
	{
		_ulong numVerticesX = 0;
		_ulong numVerticesZ = 0;
	};

private:
	explicit VIBufferTerrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit VIBufferTerrain(const VIBufferTerrain& rhs);
	virtual ~VIBufferTerrain() = default;

public:
	virtual HRESULT InitializePrototype(const wstring& heightMapPath);
	virtual HRESULT	Initialize(void* argument) override;	

public:

	Vec3* GetVerticesPosition() { return _vertices; }
	TERRAIN_DESC GetTerranDesc() { return _terrainDesc; }

private:
	TERRAIN_DESC				_terrainDesc = {};
	Vec3* _vertices							 = nullptr;

public:
	static VIBufferTerrain* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wstring& heightMapPath);
	virtual Component* Clone(void* argument) override;
	virtual void Free() override;
};

END
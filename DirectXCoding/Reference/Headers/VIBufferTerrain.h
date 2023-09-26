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
	_bool Pick(int32 screenX, int32 screenY, Vec3& pickPos, _float& distance, Transform& trans, _float& winSizeX, _float& winSizeY);

private:
	_ulong _numVerticesX = 0;
	_ulong _numVerticesZ = 0;

	VertexTextureNormalData* _vertices = nullptr;

public:
	static VIBufferTerrain* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wstring& heightMapPath);
	virtual Component* Clone(void* argument) override;
	virtual void Free() override;
};

END
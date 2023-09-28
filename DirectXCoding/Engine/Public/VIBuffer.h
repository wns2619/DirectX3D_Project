#pragma once
#include "Component.h"


BEGIN(Engine)

class ENGINE_DLL VIBuffer abstract : public Component
{
protected:
	explicit VIBuffer(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit VIBuffer(const VIBuffer& rhs);
	virtual ~VIBuffer() = default;
public:
	virtual HRESULT	InitializePrototype()		override;
	virtual HRESULT	Initialize(void* argument)  override;
	virtual HRESULT	Render();


protected:
	ID3D11Buffer*				_vertexBuffer		= nullptr;
	ID3D11Buffer*				_indexBuffer		= nullptr;



	D3D11_BUFFER_DESC			_bufferDesc	;
	D3D11_SUBRESOURCE_DATA		_subResourceData;
	DXGI_FORMAT					_indexFormat;
	D3D11_PRIMITIVE_TOPOLOGY	_topology;
	uint32						_stride				= 0;
	uint32						_numvertices		= 0;
	uint32						_indexStride		= 0;
	uint32						_numIndices			= 0;
	uint32						_numVBs				= 0;


protected:
	HRESULT	CreateBuffer(_Inout_ ID3D11Buffer** bufferOut);

public:
	virtual Component* Clone(void* argument) = 0;
	virtual void Free() override;
};


END


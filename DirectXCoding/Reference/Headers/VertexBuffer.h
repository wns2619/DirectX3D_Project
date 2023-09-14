#pragma once

#include "Base.h"

BEGIN(Engine)

class VertexBuffer
{
private:
	explicit VertexBuffer(ID3D11Device*	device);
	virtual ~VertexBuffer();

public:
	template<typename T>
	void Create(const vector<T> vertices)
	{
		_stride = sizeof(T);
		_count = static_cast<uint32>(vertices.size());
		
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth = (uint32)(_stride * _count);

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = vertices.data();
		
		HRESULT hr = _device->CreateBuffer(&desc, &data, _vertexBuffer);
		FAILED(hr);
	}

private:
	ID3D11Device* _device = nullptr;
	ID3D11Buffer* _vertexBuffer = nullptr;

	uint32 _stride = 0;
	uint32 _offset = 0;
	uint32 _count = 0;
};

END
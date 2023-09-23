#pragma once
#include "Component.h"

BEGIN(Engine)

template<typename T>
class ENGINE_DLL ConstantBuffer final : Component
{
private:
	explicit ConstantBuffer(ID3D11Device* device, ID3D11DeviceContext* deviceConetxt)
		: Component(device, deviceConetxt)
	{

	}
	explicit ConstantBuffer(const ConstantBuffer& rhs) : _constantBuffer(rhs._constantBuffer) {}
	virtual ~ConstantBuffer() = default;

public:
	virtual HRESULT	InitializePrototype()		override
	{
		D3D11_BUFFER_DESC desc;
		::ZeroMemory(&desc, sizeof(desc));
		{
			desc.Usage = D3D11_USAGE_DYNAMIC;
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = sizeof(T);
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}

		if (FAILED(_device->CreateBuffer(&desc, nullptr, &_constantBuffer)))
			return E_FAIL;

		return S_OK;

	}

	virtual HRESULT Initialize(void* argument)	override
	{

		return S_OK;
	}

private:
	ID3D11Buffer* _constantBuffer		= nullptr;

public:
	static ConstantBuffer* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		ConstantBuffer* constantBuffer = new ConstantBuffer(device, deviceContext);

		if (FAILED(constantBuffer->InitializePrototype()))
		{
			MSG_BOX("Create Failed ConstantBuffer : Prototype");
			Safe_Release<ConstantBuffer*>(constantBuffer);
			return nullptr;
		}

		return constantBuffer;
	}
	virtual Component* Clone(void* argument) override
	{
		ConstantBuffer* constantBuffer = new ConstantBuffer(*this);

		if (FAILED(constantBuffer->Initialize(argument)))
		{
			MSG_BOX("Create Failed ConstantBuffer : Prototype");
			Safe_Release<ConstantBuffer*>(constantBuffer);
			return nullptr;
		}

		return constantBuffer;
	}
	virtual void Free() override
	{
		__super::Free();
	}
};

END


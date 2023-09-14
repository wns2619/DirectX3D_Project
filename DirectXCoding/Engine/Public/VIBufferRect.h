#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL VIBufferRect final : public VIBuffer
{
private:
	explicit VIBufferRect(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit VIBufferRect(const VIBufferRect& rhs);
	virtual ~VIBufferRect() = default;

public:
	virtual HRESULT	InitializePrototype()		override;
	virtual HRESULT Initialize(void* argument)	override;

public:
	static VIBufferRect* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual Component* Clone(void* argument) override;
	virtual void Free() override;
};

END


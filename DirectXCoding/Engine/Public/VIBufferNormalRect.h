#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL VIBufferNormalRect final : public VIBuffer
{
private:
	explicit VIBufferNormalRect(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit VIBufferNormalRect(const VIBufferNormalRect& rhs);
	virtual ~VIBufferNormalRect() = default;

public:
	virtual HRESULT	InitializePrototype()		override;
	virtual HRESULT Initialize(void* argument)	override;

public:
	static VIBufferNormalRect* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual Component* Clone(void* argument) override;
	virtual void Free() override;
};

END


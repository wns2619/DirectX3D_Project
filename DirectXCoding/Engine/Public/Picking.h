#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL Picking : public Component
{
private:
	explicit Picking(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Picking(const Picking& rhs);
	virtual ~Picking() = default;

public:
	void Pick(int32 screenX, int32 screenY, _float winsizeX, _float winSizeY);
	
public:
	static Picking* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual Component* Clone(void* argument) override;
	virtual void Free() override;
};


END


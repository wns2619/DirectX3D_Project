#pragma once

#include "Base.h"

BEGIN(Engine)

class OtherLight final : public Base
{
private:
	OtherLight();
	virtual ~OtherLight() = default;

public:
	LIGHT_DESC* GetLightDesc() { return &_lightDesc; }
public:
	HRESULT	Initialize(const LIGHT_DESC& lightdesc);

private:
	LIGHT_DESC _lightDesc;

public:
	static OtherLight* Create(const LIGHT_DESC& LightDesc);
	virtual void Free() override;
};

END

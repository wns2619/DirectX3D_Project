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
	
	void SetLightNumber(uint32 num) { _ownLightNumber = num; }
	uint32 GetLightNumber() { return _ownLightNumber; }
	void SetLightName(const string& name) { _strlightName = name; }
	const string& GetLightName() { return _strlightName; }

private:
	LIGHT_DESC _lightDesc;
	string _strlightName = "";

	uint32 _ownLightNumber = 0;
public:
	static OtherLight* Create(const LIGHT_DESC& LightDesc);
	virtual void Free() override;
};

END

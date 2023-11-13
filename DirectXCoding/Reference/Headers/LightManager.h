#pragma once

#include "Base.h"
#include "OtherLight.h"

BEGIN(Engine)

class LightManager final : public Base
{
	DECLARE_SINGLETON(LightManager)

private:
	LightManager();
	virtual ~LightManager() = default;

public:
	LIGHT_DESC* getLightDesc(uint32 lightIndex);
	vector<OtherLight*>* getLightList() { return &_lights; }

	uint32 GetLightCount() { return _iLightCount; }
	void SetLightCount(uint32 lightCount) { _iLightCount = lightCount; }
	void GetLighsNumber();

public:
	HRESULT Render(class Shader* pShader, class VIBufferRect* pVIBuffer);

public:
	HRESULT	AddLight(const LIGHT_DESC& lightDesc);
	HRESULT DeleteLight(uint32 lightIndex, const string& lightName);
	OtherLight* FindLightFromID(const uint32 id);
	HRESULT SelectTurnOffLight(uint32* pLightArray, uint32 arraySize);
	HRESULT	AllTurnOnLight();

private:
	vector<OtherLight*> _lights;
	uint32 _iLightCount = 0;

public:
	virtual void Free() override;
};

END


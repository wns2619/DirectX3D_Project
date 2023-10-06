#pragma once

#include "Base.h"
#include "OtherLight.h"

BEGIN(Engine)

//class LightManager final : public Base
//{
//	DECLARE_SINGLETON(LightManager)
//public:
//
//private:
//	LightManager();
//	virtual ~LightManager() = default;
//
//public:
//	void Tick(const _float timeDelta);
//	void LateTick(const _float timeDelta);
//	void Clear(uint32 levelIndex);
//
//	HRESULT	ReserveManager(uint32 numLevels);
//	HRESULT AddLightProtoType(uint32 levelIndex, Light::LightType type, const wstring& lighttag, Component* prototype);
//	Component* CloneLight(uint32 levelIndex, Light::LightType type, const wstring& lighttag, void* argument);
//
//private:
//	uint32 _numLevels = 0;
//	typedef map<const wstring, Component*> MAPLIGHT;
//	MAPLIGHT* _directionalLight = nullptr;
//	MAPLIGHT* _pointLight		= nullptr;
//	MAPLIGHT* _spotLight		= nullptr;
//
//private:
//	Component* FindProtoType(uint32 levelIndex, Light::LightType type, const wstring& lighttag);
//
//public:
//	virtual void Free() override;
//};


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
	HRESULT	AddLight(const LIGHT_DESC& lightDesc);
	HRESULT DeleteLight(uint32 lightIndex, const string& lightName);

private:
	vector<OtherLight*> _lights;
	uint32 _iLightCount = 0;

public:
	virtual void Free() override;
};

END


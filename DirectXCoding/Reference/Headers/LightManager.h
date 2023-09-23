#pragma once

#include "Base.h"
#include "Light.h"

BEGIN(Engine)

class Light;

class LightManager final : public Base
{
	DECLARE_SINGLETON(LightManager)
public:

private:
	LightManager();
	virtual ~LightManager() = default;

public:
	void Tick(const _float timeDelta);
	void LateTick(const _float timeDelta);
	void Clear(uint32 levelIndex);

	HRESULT	ReserveManager(uint32 numLevels);
	HRESULT AddLightProtoType(uint32 levelIndex, Light::LightType type, const wstring& lighttag, Component* prototype);
	Component* CloneLight(uint32 levelIndex, Light::LightType type, const wstring& lighttag, void* argument);

private:
	uint32 _numLevels = 0;
	typedef map<const wstring, Component*> MAPLIGHT;
	MAPLIGHT* _directionalLight = nullptr;
	MAPLIGHT* _pointLight		= nullptr;
	MAPLIGHT* _spotLight		= nullptr;

private:
	Component* FindProtoType(uint32 levelIndex, Light::LightType type, const wstring& lighttag);

public:
	virtual void Free() override;
};

END


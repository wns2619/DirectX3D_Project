#pragma once

#include "Renderer.h"
#include "Shader.h"
#include "VIBufferRect.h"
#include "Texture.h"
#include "Transform.h"

BEGIN(Engine)

class Component;

class ComponentManager : public Base
{
	DECLARE_SINGLETON(ComponentManager)

private:
	ComponentManager();
	virtual ~ComponentManager() = default;

public:
	HRESULT	ReserveManager(uint32 numLevels);
	HRESULT	AddProtoType(uint32 levelIndex, const wstring& protoTypeTag, class Component* protoType);
	Component* CloneComponent(uint32 levelIndex, const wstring& protoTypeTag, void* argument);

private:
	uint32 _numLevels = 0;
	map<const wstring, Component*>* _protoTypes = nullptr;
	typedef map<const wstring, Component*> PROTOTYPES;

private:
	Component* FindProtoType(uint32 levelIndex, const wstring& protoTypeTag);

public:
	virtual void Free() override;
};

END

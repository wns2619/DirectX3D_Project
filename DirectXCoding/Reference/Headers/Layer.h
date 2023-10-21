#pragma once

#include "Base.h"

BEGIN(Engine)

class GameObject;
class Component;

class ENGINE_DLL Layer final : public Base
{
private:
	Layer();
	virtual ~Layer() = default;

public:
	HRESULT	Initialize();
	HRESULT	AddGameObject(GameObject* gameObject);
	void PriorityTick(const _float& timeDelta);
	void Tick(const _float& timeDelta);
	void LateTick(const _float& timeDelta);

public:
	HRESULT DeleteLayerObject(uint32 objectNumber, const string& modelName);
	vector<GameObject*> GetCurrenLayer() { return _gameObjects; }

public:
	vector<GameObject*>* GetGameObject() { return &_gameObjects; }
	Component* GetComponent(const wstring& strComponentTag, const string& modelName, uint32 iIndex);
public:
	void SetLayerTag(LAYER_TAG type) { _layerTag = type; }
	LAYER_TAG GetLayerTag() { return _layerTag; }
private:
	vector<GameObject*> _gameObjects;

	LAYER_TAG _layerTag;

public:
	static Layer* Create();
	virtual void Free() override;
};

END


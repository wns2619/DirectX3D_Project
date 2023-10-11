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
	void Tick(const _float& timeDelta);
	void LateTick(const _float& timeDelta);

public:
	HRESULT DeleteLayerObject(uint32 objectNumber, const string& modelName);
	vector<GameObject*> GetCurrenLayer() { return _gameObjects; }

public:
	vector<GameObject*>* GetGameObject() { return &_gameObjects; }

private:
	vector<GameObject*> _gameObjects;

public:
	static Layer* Create();
	virtual void Free() override;
};

END


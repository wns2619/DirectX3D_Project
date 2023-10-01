#pragma once

#include "Base.h"

BEGIN(Engine)

class GameObject;
class Component;

class Layer final : public Base
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
	vector<GameObject*>* GetGameObject() { return &_gameObjects; }

private:
	vector<GameObject*> _gameObjects;

public:
	static Layer* Create();
	virtual void Free() override;
};

END


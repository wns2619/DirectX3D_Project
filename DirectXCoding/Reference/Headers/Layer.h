#pragma once

#include "Base.h"

BEGIN(Engine)

class GameObject;

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

private:
	list<GameObject*> _gameObjects;

public:
	static Layer* Create();
	virtual void Free() override;
};

END


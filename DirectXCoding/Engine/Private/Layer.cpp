#include "pch.h"
#include "Layer.h"
#include "GameObject.h"

Layer::Layer()
{
}

HRESULT Layer::Initialize()
{
	return S_OK;
}

HRESULT Layer::AddGameObject(GameObject* gameObject)
{
	if (nullptr == gameObject)
		return E_FAIL;

	_gameObjects.push_back(gameObject);

	return S_OK;
}

void Layer::Tick(const _float& timeDelta)
{
	for (auto& gameiter : _gameObjects)
		if (nullptr != gameiter)
			gameiter->Tick(timeDelta);
}

void Layer::LateTick(const _float& timeDelta)
{
	for (auto& gameiter : _gameObjects)
		if (nullptr != gameiter)
			gameiter->LateTick(timeDelta);
}

Layer* Layer::Create()
{
	Layer* layer = new Layer();

	if (FAILED(layer->Initialize()))
	{
		MSG_BOX("Failed Create Layer");
		Safe_Release<Layer*>(layer);
		return nullptr;
	}

	return layer;
}

void Layer::Free()
{
	__super::Free();

	for (auto& gameiter : _gameObjects)
		Safe_Release<GameObject*>(gameiter);

	_gameObjects.clear();
}

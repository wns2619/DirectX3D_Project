#include "pch.h"
#include "ObjectManager.h"
#include "Layer.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(ObjectManager)

ObjectManager::ObjectManager()
{
}

HRESULT ObjectManager::ReserveManager(uint32 levelNumbers)
{
	if (nullptr != _Layers)
		return E_FAIL;

	_Layers = new LAYERS[levelNumbers];

	_levelNumber = levelNumbers;


	return S_OK;
}

HRESULT ObjectManager::AddPrototype(const wstring& prototypeTag, GameObject* prototype)
{
	if (nullptr != FindPrototype(prototypeTag))
		return E_FAIL;

	_protoTypes.emplace(prototypeTag, prototype);

	return S_OK;
}

HRESULT ObjectManager::AddGameObject(uint32 levelIndex, const wstring& layerTag, const wstring& prototypeTag, void* argument)
{
	GameObject* protoType = FindPrototype(prototypeTag);
	if (nullptr == protoType)
		return E_FAIL;

	GameObject* gameObject = protoType->Clone(argument);
	if (nullptr == gameObject)
		return E_FAIL;

	Layer* layer = FindLayer(levelIndex, layerTag);

	if (nullptr == layer)
	{
		layer = Layer::Create();

		layer->AddGameObject(gameObject);

		_Layers[levelIndex].emplace(layerTag, layer);
	}
	else
		layer->AddGameObject(gameObject);


	return S_OK;
}

void ObjectManager::Tick(const _float& timeDelta)
{
	for (size_t i = 0; i < _levelNumber; i++)
		for (auto& Pair : _Layers[i])
			Pair.second->Tick(timeDelta);
}

void ObjectManager::LateTick(const _float& timeDelta)
{
	for (size_t i = 0; i < _levelNumber; ++i)
		for (auto& pair : _Layers[i])
			pair.second->LateTick(timeDelta);
}

void ObjectManager::Clear(uint32 levelIndex)
{
	for (auto& pair : _Layers[levelIndex])
		Safe_Release<Layer*>(pair.second);

	_Layers[levelIndex].clear();
}

GameObject* ObjectManager::FindPrototype(const wstring& prototypeTag)
{
	auto iter = _protoTypes.find(prototypeTag);

	if (iter == _protoTypes.end())
		return nullptr;

	return iter->second;
}

Layer* ObjectManager::FindLayer(int32 levelIndex, const wstring& layerTag)
{
	if (levelIndex >= _levelNumber)
		return nullptr;

	auto iter = _Layers[levelIndex].find(layerTag);

	if (iter == _Layers[levelIndex].end())
		return nullptr;

	return iter->second;
}

void ObjectManager::Free()
{
	__super::Free();

	for (size_t i = 0; i < _levelNumber; ++i)
	{
		for (auto& pair : _Layers[i])
			Safe_Release<Layer*>(pair.second);

		_Layers[i].clear();
	}

	Safe_Delete_Array<LAYERS*>(_Layers);
	
	for (auto& iter : _protoTypes)
		Safe_Release<GameObject*>(iter.second);

	_protoTypes.clear();
}

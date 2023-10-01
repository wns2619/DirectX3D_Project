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

		_currenlevel = levelIndex;
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

GameObject* ObjectManager::GetLayerObject(const wstring& layertag, OBJECT_TYPE type)
{
	Layer* CurrentLayer = FindLayer(_currenlevel, layertag);
	if (CurrentLayer == nullptr)
		return nullptr;

	vector<GameObject*>* objectlist = CurrentLayer->GetGameObject();

	for (GameObject* gameObject : *objectlist)
	{
		if (gameObject->GetObjectType() == type)
			return gameObject;
	}

	return nullptr;
}

uint32 ObjectManager::GetLayerObjectCount()
{
	_levelObjectCount = 0;

	for (const auto& pair : _Layers[_currenlevel])
	{
		const Layer* layer = pair.second;

		if (layer)
		{
			int32 listsize = pair.second->GetGameObject()->size();
			_levelObjectCount += listsize;
		}
	}

	return _levelObjectCount;
}

vector<GameObject*>* ObjectManager::GetCurrentObjectList(wstring& layerTag)
{
	Layer* layer = FindLayer(_currenlevel, layerTag);

	if (nullptr == layer)
	{
		return nullptr;
	}

	return layer->GetGameObject();
}

HRESULT ObjectManager::ComparisonAddObject(int32 levelIndex, const string& addObjectfile, const wstring& layertag, void* argument)
{
	if (levelIndex >= _levelNumber)
		return E_FAIL;

	// ImGui에서 받은 경로를 ProtoType의 경로에 쏴준다.
	GameObject* protoType = FindPrototype(wstring(addObjectfile.begin(), addObjectfile.end()));
	if (nullptr == protoType)
		return E_FAIL;

	GameObject* gameObject = protoType->Clone(argument);
	if (nullptr == gameObject)
		return E_FAIL;

	Layer* layer = FindLayer(levelIndex, layertag);

	if (nullptr == layer)
	{
		layer = Layer::Create();

		layer->AddGameObject(gameObject);

		_Layers[levelIndex].emplace(layertag, layer);

		_currenlevel = levelIndex;
	}
	else
		layer->AddGameObject(gameObject);


	return S_OK;
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

#include "pch.h"
#include "ObjectManager.h"
#include "Layer.h"
#include "GameObject.h"
#include "Model.h"
#include "Utils.h"

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

HRESULT ObjectManager::AddGameObject(uint32 levelIndex, const LAYER_TAG layerTag, const wstring& prototypeTag, void* argument)
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
		layer->SetLayerTag(layerTag);

		_Layers[levelIndex].emplace(layerTag, layer);

		_currenlevel = levelIndex;
	}
	else
		layer->AddGameObject(gameObject);


	return S_OK;
}

void ObjectManager::PriorityTick(const _float& timeDelta)
{
	for (uint32 i = 0; i < _levelNumber; ++i)
	{
		for (auto& pair : _Layers[i])
		{

			pair.second->PriorityTick(timeDelta);
		}
	}
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

vector<GameObject*>& ObjectManager::GetLayerObject(const LAYER_TAG layertag)
{
	Layer* CurrentLayer = FindLayer(_currenlevel, layertag);

	return *CurrentLayer->GetGameObject();
}

GameObject* ObjectManager::GetLayerObjectTag(const LAYER_TAG layerag, const string& modelname)
{
	Layer* CurrentLayer = FindLayer(_currenlevel, layerag);
	if (CurrentLayer == nullptr)
		return nullptr;

	vector<GameObject*>* objectlist = CurrentLayer->GetGameObject();

	for (GameObject* gameObject : *objectlist)
	{
		if (gameObject->GetModelName() == modelname)
			return gameObject;
	}

	return nullptr;
}

uint32 ObjectManager::GetLayerObjectCount()
{
	_levelObjectCount = 0;
	int32 j = 0;
	size_t listsize = 0;

	for (const auto& pair : _Layers[_currenlevel])
	{
		const Layer* layer = pair.second;

		if (nullptr != layer)
		{
			vector<GameObject*>* gameObject = pair.second->GetGameObject();


			for (size_t i = 0; i < gameObject->size(); i++)
			{
				if ((*gameObject)[i]->GetObjectType() != OBJECT_TYPE::CAMERA)
				{
					(*gameObject)[i]->SetIdNumber(j);
					++j;
				}
			}
	


			listsize += pair.second->GetGameObject()->size();
		}
	}

	return listsize;
}

vector<GameObject*>* ObjectManager::GetCurrentObjectList(LAYER_TAG layerTag)
{
	Layer* layer = FindLayer(_currenlevel, layerTag);

	if (nullptr == layer)
	{
		return nullptr;
	}

	return layer->GetGameObject();
}

GameObject* ObjectManager::CloneGameObject(const wstring& strPrototypeTag, void* pArg)
{
	GameObject* pPrototype = FindPrototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	/* 원형을 보ㅓㄱ제하여 사본을 마느단다. */
	GameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return nullptr;

	return pGameObject;
}

Component* ObjectManager::GetComponent(uint32 iLevelIndex, const LAYER_TAG eTag, const wstring& strComponentTag, const string& modelName, uint32 iIndex)
{
	Layer* pLayer = FindLayer(iLevelIndex, eTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->GetComponent(strComponentTag, modelName, iIndex);
}

Component* ObjectManager::GetComponentFromID(uint32 iLevelIndex, const LAYER_TAG tag, const wstring& strComponentTag, const string& modelName, uint32 id)
{
	Layer* pLayer = FindLayer(iLevelIndex, tag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->GetComponentFromID(strComponentTag, modelName, id);
}

HRESULT ObjectManager::ComparisonAddObject(int32 levelIndex, const string& addObjectfile, LAYER_TAG layertag, void* argument)
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

HRESULT ObjectManager::DeleteObject(uint32 levelIndex, const LAYER_TAG layertag, const uint32 objectNumber, const string& modelnames)
{
	auto iter = FindLayer(levelIndex, layertag);
	if (nullptr == iter)
		return E_FAIL;

	return iter->DeleteLayerObject(objectNumber, modelnames);
}

GameObject* ObjectManager::FindPrototype(const wstring& prototypeTag)
{
	auto iter = _protoTypes.find(prototypeTag);

	if (iter == _protoTypes.end())
		return nullptr;

	return iter->second;
}

Layer* ObjectManager::FindLayer(int32 levelIndex, LAYER_TAG layerTag)
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

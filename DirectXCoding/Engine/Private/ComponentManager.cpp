#include "pch.h"
#include "ComponentManager.h"

IMPLEMENT_SINGLETON(ComponentManager)

ComponentManager::ComponentManager()
{

}

HRESULT ComponentManager::ReserveManager(uint32 numLevels)
{
	if (nullptr != _protoTypes)
		return E_FAIL;

	_numLevels = numLevels;

	_protoTypes = new PROTOTYPES[numLevels];

	return S_OK;
}

HRESULT ComponentManager::AddProtoType(uint32 levelIndex, const wstring& protoTypeTag, Component* protoType)
{
	if (nullptr == _protoTypes || nullptr != FindProtoType(levelIndex, protoTypeTag))
		return E_FAIL;

	_protoTypes[levelIndex].emplace(protoTypeTag,protoType);

	return S_OK;
}

Component* ComponentManager::CloneComponent(uint32 levelIndex, const wstring& protoTypeTag, void* argument)
{
	Component* protoType = FindProtoType(levelIndex, protoTypeTag);

	if (nullptr == protoType)
		return nullptr;

	Component* component = protoType->Clone(argument);

	if (nullptr == component)
		return nullptr;

	return component;
}

Component* ComponentManager::FindProtoType(uint32 levelIndex, const wstring& protoTypeTag)
{
	if (levelIndex >= _numLevels)
		return nullptr;

	auto iter = _protoTypes[levelIndex].find(protoTypeTag);

	if (iter == _protoTypes[levelIndex].end())
		return nullptr;


	return iter->second;
}

void ComponentManager::Free()
{
	__super::Free();

	for (size_t i = 0; i < _numLevels; ++i)
	{
		for (auto& pair : _protoTypes[i])
			Safe_Release<Component*>(pair.second);

		_protoTypes[i].clear();
	}

	Safe_Delete_Array(_protoTypes);
}

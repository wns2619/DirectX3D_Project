#include "pch.h"
#include "LightManager.h"
#include "OtherLight.h"

IMPLEMENT_SINGLETON(LightManager)

//LightManager::LightManager()
//{
//}
//
//void LightManager::Tick(const _float timeDelta)
//{
//}
//
//void LightManager::LateTick(const _float timeDelta)
//{
//}
//
//void LightManager::Clear(uint32 levelIndex)
//{
//	for (auto& pair : _directionalLight[levelIndex])
//		Safe_Release<Component*>(pair.second);
//
//	for (auto& pair : _pointLight[levelIndex])
//		Safe_Release<Component*>(pair.second);
//
//	for (auto& pair : _spotLight[levelIndex])
//		Safe_Release<Component*>(pair.second);
//
//
//	_directionalLight[levelIndex].clear();
//	_pointLight[levelIndex].clear();
//	_spotLight[levelIndex].clear();
//}
//
//HRESULT LightManager::ReserveManager(uint32 numLevels)
//{
//	if (nullptr != _directionalLight)
//		return E_FAIL;
//
//	if (nullptr != _pointLight)
//		return E_FAIL;
//
//	if (nullptr != _spotLight)
//		return E_FAIL;
//
//	_numLevels = numLevels;
//
//	_directionalLight = new MAPLIGHT[numLevels];
//	_pointLight = new MAPLIGHT[numLevels];
//	_spotLight = new MAPLIGHT[numLevels];
//
//
//	return S_OK;
//}
//
//HRESULT LightManager::AddLightProtoType(uint32 levelIndex, Light::LightType type, const wstring& lighttag, Component* prototype)
//{
//
// 	if (Light::LightType::DIRECTIONAL == type)
//	{
//		if (nullptr == _directionalLight || nullptr != FindProtoType(levelIndex, type, lighttag))
//			return E_FAIL;
//
//		_directionalLight[levelIndex].emplace(make_pair(lighttag, prototype));
//	}
//
//	if (Light::LightType::POINT == type)
//	{
//		if (nullptr == _pointLight || nullptr != FindProtoType(levelIndex, type, lighttag))
//			return E_FAIL;
//
//		_pointLight[levelIndex].emplace(make_pair(lighttag, prototype));
//	}
//
//	if (Light::LightType::SPOT == type)
//	{
//		if (nullptr == _spotLight || nullptr != FindProtoType(levelIndex, type, lighttag))
//			return E_FAIL;
//
//		_spotLight[levelIndex].emplace(make_pair(lighttag, prototype));
//	}
//
//	return S_OK;
//}
//
//Component* LightManager::CloneLight(uint32 levelIndex, Light::LightType type, const wstring& lighttag, void* argument)
//{
//	if (levelIndex >= _numLevels || Light::LightType::AMBIENT > type || Light::LightType::LIGHT_END <= type)
//		return nullptr;
//
//	Component* prototype = FindProtoType(levelIndex, type, lighttag);
//
//	if (nullptr == prototype)
//		return nullptr;
//
//	Component* component = prototype->Clone(argument);
//
//	if (nullptr == component)
//		return nullptr;
//
//	return component;
//}

//Component* LightManager::FindProtoType(uint32 levelIndex, Light::LightType type, const wstring& lighttag)
//{
//	if (levelIndex >= _numLevels || Light::LightType::AMBIENT > type || Light::LightType::LIGHT_END <= type)
//		return nullptr;
//
//
//	switch (type)
//	{
//		case Light::LightType::DIRECTIONAL:
//		{
//			auto iter = _directionalLight[levelIndex].find(lighttag);
//
//			if (iter == _directionalLight[levelIndex].end())
//				return nullptr;
//
//			return iter->second;
//		}
//
//			
//		case Light::LightType::POINT:
//		{
//			auto iter = _pointLight[levelIndex].find(lighttag);
//
//			if (iter == _pointLight[levelIndex].end())
//				return nullptr;
//
//			return iter->second;
//		}
//
//		case Light::LightType::SPOT:
//		{
//			auto iter = _spotLight[levelIndex].find(lighttag);
//
//			if (iter == _spotLight[levelIndex].end())
//				return nullptr;
//
//			return iter->second;
//		}
//
//	}
//
//
//	return nullptr;
//}
//
//void LightManager::Free()
//{
//	__super::Free();
//
//	for (size_t i = 0; i < _numLevels; i++)
//	{
//		for (auto& pair : _directionalLight[i])
//			Safe_Release<Component*>(pair.second);
//
//		for (auto& pair : _pointLight[i])
//			Safe_Release<Component*>(pair.second);
//
//		for (auto& pair : _spotLight[i])
//			Safe_Release<Component*>(pair.second);
//
//	}
//
//	Safe_Delete_Array<MAPLIGHT*>(_directionalLight);
//	Safe_Delete_Array<MAPLIGHT*>(_pointLight);
//	Safe_Delete_Array<MAPLIGHT*>(_spotLight);
//}

LightManager::LightManager()
{
}

LIGHT_DESC* LightManager::getLightDesc(uint32 lightIndex)
{
	if (lightIndex >= _lights.size())
		return nullptr;

	auto iter = _lights.begin();

	for (size_t i = 0; i < lightIndex; i++)
		++iter;

	return (*iter)->GetLightDesc();
}

HRESULT LightManager::AddLight(const LIGHT_DESC& lightDesc)
{
	OtherLight* light = OtherLight::Create(lightDesc);

	if (nullptr == light)
		return E_FAIL;

	_lights.push_back(light);

	return S_OK;
}

void LightManager::Free()
{
	for (auto& light : _lights)
		Safe_Release<OtherLight*>(light);

	_lights.clear();
}

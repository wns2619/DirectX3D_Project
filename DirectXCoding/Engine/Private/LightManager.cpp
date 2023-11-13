#include "pch.h"
#include "LightManager.h"
#include "OtherLight.h"

IMPLEMENT_SINGLETON(LightManager)

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

void LightManager::GetLighsNumber()
{
	int32 i = 0;

	for (const auto& pair : _lights)
	{
		pair->SetLightNumber(i);
		++i;
	}
}

HRESULT LightManager::Render(Shader* pShader, VIBufferRect* pVIBuffer)
{
	for (auto& pLight : _lights)
	{
		if(false == pLight->GetLightDesc()->bEnable)
			pLight->Render(pShader, pVIBuffer);
	}

	return S_OK;
}

HRESULT LightManager::AddLight(const LIGHT_DESC& lightDesc)
{
	OtherLight* light = OtherLight::Create(lightDesc);

	if (nullptr == light)
		return E_FAIL;
	
	light->SetLightNumber(_lights.size());
	_lights.push_back(light);
	

	return S_OK;
}

HRESULT LightManager::DeleteLight(uint32 lightIndex, const string& lightName)
{
	if (lightIndex > _lights.size())
		return E_FAIL;

	for (auto lightiter = _lights.begin(); lightiter != _lights.end(); ++lightiter)
	{
		uint32 lightNum = (*lightiter)->GetLightNumber();

		if (lightNum == lightIndex && (*lightiter)->GetLightName() == lightName)
		{
			Safe_Release<OtherLight*>(*lightiter);
			_lights.erase(lightiter);

			uint32 reNumber = 0;

			for (auto& lightReNumber : _lights)
			{
				if (nullptr != lightReNumber && lightName == lightReNumber->GetLightName())
				{
					lightReNumber->SetLightNumber(reNumber);
					++reNumber;
				}
			}

			break;
		}

	}

	return S_OK;;
}

OtherLight* LightManager::FindLightFromID(const uint32 id)
{
	if (id > _lights.size())
		return nullptr;

	auto iter = find_if(_lights.begin(), _lights.end(), [&](OtherLight* pLight) {
		if (id == pLight->GetLightNumber())
			return true;

		return false;
		});

	if (iter == _lights.end())
		return nullptr;

	return *iter;
}

HRESULT LightManager::SelectTurnOffLight(uint32* pLightArray, uint32 arraySize)
{
	for (auto& pLight : _lights)
	{
		// 배열의 사이즈도 받아서 배열의 사이즈만큼 라이트를 전부 순회해서 비교. 해당하는게 있으면 true. 
		for (uint32 i = 0; i < arraySize; ++i)
		{
			if (pLight->GetLightNumber() == pLightArray[i])
				pLight->GetLightDesc()->bEnable = true;
		}
	}

	return S_OK;
}

HRESULT LightManager::AllTurnOnLight()
{
	for (auto& pLight : _lights)
	{
		if (pLight->GetLightDesc()->PlayerLight != true)
			pLight->GetLightDesc()->bEnable = false;
	}

	return S_OK;
}

void LightManager::Free()
{
	for (auto& light : _lights)
		Safe_Release<OtherLight*>(light);

	_lights.clear();
}

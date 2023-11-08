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
		pLight->Render(pShader, pVIBuffer);

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

void LightManager::Free()
{
	for (auto& light : _lights)
		Safe_Release<OtherLight*>(light);

	_lights.clear();
}

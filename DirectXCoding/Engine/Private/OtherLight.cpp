#include "pch.h"
#include "OtherLight.h"

OtherLight::OtherLight()
{
}

HRESULT OtherLight::Initialize(const LIGHT_DESC& lightdesc)
{
	::memmove(&_lightDesc, &lightdesc, sizeof(lightdesc));

	if (_lightDesc.type == LIGHT_DESC::TYPE::DIRECTION)
		_strlightName = "Direction";
	else if (_lightDesc.type == LIGHT_DESC::TYPE::POINT)
		_strlightName = "Point";
	else
		_strlightName = "Spot";

	return S_OK;
}

OtherLight* OtherLight::Create(const LIGHT_DESC& LightDesc)
{
	OtherLight* light = new OtherLight;
	
	if (FAILED(light->Initialize(LightDesc)))
	{
		MSG_BOX("Failed to Created : Light");
		Safe_Release<OtherLight*>(light);
	}

	return light;
}

void OtherLight::Free()
{
	__super::Free();
}

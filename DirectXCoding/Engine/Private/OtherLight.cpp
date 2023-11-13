#include "pch.h"
#include "OtherLight.h"
#include "VIBufferRect.h"
#include "Shader.h"
#include "TimeManager.h"

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

HRESULT OtherLight::Render(Shader* pShader, VIBufferRect* pVIBuffer)
{
	uint32 iPassIndex = 0;

	if (true == _lightDesc.bEnable)
		return S_OK;

	if (LIGHT_DESC::DIRECTION == _lightDesc.type)
	{
		if (FAILED(pShader->BindRawValue("DirToLight", &_lightDesc.Direction, sizeof(Vec4))))
			return E_FAIL;
		
		if (FAILED(pShader->BindRawValue("DirLightColor", &_lightDesc.Diffuse, sizeof(Vec4))))
			return E_FAIL;

		iPassIndex = 1;
	}

	else if (LIGHT_DESC::POINT == _lightDesc.type)
	{

		if (FAILED(pShader->BindRawValue("PointLightColor", &_lightDesc.Diffuse, sizeof(Vec4))))
			return E_FAIL;
		if (FAILED(pShader->BindRawValue("PointLightPosition", &_lightDesc.Position, sizeof(Vec4))))
			return E_FAIL;


		
		if (_ownLightNumber == 5)
		{
			TimeManager* pTimeInstance = GET_INSTANCE(TimeManager);

			_fBlinkTime += pTimeInstance->Compute_TimeDelta(L"Timer_60") * 50.f;

			_float fRange = _fMinBlinkRange + 0.5f * (_fMaxBlinkRange - _fMinBlinkRange) * (1.f + sinf(_fBlinkSpeed * _fBlinkTime));

			_lightDesc.pointLightRangeRcp = 1.f / fRange;

			if (FAILED(pShader->BindRawValue("PointLightRangeRcp", &_lightDesc.pointLightRangeRcp, sizeof(_float))))
				return E_FAIL;

			RELEASE_INSTANCE(TimeManager);
		}
		else
			if(FAILED(pShader->BindRawValue("PointLightRangeRcp", &_lightDesc.pointLightRangeRcp, sizeof(_float))))
				return E_FAIL;

		iPassIndex = 2;
	}

	else if (LIGHT_DESC::SPOT == _lightDesc.type)
	{
		if (FAILED(pShader->BindRawValue("SpotLightColor", &_lightDesc.Diffuse, sizeof(Vec4))))
			return E_FAIL;
		if (FAILED(pShader->BindRawValue("SpotLightPos", &_lightDesc.Position, sizeof(Vec4))))
			return E_FAIL;
		if (FAILED(pShader->BindRawValue("SpotDirToLight", &_lightDesc.Direction, sizeof(Vec3))))
			return E_FAIL;
		if (FAILED(pShader->BindRawValue("SpotLightRangeRcp", &_lightDesc.fSpotLightRangeRcp, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pShader->BindRawValue("SpotCosOuterCone", &_lightDesc.fSpotCosOuterCone, sizeof(_float))))
			return E_FAIL;
		if (FAILED(pShader->BindRawValue("SpotCosInnerConeRcp", &_lightDesc.fSpotInnerConeRcp, sizeof(_float))))
			return E_FAIL;

		iPassIndex = 3;
	}


	if (FAILED(pShader->BindRawValue("ambientDown", &_lightDesc.vAmbientLowerColor, sizeof(Vec4))))
		return E_FAIL;

	if (FAILED(pShader->BindRawValue("ambientUp", &_lightDesc.vAmbientUpperColor, sizeof(Vec4))))
		return E_FAIL;

	if (FAILED(pShader->BindRawValue("specExp", &_lightDesc.fSpecExp, sizeof(_float))))
		return E_FAIL;

	if (FAILED(pShader->BindRawValue("specIntensity", &_lightDesc.fSpecIntensity, sizeof(_float))))
		return E_FAIL;


	if (FAILED(pShader->Begin(iPassIndex)))
		return E_FAIL;

	if (FAILED(pVIBuffer->Render()))
		return E_FAIL;


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

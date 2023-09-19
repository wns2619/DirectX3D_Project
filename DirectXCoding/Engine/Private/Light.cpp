#include "pch.h"
#include "Light.h"
#include "Shader.h"

Light::Light(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Component(device, deviceContext)
{
	::ZeroMemory(&_lightGroup, sizeof(_lightGroup));
}

Light::Light(const Light& rhs)
	: Component(rhs)
	, _lightGroup(rhs._lightGroup)
{
	
}

HRESULT Light::InitializePrototype()
{
	return S_OK;
}

HRESULT Light::Initialize(void* argument)
{ 
	if (nullptr != argument)
	{
		LightGroup* group = static_cast<LightGroup*>(argument);
		/* Directional Light */
		_lightGroup.dirLight.Ambient = group->dirLight.Ambient;
		_lightGroup.dirLight.Diffuse = group->dirLight.Diffuse;
		_lightGroup.dirLight.Direction = group->dirLight.Direction;
		_lightGroup.dirLight.Specular = group->dirLight.Specular;

		/* Point Light */
		_lightGroup.pointLight.Ambient = group->pointLight.Ambient;
		_lightGroup.pointLight.Attenuation = group->pointLight.Attenuation;
		_lightGroup.pointLight.Diffuse = group->pointLight.Diffuse;
		_lightGroup.pointLight.Position = group->pointLight.Position;
		_lightGroup.pointLight.Range = group->pointLight.Range;
		_lightGroup.pointLight.Specular = group->pointLight.Specular;

		/* Spot Light */
		_lightGroup.spotLight.Ambient = group->spotLight.Ambient;
		_lightGroup.spotLight.Attenuation = group->spotLight.Attenuation;
		_lightGroup.spotLight.Diffuse = group->spotLight.Diffuse;
		_lightGroup.spotLight.Direction = group->spotLight.Direction;
		_lightGroup.spotLight.Position = group->spotLight.Position;
		_lightGroup.spotLight.Range = group->spotLight.Range;
		_lightGroup.spotLight.Specular = group->spotLight.Specular;
		_lightGroup.spotLight.Spot = group->spotLight.Spot;
		
	}
	return S_OK;
}

Light* Light::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Light* light = new Light(device, deviceContext);

	if (FAILED(light->InitializePrototype()))
	{
		MSG_BOX("Light Created Failed");
		Safe_Release<Light*>(light);
		return nullptr;
	}

	return light;
}

Component* Light::Clone(void* argument)
{
	Light* light = new Light(*this);

	if (FAILED(light->Initialize(argument)))
	{
		MSG_BOX("Light Created Clond Failed");
		Safe_Release<Light*>(light);
		return nullptr;
	}

	return light;
}

void Light::Free()
{
	__super::Free();
}

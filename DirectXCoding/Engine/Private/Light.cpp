#include "pch.h"
#include "Light.h"
#include "Shader.h"

Light::Light(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Component(device, deviceContext, COMPONENT_TYPE::LIGHT)
{
	
}

Light::Light(const Light& rhs)
	: Component(rhs), _dirLight(rhs._dirLight)
{
}

HRESULT Light::InitializePrototype()
{

	return S_OK;
}

HRESULT Light::Initialize(void* argument)
{
	if (argument != nullptr)
	{
		DirectinoalLight* light = static_cast<DirectinoalLight*>(argument);

		_dirLight.Ambient = light->Ambient;
		_dirLight.Diffuse = light->Diffuse;
		_dirLight.Direction = light->Direction;
		_dirLight.Specular = light->Specular;
		_dirLight.Pad = light->Pad;
	}

	return S_OK;
}

void Light::FowardSetup(ID3D11Buffer* constantbuffer, LightType type)
{
	//ID3D11DepthStencilState* lightDepthStencilState = nullptr;
	//_deviceContext->OMSetDepthStencilState(lightDepthStencilState, 0);

	//D3D11_MAPPED_SUBRESOURCE MappedResource;
	//if (FAILED(_deviceContext->Map(constantbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource)))
	//{
	//	CB_DIRECTIONAL* directionalValues = static_cast<CB_DIRECTIONAL*>(MappedResource.pData);
	//	directionalValues->ambientLower = GammaToLinear(_directionalLight.ambientLower);
	//	directionalValues->ambientRange = GammaToLinear(_directionalLight.ambientRange) - GammaToLinear(_directionalLight.ambientLower);
	//	directionalValues->directionToLight = -DirectionalDir;
	//	directionalValues->DirectionalColor = GammaToLinear(_directionalLight.DirectionalColor);
	//	_deviceContext->Unmap(constantbuffer, 0);

	//	_deviceContext->PSSetConstantBuffers(1, 1, &constantbuffer);
	//}

	//Safe_Release<ID3D11DepthStencilState*>(lightDepthStencilState);
}

HRESULT Light::BindingLightToShader(Shader* shader, const _char* constantName, LightType type, uint32 legnth, uint32 offset)
{
	if (type < LightType::AMBIENT || type >= LightType::LIGHT_END)
		return E_FAIL;

	if(LightType::DIRECTIONAL == type)
		return shader->BindRawValue(constantName, &_dirLight, legnth, offset);
}


Light* Light::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Light* light = new Light(device, deviceContext);

	if (FAILED(light->InitializePrototype()))
	{
		::MSG_BOX("Create Failed Light : prototype");
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
		::MSG_BOX("Create Filaed Light : Clone");
		Safe_Release<Light*>(light);
		return nullptr;
	}

	return light;

}

void Light::Free()
{
	__super::Free();
}

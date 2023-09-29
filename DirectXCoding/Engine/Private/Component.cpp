#include "pch.h"
#include "Component.h"

Component::Component(ID3D11Device* device, ID3D11DeviceContext* deviceContext, COMPONENT_TYPE type)
	: _device(device), _deviceContext(deviceContext), _isCloned(false), _type(type)
{
	Safe_AddRef<ID3D11Device*>(_device);
	Safe_AddRef<ID3D11DeviceContext*>(_deviceContext);
}

Component::Component(const Component& rhs)
	: _device(rhs._device), _deviceContext(rhs._deviceContext), _isCloned(true), _type(rhs._type)
{
	Safe_AddRef<ID3D11Device*>(_device);
	Safe_AddRef<ID3D11DeviceContext*>(_deviceContext);
}

HRESULT Component::InitializePrototype()
{
	return S_OK;
}

HRESULT Component::Initialize(void* argument)
{
	return S_OK;
}

void Component::Free()
{
	Safe_Release<ID3D11Device*>(_device);
	Safe_Release<ID3D11DeviceContext*>(_deviceContext);
}

#include "pch.h"
#include "GameObject.h"
#include "GameInstance.h"

GameObject::GameObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: _device(device), _deviceContext(deviceContext)
{
	Safe_AddRef<ID3D11Device*>(_device);
	Safe_AddRef<ID3D11DeviceContext*>(_deviceContext);
}

GameObject::GameObject(const GameObject& rhs)
	: _device(rhs._device), _deviceContext(rhs._deviceContext)
{
	Safe_AddRef<ID3D11Device*>(_device);
	Safe_AddRef<ID3D11DeviceContext*>(_deviceContext);
}

HRESULT GameObject::InitializePrototype()
{
	return S_OK;
}

HRESULT GameObject::Initialize(void* argument)
{
	return S_OK;
}

void GameObject::Tick(const _float& timeDelta)
{
}

void GameObject::LateTick(const _float& timeDelta)
{
}

HRESULT GameObject::Render()
{
	return S_OK;
}

HRESULT GameObject::AddComponent(uint32 levelIndex, const wstring& prototypeTag, const wstring& ComponentTag, _Inout_ Component** componentout, void* argument)
{
	if (nullptr != FindComponent(ComponentTag))
		return E_FAIL;

	GameInstance* gameInstance = GameInstance::GetInstance();
	Safe_AddRef<GameInstance*>(gameInstance);

	Component* component = gameInstance->CloneComponent(levelIndex, prototypeTag, argument);

	if (nullptr == component)
		return E_FAIL;

	_Components.emplace(prototypeTag, component);

	*componentout = component;

	Safe_AddRef<Component*>(component);

	Safe_Release<GameInstance*>(gameInstance);

	return S_OK;
}

HRESULT GameObject::AddLightComponent(uint32 levelIndex, const Light::LightType type, const wstring& ComponentTag, _Inout_ Component** componentout, void* argument)
{
	if (nullptr != FindComponent(ComponentTag))
		return E_FAIL;

	GameInstance* gameInstance = GET_INSTANCE(GameInstance);

	Component* component = gameInstance->CloneLight(levelIndex, type, ComponentTag, argument);

	if (nullptr == component)
		return E_FAIL;

	_Components.emplace(ComponentTag, component);

	*componentout = component;

	Safe_AddRef<Component*>(component);

	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

Component* GameObject::FindComponent(const wstring& componentTag)
{
	auto iter = _Components.find(componentTag);

	if (iter == _Components.end())
		return nullptr;

	return iter->second;
}

void GameObject::Free()
{
	__super::Free();

	for (auto& iter : _Components)
		Safe_Release<Component*>(iter.second);

	_Components.clear();

	Safe_Release<ID3D11Device*>(_device);
	Safe_Release<ID3D11DeviceContext*>(_deviceContext);
}

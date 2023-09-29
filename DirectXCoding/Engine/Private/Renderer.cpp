#include "pch.h"
#include "Renderer.h"
#include "GameObject.h"

Renderer::Renderer(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Component(device, deviceContext, COMPONENT_TYPE::RENDERER)
{
}

HRESULT Renderer::InitializePrototype()
{
	return S_OK;
}

HRESULT Renderer::Initialize(void* argument)
{
	return S_OK;
}

HRESULT Renderer::AddRenderGroup(RENDERGROUP renderGroup, GameObject* gameObject)
{
	if (renderGroup >= RENDERGROUP::RENDER_END || renderGroup < RENDERGROUP::PRIORITY)
		return E_FAIL;

	_renderObjects[static_cast<int32>(renderGroup)].push_back(gameObject);

	Safe_AddRef<GameObject*>(gameObject);

	return S_OK;
}

HRESULT Renderer::DrawRenderObjects()
{
	if (FAILED(RenderPriority()))
		return E_FAIL;

	if (FAILED(RenderNonLight()))
		return E_FAIL;

	if (FAILED(RenderNonBlend()))
		return E_FAIL;

	if (FAILED(RenderBlend()))
		return E_FAIL;

	if (FAILED(RenderUI()))
		return E_FAIL;


	return S_OK;
}

HRESULT Renderer::RenderPriority()
{
	for (auto& gameObjects : _renderObjects[static_cast<uint32>(RENDERGROUP::PRIORITY)])
	{
		if (nullptr != gameObjects)
			gameObjects->Render();

		Safe_Release<GameObject*>(gameObjects);
	}

	_renderObjects[static_cast<uint32>(RENDERGROUP::PRIORITY)].clear();

	return S_OK;
}

HRESULT Renderer::RenderNonLight()
{
	for (auto& gameObjects : _renderObjects[static_cast<uint32>(RENDERGROUP::NONLIGHT)])
	{
		if (nullptr != gameObjects)
			gameObjects->Render();

		Safe_Release<GameObject*>(gameObjects);
	}

	_renderObjects[static_cast<uint32>(RENDERGROUP::NONLIGHT)].clear();

	return S_OK;
}

HRESULT Renderer::RenderNonBlend()
{
	for (auto& gameObjects : _renderObjects[static_cast<uint32>(RENDERGROUP::NONBLEND)])
	{
		if (nullptr != gameObjects)
			gameObjects->Render();

		Safe_Release<GameObject*>(gameObjects);
	}

	_renderObjects[static_cast<uint32>(RENDERGROUP::NONBLEND)].clear();

	return S_OK;      
}

HRESULT Renderer::RenderBlend()
{
	for (auto& gameObjects : _renderObjects[static_cast<uint32>(RENDERGROUP::BLEND)])
	{
		if (nullptr != gameObjects)
			gameObjects->Render();

		Safe_Release<GameObject*>(gameObjects);
	}

	_renderObjects[static_cast<uint32>(RENDERGROUP::BLEND)].clear();

	return S_OK;
}

HRESULT Renderer::RenderUI()
{
	for (auto& gameObjects : _renderObjects[static_cast<uint32>(RENDERGROUP::UI)])
	{
		if (nullptr != gameObjects)
			gameObjects->Render();

		Safe_Release<GameObject*>(gameObjects);
	}

	_renderObjects[static_cast<uint32>(RENDERGROUP::UI)].clear();

	return S_OK;
}

Renderer* Renderer::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Renderer* render = new Renderer(device, deviceContext);

	if (FAILED(render->InitializePrototype()))
	{
		MSG_BOX("Failed Create Renderer");
		Safe_Release<Renderer*>(render);
		return nullptr;
	}

	return render;
}

Component* Renderer::Clone(void* argument)
{
	AddRef();

	return this;
}

void Renderer::Free()
{
	__super::Free();
}

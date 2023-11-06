#include "pch.h"
#include "Renderer.h"
#include "GameObject.h"
#include "TargetManager.h"

Renderer::Renderer(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Component(device, deviceContext, COMPONENT_TYPE::RENDERER)
	, _pTargetManager(TargetManager::GetInstance())
{
	Safe_AddRef<TargetManager*>(_pTargetManager);
}

HRESULT Renderer::InitializePrototype()
{
	/*D3D11_VIEWPORT viewPortDesc;

	uint32 iNumViewports = 1;

	_deviceContext->RSGetViewports(&iNumViewports, &viewPortDesc);

	if (FAILED(_pTargetManager->AddRenderTarget(_device, _deviceContext, TEXT("TargetDiffuse"),
		viewPortDesc.Width, viewPortDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM)))
		return E_FAIL;

	if (FAILED(_pTargetManager->AddRenderTarget(_device, _deviceContext, TEXT("TargetNormal"),
		viewPortDesc.Width, viewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM)))
		return E_FAIL;

	if (FAILED(_pTargetManager->AddRenderTarget(_device, _deviceContext, TEXT("TargetShade"),
		viewPortDesc.Width, viewPortDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM)))
		return E_FAIL;

	if (FAILED(_pTargetManager->AddMRT(TEXT("MRT_GameObjects"), TEXT("TargetDiffuse"))))
		return E_FAIL;*/


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
	//if (FAILED(_pTargetManager->BeginMRT(_deviceContext, TEXT("MRT_GameObjects"))))
	//	return E_FAIL;

	for (auto& gameObjects : _renderObjects[static_cast<uint32>(RENDERGROUP::NONBLEND)])
	{
		if (nullptr != gameObjects)
			gameObjects->Render();

		Safe_Release<GameObject*>(gameObjects);
	}

	_renderObjects[static_cast<uint32>(RENDERGROUP::NONBLEND)].clear();

	//if (FAILED(_pTargetManager->EndMRT(_deviceContext)))
	//	return E_FAIL;

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

	Safe_Release<TargetManager*>(_pTargetManager);
}

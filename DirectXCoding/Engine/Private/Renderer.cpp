#include "pch.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Shader.h"
#include "VIBufferRect.h"
#include "RenderTargetManager.h"
#include "LightManager.h"

Renderer::Renderer(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Component(device, deviceContext, COMPONENT_TYPE::RENDERER)
	, _pTargetManager(RenderTargetManager::GetInstance())
	, _pLightManager(LightManager::GetInstance())
{
	Safe_AddRef<RenderTargetManager*>(_pTargetManager);
	Safe_AddRef<LightManager*>(_pLightManager);
}

HRESULT Renderer::InitializePrototype()
{
	D3D11_VIEWPORT		ViewportDesc;

	uint32				iNumViewports = 1;

	_deviceContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	/* For.Target_Diffuse */
	if (FAILED(_pTargetManager->AddRenderTarget(_device, _deviceContext, TEXT("Target_Diffuse"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, Color(1.f, 1.f, 1.f, 0.f))))
		return E_FAIL;

	/* For.Target_Normal */
	if (FAILED(_pTargetManager->AddRenderTarget(_device, _deviceContext, TEXT("Target_Normal"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, Color(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	/* For.Target_Shade */
	if (FAILED(_pTargetManager->AddRenderTarget(_device, _deviceContext, TEXT("Target_Shade"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, Color(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(_pTargetManager->ReadyDebug(TEXT("Target_Diffuse"), 150.0f, 150.f, 300.0f, 300.f)))
		return E_FAIL;
	if (FAILED(_pTargetManager->ReadyDebug(TEXT("Target_Normal"), 150.0f, 450.f, 300.f, 300.f)))
		return E_FAIL;
	if (FAILED(_pTargetManager->ReadyDebug(TEXT("Target_Shade"), 450.0f, 150.f, 300.f, 300.f)))
		return E_FAIL;
#endif

	/* �� ����Ÿ�ٵ��� �׷����� ��ü�κ��� ���� ����޴´�. */
	/* For.MRT_GameObjects */
	if (FAILED(_pTargetManager->AddMRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(_pTargetManager->AddMRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	/* �� ����Ÿ�ٵ��� ���ӳ��� �����ϴ� �����κ��� ������ ����� ����޴´�. */
	/* For.MRT_ */
	if (FAILED(_pTargetManager->AddMRT(TEXT("MRT_Lights"), TEXT("Target_Shade"))))
		return E_FAIL;


	_pVIBuffer = VIBufferRect::Create(_device, _deviceContext);
	if (nullptr == _pVIBuffer)
		return E_FAIL;

	_pShader = Shader::Create(_device, _deviceContext, TEXT("../Binaries/Shaders/Shader_Deferred.fx"), VertexTextureData::Elements, VertexTextureData::numElements);
	if (nullptr == _pShader)
		return E_FAIL;

	XMStoreFloat4x4(&_mWorldMatrix, XMMatrixIdentity());
	_mWorldMatrix._11 = ViewportDesc.Width;
	_mWorldMatrix._22 = ViewportDesc.Height;

	XMStoreFloat4x4(&_mViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&_mProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

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
	if (FAILED(RenderLightAcc()))
		return E_FAIL;
	if (FAILED(RenderDeferred()))
		return E_FAIL;
	if (FAILED(RenderBlend()))
		return E_FAIL;
	if (FAILED(RenderUI()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(RenderDebug()))
		return E_FAIL;
#endif

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
	/* Diffuse + Normal */
	if (FAILED(_pTargetManager->BeginMRT(_deviceContext, TEXT("MRT_GameObjects"))))
		return E_FAIL;

	for (auto& pGameObject : _renderObjects[static_cast<uint32>(RENDERGROUP::NONBLEND)])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	_renderObjects[static_cast<uint32>(RENDERGROUP::NONBLEND)].clear();

	if (FAILED(_pTargetManager->EndMRT(_deviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT Renderer::RenderLightAcc()
{
	/* �������� ����� �׷����� ShadeŸ���� ��ġ�� ���ε� �Ѵ�. */
	if (FAILED(_pTargetManager->BeginMRT(_deviceContext, TEXT("MRT_Lights"))))
		return E_FAIL;


	/* �簢�� ���۸� ������������ ShadeŸ���� �����ŭ �� ä���� �׸�����. */
	if (FAILED(_pShader->BindMatrix("W", &_mWorldMatrix)))
		return E_FAIL;
	if (FAILED(_pShader->BindMatrix("V", &_mViewMatrix)))
		return E_FAIL;
	if (FAILED(_pShader->BindMatrix("P", &_mProjMatrix)))
		return E_FAIL;

	if (FAILED(_pTargetManager->BindSRV(_pShader, TEXT("Target_Normal"), "NormalMap")))
		return E_FAIL;

	_pLightManager->Render(_pShader, _pVIBuffer);


	/* �ٽ� ��ġ�� 0��° ���Ͽ� �� ���۸� �ø���. */
	if (FAILED(_pTargetManager->EndMRT(_deviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT Renderer::RenderDeferred()
{
	/* ��ǻ�� Ÿ�ٰ� ���̵� Ÿ���� ���� ���Ͽ� ����ۿ� ���������� ����. */
	if (FAILED(_pShader->BindMatrix("W", &_mWorldMatrix)))
		return E_FAIL;
	if (FAILED(_pShader->BindMatrix("V", &_mViewMatrix)))
		return E_FAIL;
	if (FAILED(_pShader->BindMatrix("P", &_mProjMatrix)))
		return E_FAIL;

	if (FAILED(_pTargetManager->BindSRV(_pShader, TEXT("Target_Diffuse"), "DiffuseMap")))
		return E_FAIL;

	if (FAILED(_pTargetManager->BindSRV(_pShader, TEXT("Target_Shade"), "ShadeMap")))
		return E_FAIL;

	if (FAILED(_pShader->Begin(3)))
		return E_FAIL;

	if (FAILED(_pVIBuffer->Render()))
		return E_FAIL;

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

#ifdef _DEBUG


HRESULT Renderer::RenderDebug()
{
	
	if (FAILED(_pShader->BindMatrix("V", &_mViewMatrix)))
		return E_FAIL;
	if (FAILED(_pShader->BindMatrix("P", &_mProjMatrix)))
		return E_FAIL;

	if (FAILED(_pTargetManager->Render(TEXT("MRT_GameObjects"), _pShader, _pVIBuffer)))
		return E_FAIL;
	if (FAILED(_pTargetManager->Render(TEXT("MRT_Lights"), _pShader, _pVIBuffer)))
		return E_FAIL;



	return S_OK;
}

#endif // _DEBUG



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

	Safe_Release<Shader*>(_pShader);
	Safe_Release<VIBufferRect*>(_pVIBuffer);
	Safe_Release<LightManager*>(_pLightManager);
	Safe_Release<RenderTargetManager*>(_pTargetManager);

}

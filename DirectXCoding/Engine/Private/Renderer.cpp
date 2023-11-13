#include "pch.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Shader.h"
#include "VIBufferRect.h"
#include "RenderTargetManager.h"
#include "LightManager.h"
#include "GameInstance.h"

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

	if (FAILED(_pTargetManager->AddRenderTarget(_device, _deviceContext, TEXT("Target_Depth"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, Color(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	/* For.Target_Shade */
	if (FAILED(_pTargetManager->AddRenderTarget(_device, _deviceContext, TEXT("Target_Shade"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, Color(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	if (FAILED(_pTargetManager->AddRenderTarget(_device, _deviceContext, TEXT("Target_Specular"),
		ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, Color(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;



#ifdef _DEBUG
	if (FAILED(_pTargetManager->ReadyDebug(TEXT("Target_Diffuse"), 100.0f, 100.f, 200.0f, 200.f)))
		return E_FAIL;
	if (FAILED(_pTargetManager->ReadyDebug(TEXT("Target_Normal"), 100.0f, 300.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(_pTargetManager->ReadyDebug(TEXT("Target_Depth"), 100.0f, 500.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(_pTargetManager->ReadyDebug(TEXT("Target_Shade"), 300.0f, 100.0f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(_pTargetManager->ReadyDebug(TEXT("Target_Specular"), 300.0f, 300.f, 200.f, 200.f)))
		return E_FAIL;
#endif

	/* 이 렌더타겟들은 그려지는 객체로부터 값을 저장받는다. */
	/* For.MRT_GameObjects */
	if (FAILED(_pTargetManager->AddMRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(_pTargetManager->AddMRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(_pTargetManager->AddMRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
		return E_FAIL;
	/* 이 렌더타겟들은 게임내에 존재하는 빛으로부터 연산한 결과를 저장받는다. */
	/* For.MRT_ */
	if (FAILED(_pTargetManager->AddMRT(TEXT("MRT_Lights"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(_pTargetManager->AddMRT(TEXT("MRT_Lights"), TEXT("Target_Specular"))))
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
	/* 빛연산의 결과를 그려놓을 Shade타겟을 장치에 바인딩 한다. */
	if (FAILED(_pTargetManager->BeginMRT(_deviceContext, TEXT("MRT_Lights"))))
		return E_FAIL;


	/* 사각형 버퍼를 직교투영으로 Shade타겟의 사이즈만큼 꽉 채워서 그릴꺼야. */
	if (FAILED(_pShader->BindMatrix("W", &_mWorldMatrix)))
		return E_FAIL;
	if (FAILED(_pShader->BindMatrix("V", &_mViewMatrix)))
		return E_FAIL;
	if (FAILED(_pShader->BindMatrix("P", &_mProjMatrix)))
		return E_FAIL;

	CameraHelper* pCamera = GET_INSTANCE(CameraHelper);

	Matrix viewInv = pCamera->GetInverseTransformCalculator(CameraHelper::TRANSFORMSTATE::D3DTS_VIEW);
	Matrix ProjInv = pCamera->GetInverseTransformCalculator(CameraHelper::TRANSFORMSTATE::D3DTS_PROJ);

	if (FAILED(_pShader->BindMatrix("viewInv", &viewInv)))
		return E_FAIL;

	if (FAILED(_pShader->BindMatrix("projInv", &ProjInv)))
		return E_FAIL;

	Vec4 vCameraPos = pCamera->GetCameraPosition();

	if (FAILED(_pShader->BindRawValue("EyePosition", &vCameraPos, sizeof(Vec4))))
		return E_FAIL;

	RELEASE_INSTANCE(CameraHelper);

	if (FAILED(_pTargetManager->BindSRV(_pShader, TEXT("Target_Normal"), "NormalMap")))
		return E_FAIL;

	if (FAILED(_pTargetManager->BindSRV(_pShader, TEXT("Target_Depth"), "DepthMap")))
		return E_FAIL;


	// TODO
	// Control to Light

	_pLightManager->Render(_pShader, _pVIBuffer);


	/* 다시 장치의 0번째 소켓에 백 버퍼를 올린다. */
	if (FAILED(_pTargetManager->EndMRT(_deviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT Renderer::RenderDeferred()
{
	/* 디퓨즈 타겟과 셰이드 타겟을 서로 곱하여 백버퍼에 최종적으로 찍어낸다. */
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

	if (FAILED(_pTargetManager->BindSRV(_pShader, TEXT("Target_Specular"), "SpecularMap")))
		return E_FAIL;

	if (FAILED(_pShader->Begin(4)))
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
	 
	for (auto& pDebugCom : _renderDebug)
	{
		pDebugCom->Render();
		Safe_Release<Component*>(pDebugCom);
	}
	_renderDebug.clear();

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

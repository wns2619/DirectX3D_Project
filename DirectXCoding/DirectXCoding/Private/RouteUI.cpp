#include "pch.h"
#include "RouteUI.h"
#include "GameInstance.h"
#include "Player.h"

RouteUI::RouteUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: GameObject(pDevice, pDeviceContext, OBJECT_TYPE::UI)
{
	_modelName = "RouteUI";
}

RouteUI::RouteUI(const GameObject& rhs)
	: GameObject(rhs)
{
}

HRESULT RouteUI::InitializePrototype()
{
	return S_OK;
}

HRESULT RouteUI::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	_fSizeX = g_iWinSizeX;
	_fSizeY = g_iWinSizeY;
	
	_fX = _fSizeX * 0.5f;
	_fY = _fSizeY * 0.5f;

	_transform->SetScaling(Vec3(_fSizeX * 0.6f, _fSizeY * 0.2f, 1.f));
	_transform->SetState(Transform::STATE::POSITION, Vec4(_fX - _fSizeX * 0.5f, -_fY + _fSizeY * 0.9f, 0.f, 1.f));

	_mProjMatrix = _mProjMatrix.CreateOrthographic(_fSizeX, _fSizeY, 0.f, 1.f);

	return S_OK;
}

HRESULT RouteUI::Render()
{
	//if (FAILED(Bind_ShaderResources()))
	//	return E_FAIL;

	//if(FAILED(_pShader->Begin(0)))
	//	return E_FAIL;

	//if (FAILED(_pVIBuffer->Render()))
	//	return E_FAIL;

	GameInstance* pGameInstance = GET_INSTANCE(GameInstance);

	Player* pPlayer = static_cast<Player*>(pGameInstance->GetLayerObject(LAYER_TAG::LAYER_PLAYER).front());

	if (nullptr != pPlayer)
	{
		const _bool& bCollDynamic = pPlayer->GetCollDynamic();

		if (true == bCollDynamic)
		{
			const wstring& strEvent = pPlayer->GetEventText();

			pGameInstance->RenderFont(TEXT("DefaultFont"), strEvent, Vec2(_fX, 800.f), Vec4(1.f, 0.f, 0.f, 1.f), 0.f,
				Vec2(0.f, 0.f), 0.5f);
		}
	}



	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

void RouteUI::Tick(const _float& fTimeDelta)
{
}

void RouteUI::LateTick(const _float& fTimeDelta)
{
	_pRenderer->AddRenderGroup(Renderer::RENDERGROUP::UI, this);
}

HRESULT RouteUI::Ready_Components()
{
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentRenderer"),
		TEXT("ComponentRenderer"), reinterpret_cast<Component**>(&_pRenderer))))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentVIBufferRect"),
		TEXT("ComponentVIBuffer"), reinterpret_cast<Component**>(&_pVIBuffer))))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform))))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeShaderRect"),
		TEXT("ComponentShader"), reinterpret_cast<Component**>(&_pShader))))
		return E_FAIL;

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeRouteUI"), TEXT("ComponentTexture"),
		reinterpret_cast<Component**>(&_pTexture))))
		return E_FAIL;

	return S_OK;
}

HRESULT RouteUI::Bind_ShaderResources()
{
	if (FAILED(_transform->BindShaderResources(_pShader, "W")))
		return E_FAIL;

	if (FAILED(_pShader->BindMatrix("V", &_mViewMatrix)))
		return E_FAIL;
	if (FAILED(_pShader->BindMatrix("P", &_mProjMatrix)))
		return E_FAIL;

	if (FAILED(_pTexture->BindShaderResource(_pShader, "UITexture", 0)))
		return E_FAIL;

	return S_OK;
}

GameObject* RouteUI::Clone(void* pArg)
{
	RouteUI* pInstance = new RouteUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : RouteUI");
		Safe_Release<RouteUI*>(pInstance);
	}

	return pInstance;
}

void RouteUI::Free()
{
	__super::Free();

	Safe_Release<Renderer*>(_pRenderer);
	Safe_Release<Shader*>(_pShader);
	Safe_Release<VIBufferRect*>(_pVIBuffer);
	Safe_Release<Texture*>(_pTexture);
}

RouteUI* RouteUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	RouteUI* pInstance = new RouteUI(pDevice, pDeviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to RouteUI");
		Safe_Release<RouteUI*>(pInstance);
	}

	return pInstance;
}

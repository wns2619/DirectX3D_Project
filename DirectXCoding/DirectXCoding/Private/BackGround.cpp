#include "pch.h"
#include "BackGround.h"

#include "GameInstance.h"

BackGround::BackGround(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: GameObject(device, deviceContext, OBJECT_TYPE::BACKGROUND)
{
	_objectType = OBJECT_TYPE::BACKGROUND;
	_modelName = "BackGround";
}

BackGround::BackGround(const GameObject& rhs)
	: GameObject(rhs)
{
	
}

HRESULT BackGround::InitializePrototype()
{

	return S_OK;
}

HRESULT BackGround::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	_sizeX = g_iWinSizeX;
	_sizeY = g_iWinSizeY;

	_x = g_iWinSizeX * 0.5f;
	_y = g_iWinSizeY * 0.5f;

	_transform->SetScaling(Vec3(_sizeX, _sizeY, 1.f));
	_transform->SetState(Transform::STATE::POSITION, ::XMVectorSet(_x - g_iWinSizeX * 0.5f,
		-_y + g_iWinSizeY * 0.5f, 0.f, 1.f));

	::XMStoreFloat4x4(&_viewMatrix, ::XMMatrixIdentity());
	::XMStoreFloat4x4(&_projMatrix, ::XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	return S_OK;
}

void BackGround::Tick(const _float& fTimeDelta)
{
	
}

void BackGround::LateTick(const _float& fTimeDelta)
{
	
	_renderComponent->AddRenderGroup(Renderer::RENDERGROUP::UI, this);
}

HRESULT BackGround::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_shader->Begin(0);

	_viBuffer->Render();

	return S_OK;
}

HRESULT BackGround::Ready_Components()
{
	/* Renderer Component */
 	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentRenderer"),
		TEXT("ComponentRenderer"), reinterpret_cast<Component**>(&_renderComponent))))
		return E_FAIL;

	/* Shader Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentShaderVertexTextureData"),
		TEXT("ComponentShader"), reinterpret_cast<Component**>(&_shader))))
		return E_FAIL;

	/* VIBuffer Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentVIBufferRect"),
		TEXT("ComponentVIBuffer"), reinterpret_cast<Component**>(&_viBuffer))))
		return E_FAIL;

	/* Texture Component */
	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::LOGO), TEXT("ProtoTypeComponentTextureBackGround"),
		TEXT("ComponenetTexture"), reinterpret_cast<Component**>(&_texture))))
		return E_FAIL;

	/* Transform Component */
	Transform::TRANSFORM_DESC transformDesc;
	ZeroMemory(&transformDesc, sizeof(transformDesc));
	{
		transformDesc.speedPerSec = 5.f;
		transformDesc.rotationRadianPerSec = ::XMConvertToRadians(90.f);
	}

	if (FAILED(__super::AddComponent(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		TEXT("ComponentTransform"), reinterpret_cast<Component**>(&_transform), &transformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT BackGround::Bind_ShaderResources()
{
	if (FAILED(_transform->BindShaderResources(_shader, "worldMatrix")))
		return E_FAIL;
	if (FAILED(_shader->BindMatrix("viewMatrix", &_viewMatrix)))
		return E_FAIL;
	if (FAILED(_shader->BindMatrix("ProjMatrix", &_projMatrix)))
		return E_FAIL;

	if (FAILED(_texture->BindShaderResource(_shader, "ShadersTexture", 0)))
		return E_FAIL;
	if (FAILED(_texture->BindShaderReosurces(_shader, "ShadersTextures")))
		return E_FAIL;

	return S_OK;
}

GameObject* BackGround::Clone(void* argument)
{
	BackGround* pInstance = new BackGround(*this);
	
	if (FAILED(pInstance->Initialize(argument)))
	{
		MSG_BOX("Failed Cloned BackGround");
		Safe_Release<BackGround*>(pInstance);
		return nullptr;
	}

	return pInstance;
}

void BackGround::Free()
{
	__super::Free();

	Safe_Release<Texture*>(_texture);
	Safe_Release<Shader*>(_shader);
	Safe_Release<VIBufferRect*>(_viBuffer);
	Safe_Release<Renderer*>(_renderComponent);
	Safe_Release<Transform*>(_transform);
}

BackGround* BackGround::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	BackGround* pInstance = new BackGround(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed Create BackGround");
		Safe_Release<BackGround*>(pInstance);
		return nullptr;
	}

	return pInstance;
}

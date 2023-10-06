#include "pch.h"
#include "Game.h"
#include "GameInstance.h"

#include "LevelLoading.h"

#include "ImGuiManager.h"
#include "ImguiResourceHandler.h"


Game::Game()
	: _gameInstance(GameInstance::GetInstance())
{
	Safe_AddRef<GameInstance*>(_gameInstance);
}

Game::~Game()
{
}

HRESULT Game::Initialize(HWND hwnd)
{
	GRAPHIC_DESC desc;
	ZeroMemory(&desc, sizeof(GRAPHIC_DESC));
	{
		desc.iWinSizeX = g_iWinSizeX;
		desc.iWinSizeY = g_iWinSizeY;
		desc._winMode = GRAPHIC_DESC::WINMODE_WIN;
		desc._hwnd = g_hWnd;
	}

	if (FAILED(_gameInstance->Initialize_Engine(static_cast<uint32>(LEVEL::LEVEL_END), g_hInstance, desc, &_device, &_devicecontext)))
		return E_FAIL;


	ImGuiManager::GetInstance()->Initialize(_device, _devicecontext);
	ImGuiResourceHandler::GetInstance()->Initialize(_device, _devicecontext);




	if (FAILED(ReadyProtoTypeComponents()))
		return E_FAIL;


	if (FAILED(OpenLevel(LEVEL::EDIT)))
		return E_FAIL;
	
	
	return S_OK;
}

void Game::Tick(const _float& timeDelta)
{
	//ImGuiManager::GetInstance()->LateTick(timeDelta);
	_gameInstance->Tick(timeDelta);
}

HRESULT Game::Render()
{
	Color color = Color{ 0.5f,0.5f,0.5f,0.5f };

	_gameInstance->ClearBackBufferView(color);
	_gameInstance->ClearDepthStencilView();


	_renderer->DrawRenderObjects();


	if (_gameInstance->GetCurrentLevelIndex() == static_cast<uint32>(LEVEL::EDIT))
		ImGuiManager::GetInstance()->Render();



	_gameInstance->Present();

	return S_OK;
}

HRESULT Game::OpenLevel(LEVEL levelID)
{
	if (nullptr == _gameInstance)
		return E_FAIL;

	if (FAILED(_gameInstance->OpenLevel(static_cast<uint32>(LEVEL::LOADING), LevelLoading::Create(_device, _devicecontext, levelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT Game::ReadyProtoTypeComponents()
{

	/* ProtoType Component Renderer */
	
	//wstring test = TEXT("ProtoTypeComponentRenderer");

	if(FAILED(_gameInstance->AddProtoType(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentRenderer"),
		_renderer = Renderer::Create(_device, _devicecontext))))
		return E_FAIL;

	/* ProtoType Component Shader_VtxPosTex */

	if (FAILED(_gameInstance->AddProtoType(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentShaderVertexTextureData"),
		Shader::Create(_device, _devicecontext, TEXT("../Binaries/Shaders/Shader_VtxPosTex.fx"), VertexTextureData::Elements,
			VertexTextureData::numElements))))
		return E_FAIL;

	/* ProtoType Component VIBuffer Rect */
	
	if(FAILED(_gameInstance->AddProtoType(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentVIBufferRect"),
		VIBufferRect::Create(_device, _devicecontext))))
		return E_FAIL;

	/* ProtoType Component Transform */

	if (FAILED(_gameInstance->AddProtoType(static_cast<uint32>(LEVEL::STATIC), TEXT("ProtoTypeComponentTransform"),
		Transform::Create(_device, _devicecontext))))
		return E_FAIL;


	Safe_AddRef<Renderer*>(_renderer);

	return S_OK;
}

Game* Game::Create(HWND hwnd)
{
	Game* pInstance = new Game;

	if (FAILED(pInstance->Initialize(hwnd)))
	{
		MSG_BOX("Game Create Failed");
		Safe_Release<Game*>(pInstance);

		return nullptr;
	}

	return pInstance;
}

void Game::Free()
{
	__super::Free();

	Safe_Release<Renderer*>(_renderer);

	Safe_Release<ID3D11Device*>(_device);
	Safe_Release<ID3D11DeviceContext*>(_devicecontext);

	Safe_Release<GameInstance*>(_gameInstance);


	ImGuiResourceHandler::GetInstance()->DestroyInstance();
	ImGuiManager::GetInstance()->DestroyInstance();

	GameInstance::Release_Engine();
}

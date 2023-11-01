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


	//if (FAILED(ReadyGara()))
	//	return E_FAIL;

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

HRESULT Game::ReadyGara()
{
	ID3D11Texture2D* pTexture2D = { nullptr };

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof TextureDesc);

	TextureDesc.Width = 256;
	TextureDesc.Height = 256;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	// _ulong		Data = 10ul;
	_ulong* pPixel = new _ulong[TextureDesc.Width * TextureDesc.Height];

	for (size_t i = 0; i < 256; i++)
	{
		for (size_t j = 0; j < 256; j++)
		{
			uint32		iIndex = i * 256 + j;

			pPixel[iIndex] = D3DCOLOR_ARGB(255, 255, 255, 255);
		}
	}



	D3D11_SUBRESOURCE_DATA			InitialData;
	ZeroMemory(&InitialData, sizeof InitialData);
	InitialData.pSysMem = pPixel;
	InitialData.SysMemPitch = TextureDesc.Width * 4;


	if (FAILED(_device->CreateTexture2D(&TextureDesc, &InitialData, &pTexture2D)))
		return E_FAIL;

	SaveDDSTextureToFile(_devicecontext, pTexture2D, TEXT("../Binaries/Resources/Textures/Terrain/MyMask.dds"));


	D3D11_MAPPED_SUBRESOURCE	MappedSubResource;
	ZeroMemory(&MappedSubResource, sizeof MappedSubResource);

	pPixel[0] = D3DCOLOR_ARGB(255, 0, 0, 255);

	_devicecontext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

	memcpy(MappedSubResource.pData, pPixel, sizeof(_ulong) * 256 * 256);

	_devicecontext->Unmap(pTexture2D, 0);

	SaveDDSTextureToFile(_devicecontext, pTexture2D, TEXT("../Binaries/Resources/Textures/Terrain/MyMask.dds"));

	Safe_Delete_Array(pPixel);
	Safe_Release(pTexture2D);


	/* 네비게이션의 정보를 생성한다. */

	_ulong			dwByte = 0;
	HANDLE			hFile = CreateFile(TEXT("../Binaries/Data/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	Vec3			vPoints[3];

	ZeroMemory(vPoints, sizeof(Vec3));
	vPoints[0] = Vec3(0.f, 0.f, 10.f);
	vPoints[1] = Vec3(10.f, 0.f, 0.f);
	vPoints[2] = Vec3(0.f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(Vec3) * 3, &dwByte, nullptr);

	ZeroMemory(vPoints, sizeof(Vec3));
	vPoints[0] = Vec3(0.f, 0.f, 10.f);
	vPoints[1] = Vec3(10.f, 0.f, 10.f);
	vPoints[2] = Vec3(10.f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(Vec3) * 3, &dwByte, nullptr);

	ZeroMemory(vPoints, sizeof(Vec3));
	vPoints[0] = Vec3(0.f, 0.f, 20.f);
	vPoints[1] = Vec3(10.f, 0.f, 10.f);
	vPoints[2] = Vec3(0.f, 0.f, 10.f);
	WriteFile(hFile, vPoints, sizeof(Vec3) * 3, &dwByte, nullptr);


	ZeroMemory(vPoints, sizeof(Vec3));
	vPoints[0] = Vec3(10.f, 0.f, 10.f);
	vPoints[1] = Vec3(20.f, 0.f, 00.f);
	vPoints[2] = Vec3(10.f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(Vec3) * 3, &dwByte, nullptr);

	CloseHandle(hFile);

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

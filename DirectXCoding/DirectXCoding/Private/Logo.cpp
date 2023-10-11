#include "pch.h"
#include "Logo.h"

#include "GameInstance.h"
#include "LevelLoading.h"

Logo::Logo(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Level(device, deviceContext)
{

}

HRESULT Logo::Initialize()
{
/*	if (FAILED(ReadyLayerBackGround(TEXT("LayerBackGround"))))
		return E_FAIL*/;

	return S_OK;
}

HRESULT Logo::Tick(const _float& timeDelta)
{
	return S_OK;
}

HRESULT Logo::LateTick(const _float& timeDelta)
{
	SetWindowText(g_hWnd, TEXT("This is LogoLevel"));

	GameInstance* _gameInstance = GET_INSTANCE(GameInstance);


	if (_gameInstance->Get_DIKeyState(DIK_0) & 0x80)
	{
		if(FAILED(_gameInstance->OpenLevel(static_cast<uint32>(LEVEL::LOADING), LevelLoading::Create(_device, _deviceContext, LEVEL::GAME))))
			return E_FAIL;
	}


	if (_gameInstance->Get_DIKeyState(DIK_1) & 0x80)
	{
		if (FAILED(_gameInstance->OpenLevel(static_cast<uint32>(LEVEL::LOADING), LevelLoading::Create(_device, _deviceContext, LEVEL::EDIT))))
			return E_FAIL;
	}


	RELEASE_INSTANCE(GameInstance);

	return S_OK;
}

HRESULT Logo::ReadyLayerBackGround(const LAYER_TAG layerTag)
{
	GameInstance* gameInstance = GameInstance::GetInstance();
	Safe_AddRef<GameInstance*>(gameInstance);

	if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::LOGO), layerTag, TEXT("PrototypeBackGround"))))
		return E_FAIL;

	Safe_Release<GameInstance*>(gameInstance);

	return S_OK;
}

Logo* Logo::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	Logo* level = new Logo(device, deviceContext);

	if (FAILED(level->Initialize()))
	{
		Safe_Release<Logo*>(level);
		MSG_BOX("Level Create falied");
		return nullptr;
	}

	return level;
}

void Logo::Free()
{
	__super::Free();
}

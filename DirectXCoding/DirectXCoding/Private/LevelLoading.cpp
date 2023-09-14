#include "pch.h"
#include "LevelLoading.h"

#include "GameInstance.h"
#include "LevelHelper.h"

#include "Logo.h"
#include "GameScene.h"

LevelLoading::LevelLoading(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Level(device, deviceContext)
{
}

LevelLoading::~LevelLoading()
{
}

HRESULT LevelLoading::Initialize(LEVEL nextLevel)
{
	_nextLevel = nextLevel;

	_helper = LevelHelper::Create(_device, _deviceContext, _nextLevel);

	if (nullptr == _helper)
		return E_FAIL;

	return S_OK;
}

HRESULT LevelLoading::Tick(const _float& timeDelta)
{
	return S_OK;
}

HRESULT LevelLoading::LateTick(const _float& timeDelta)
{
	wstring lodingText = _helper->GetLoadingText();

	SetWindowText(g_hWnd, lodingText.c_str());

	if (GetKeyState(VK_SPACE) & 0x8000)
	{
		if (true == _helper->IsFinished())
		{
			GameInstance* gameInstance = GameInstance::GetInstance();
			Safe_AddRef<GameInstance*>(gameInstance);

			Level* newLevel = nullptr;

			switch (_nextLevel)
			{
			case LEVEL::LOGO:
				newLevel = Logo::Create(_device, _deviceContext);
				break;
			case LEVEL::GAME:
				newLevel = GameScene::Create(_device, _deviceContext);
				break;
			}

			if (nullptr == newLevel)
				return E_FAIL;

			if (FAILED(gameInstance->OpenLevel(static_cast<uint32>(_nextLevel), newLevel)))
				return E_FAIL;

			Safe_Release<GameInstance*>(gameInstance);
		}
	}

	return S_OK;
}

LevelLoading* LevelLoading::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LEVEL nextLevel)
{
	LevelLoading* _LevelLoading = new LevelLoading(device, deviceContext);

	if (FAILED(_LevelLoading->Initialize(nextLevel)))
	{
		MSG_BOX("Failed Create LevelLoading");
		Safe_Release<LevelLoading*>(_LevelLoading);
		return nullptr;
	}
	
	return _LevelLoading;
}

void LevelLoading::Free()
{
	__super::Free();

	Safe_Release<LevelHelper*>(_helper);
}

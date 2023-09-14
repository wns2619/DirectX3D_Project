#include "pch.h"
#include "GameScene.h"

GameScene::GameScene(ID3D11Device* _device, ID3D11DeviceContext* _deviceConetxt)
    : Level(_device, _deviceConetxt)
{

}

GameScene::~GameScene()
{

}

HRESULT GameScene::Initialize()
{
    return S_OK;
}

HRESULT GameScene::Tick(const _float& timeDelta)
{
    return S_OK;
}

HRESULT GameScene::LateTick(const _float& timeDelata)
{
    SetWindowText(g_hWnd, TEXT("This is GameLevel"));

    return S_OK;
}

GameScene* GameScene::Create(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext)
{
    GameScene* scene = new GameScene(_device, _deviceContext);

    if (FAILED(scene->Initialize()))
    {
        MSG_BOX("Failed Create GameScene");
        Safe_Release<GameScene*>(scene);
        return nullptr;
    }

    return scene;
}

void GameScene::Free()
{
    __super::Free();
}

#include "pch.h"
#include "GameScene.h"

#include "GameInstance.h"
#include "PlayerCamera.h"

GameScene::GameScene(ID3D11Device* _device, ID3D11DeviceContext* _deviceConetxt)
    : Level(_device, _deviceConetxt)
{

}

GameScene::~GameScene()
{

}

HRESULT GameScene::Initialize()
{
    if (FAILED(ReadyLayerTerrain(LAYER_TAG::LAYER_TERRAIN)))
        return E_FAIL;

    if (FAILED(ReadyLayerCamera(LAYER_TAG::LAYER_CAMERA)))
        return E_FAIL;

    if (FAILED(ReadyLayerPlayer(LAYER_TAG::LAYER_PLAYER)))
        return E_FAIL;

    if (FAILED(ReadyLight()))
        return E_FAIL;

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

HRESULT GameScene::ReadyLayerTerrain(const LAYER_TAG layerTag)
{

    GameInstance* gameInstance = GameInstance::GetInstance();
    Safe_AddRef<GameInstance*>(gameInstance);

    if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::GAME), layerTag, TEXT("ProtoTypeGameObjectTerrain"))))
        return E_FAIL;

    Safe_Release<GameInstance*>(gameInstance);


    return S_OK;
}

HRESULT GameScene::ReadyLayerCamera(const LAYER_TAG layerTag)
{
    GameInstance* gameInstance = GameInstance::GetInstance();
    Safe_AddRef<GameInstance*>(gameInstance);

    PlayerCamera::PLAYERCAMERA_DESC cameraDesc;
    ZeroMemory(&cameraDesc, sizeof(cameraDesc));
    {
        cameraDesc._mouseSensitive = 0.1f;
        cameraDesc._eye = Vec4(0.f, 10.f, -8.f, 1.f);
        cameraDesc._at = Vec4(0.f, 0.f, 0.f, 1.f);
        cameraDesc._fovy = ::XMConvertToRadians(60.f);
        cameraDesc._aspect = g_iWinSizeX / static_cast<_float>(g_iWinSizeY);
        cameraDesc._near = 0.2f;
        cameraDesc._far = 300.f;
        cameraDesc.speedPerSec = 10.f;
        cameraDesc.rotationRadianPerSec = ::XMConvertToRadians(90.f);
    }

    if(FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::GAME), layerTag, TEXT("ProtoTypeGameObjectCamera"),
        &cameraDesc)))
        return E_FAIL;

    Safe_Release<GameInstance*>(gameInstance);

    return S_OK;
}

HRESULT GameScene::ReadyLayerPlayer(const LAYER_TAG layerTag)
{

    GameInstance* gameInstance = GameInstance::GetInstance();
    Safe_AddRef<GameInstance*>(gameInstance);

    if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::GAME), layerTag, TEXT("ProtoTypeGameObjectPlayer"))))
        return E_FAIL;
   
    Safe_Release<GameInstance*>(gameInstance);


    return S_OK;
}

HRESULT GameScene::ReadyLight()
{
    GameInstance* gameInstance = GET_INSTANCE(GameInstance);


    LIGHT_DESC lightDesc;
    ZeroMemory(&lightDesc, sizeof(lightDesc));
    {
        lightDesc.Position = Vec4(0.f, 5.f, 0.f, 1.f);
        lightDesc.Diffuse = Vec3(1.f, 1.f, 1.f);
        lightDesc.intensity = 1.f;
        lightDesc.range = 4.f;
        lightDesc.type = LIGHT_DESC::DIRECTION;
        lightDesc.enabled = true;

        lightDesc.Direction = Vec3(1.f, -1.f, 1.f);
        lightDesc.Ambient = Vec4(1.f, 1.f, 1.f, 1.f);
        lightDesc.Specular = Vec4(1.f, 1.f, 1.f, 1.f);
    }

    if (FAILED(gameInstance->AddLight(lightDesc)))
        return E_FAIL;

    RELEASE_INSTANCE(GameInstance);


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

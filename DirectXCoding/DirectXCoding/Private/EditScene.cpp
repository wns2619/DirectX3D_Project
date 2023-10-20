#include "pch.h"
#include "EditScene.h"


#include "GameInstance.h"
#include "ToolCamera.h"
#include "WallPainting.h"

#include "ImGuiManager.h"
#include "ImguiResourceHandler.h"


EditScene::EditScene(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
    : Level(device, deviceContext)
{
   
}

EditScene::~EditScene()
{
}

HRESULT EditScene::Initialize()
{
    if (FAILED(ReadyLayerEditCamera(LAYER_TAG::LAYER_CAMERA)))
        return E_FAIL;

    //if(FAILED(ReadyEnvironment(LAYER_TAG::LAYER_ENVIRONMENT)))
    //    return E_FAIL;

    if (FAILED(ReadyLight()))
        return E_FAIL;

    return S_OK;
}

HRESULT EditScene::Tick(const _float& timeDelta)
{
    return S_OK;
}

HRESULT EditScene::LateTick(const _float& timeDelata)
{
    return S_OK;
}

HRESULT EditScene::ReadyEnvironment(const LAYER_TAG layerTag)
{
    GameInstance* gameInstance = GET_INSTANCE(GameInstance);

    for (uint32 i = 0; i < WallPainting::PAINT_TYPE::PAINT_END; ++i)
    {
        if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::EDIT), layerTag, TEXT("ProtoWallPaint"), &i)))
            return E_FAIL;
    }


    RELEASE_INSTANCE(GameInstance);
}

HRESULT EditScene::ReadyLayerEditCamera(const LAYER_TAG layerTag)
{
    GameInstance* gameInstance = GET_INSTANCE(GameInstance);

    ToolCamera::CAMERATOOL_DESC cameraDesc;
    ZeroMemory(&cameraDesc, sizeof(cameraDesc));
    {
        cameraDesc.fMouseSensitive = 0.3f;
        cameraDesc._eye = Vec4(0.f, 0.f, -1.f, 1.f);
        cameraDesc._at = Vec4(0.f, 0.f, 0.f, 1.f);
        cameraDesc._fovy = ::XMConvertToRadians(90.f);
        cameraDesc._aspect = g_iWinSizeX / static_cast<_float>(g_iWinSizeY);
        cameraDesc._near = 0.01f;
        cameraDesc._far = 1000.f;
        cameraDesc.speedPerSec = 10.f;
        cameraDesc.rotationRadianPerSec = ::XMConvertToRadians(30.f);
    }

    if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::EDIT), layerTag, TEXT("ProtoTypeGameObjectEditCamera"),
        &cameraDesc)))
        return E_FAIL;

    RELEASE_INSTANCE(GameInstance);


    return S_OK;
}

HRESULT EditScene::ReadyLayerEntire(const LAYER_TAG layerTage)
{
    return S_OK;
}

HRESULT EditScene::ReadyTerrain(const LAYER_TAG layerTag)
{
    GameInstance* gameInstance = GET_INSTANCE(GameInstance);

    if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::EDIT), layerTag, TEXT("ProtoTypeGameObjectEditTerrain"))))
        return E_FAIL;

    RELEASE_INSTANCE(GameInstance);
}

HRESULT EditScene::ReadyLight()
{
    GameInstance* gameInstance = GET_INSTANCE(GameInstance);


    LIGHT_DESC lightDesc;
    ZeroMemory(&lightDesc, sizeof(lightDesc));
    {
        lightDesc.Position = Vec4(0.f, 5.f, 0.f, 1.f);
        lightDesc.Diffuse = Vec4(0.8f, 0.8f, 0.8f, 1.f);
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

}

EditScene* EditScene::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    EditScene* editscene = new EditScene(device, deviceContext);

    if (FAILED(editscene->Initialize()))
    {
        MSG_BOX("Failed Create EditScene");
        Safe_Release<EditScene*>(editscene);
        return nullptr;
    }

    return editscene;
}

void EditScene::Free()
{


    __super::Free();
}

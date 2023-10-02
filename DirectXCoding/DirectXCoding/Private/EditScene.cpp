#include "pch.h"
#include "EditScene.h"


#include "GameInstance.h"
#include "PlayerCamera.h"

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
    if (FAILED(ReadyLayerEditCamera(TEXT("LayerEntireObject"))))
        return E_FAIL;


    //Im
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

HRESULT EditScene::ReadyLayerEditCamera(const wstring& layerTag)
{
    GameInstance* gameInstance = GET_INSTANCE(GameInstance);

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

    if (FAILED(gameInstance->AddGameObject(static_cast<uint32>(LEVEL::EDIT), layerTag, TEXT("ProtoTypeGameObjectEditCamera"),
        &cameraDesc)))
        return E_FAIL;

    RELEASE_INSTANCE(GameInstance);


    return S_OK;
}

HRESULT EditScene::ReadyLayerEntire(const wstring& layerTage)
{
    return S_OK;
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

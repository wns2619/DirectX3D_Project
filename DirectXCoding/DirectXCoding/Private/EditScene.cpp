#include "pch.h"
#include "EditScene.h"

#include "ImGuiManager.h"

EditScene::EditScene(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
    : Level(device, deviceContext)
{
    ImGuiManager::GetInstance()->Initialize(device, deviceContext);
}

EditScene::~EditScene()
{
}

HRESULT EditScene::Initialize()
{


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
    ImGuiManager::GetInstance()->DestroyInstance();

    __super::Free();
}

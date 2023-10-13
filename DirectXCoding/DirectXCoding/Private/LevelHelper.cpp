#include "pch.h"
#include "LevelHelper.h"
#include "GameInstance.h"
#include "BackGround.h"
#include "Terrain.h"
#include "PlayerCamera.h"
#include "EditorTerrain.h"
#include "Player.h"
#include "ImguiResourceHandler.h"
#include "OBBBoxCollider.h"
#include "AABBboxCollider.h"
#include "SphereCollider.h"
#include "ToolCamera.h"
#include "PlayerBody.h"
#include "Surefire.h"
#include "StaticObject.h"

LevelHelper::LevelHelper(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
    : _device(device), _deviceContext(deviceContext)
{
    Safe_AddRef<ID3D11Device*>(_device);
    Safe_AddRef<ID3D11DeviceContext*>(_deviceContext);
}

LevelHelper::~LevelHelper()
{
}

uint32 APIENTRY ThreadEntry(void* Argument)
{

    if(FAILED(CoInitializeEx(nullptr, 0)))
        return E_FAIL;

    LevelHelper* helper = static_cast<LevelHelper*>(Argument);

    helper->Loading();

    return 0;
}

HRESULT LevelHelper::Initialize(LEVEL nextLevel)
{
    InitializeCriticalSection(&_criticalSection);

    _nextLevel = nextLevel;

    _lodingThread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, ThreadEntry, this, 0, nullptr));

    if (0 == _lodingThread)
        return E_FAIL;

    return S_OK;
}

int32 LevelHelper::Loading()
{
    EnterCriticalSection(&_criticalSection);

    HRESULT hr = 0;

    switch (_nextLevel)
    {
    case LEVEL::LOGO:
        hr = LodingforLevelLogo();
        break;
    case LEVEL::GAME:
        hr = LodingforLevelGame();
        break;
    case LEVEL::EDIT:
        hr = LodingforLevelEdit();
        break;
    }


    if (FAILED(hr))
    {
        LeaveCriticalSection(&_criticalSection);
        return -1;
    }
 
    LeaveCriticalSection(&_criticalSection);
    return 0;
}

HRESULT LevelHelper::LodingforLevelLogo()
{
    _title = TEXT("Texture Loading");
    LoadingTexture();

    _title = TEXT("Mesh Loading");
    LoadingMesh();

    _title = TEXT("Shader Loading");
    LoadingShader();

    _title = TEXT("Object Loading");
    LoadingObject();


    _title = TEXT("Loading Successed");
    _IsFinished = true;

    return S_OK;
}

HRESULT LevelHelper::LodingforLevelGame()
{
    //if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeComponentPhysics"),
    //    Physics::Create(_device, _deviceContext))))
    //    return E_FAIL;


    _title = TEXT("Texture Loading");
    LoadingTexture();

    _title = TEXT("Mesh Loading");
    LoadingMesh();

    _title = TEXT("Shader Loading");
    LoadingShader();

    _title = TEXT("Object Loading");
    LoadingObject();

    _title = TEXT("Loading Successed");
    _IsFinished = true;

    return S_OK;
}

HRESULT LevelHelper::LodingforLevelEdit()
{
    GameInstance* gameInstance = GET_INSTANCE(GameInstance);

    _title = TEXT("Texture Loading");
    LoadingTexture();

    _title = TEXT("Mesh Loading");
    LoadingMesh();

    _title = TEXT("Shader Loading");
    LoadingShader();

    _title = TEXT("Object Loading");
    LoadingObject();


    // PLAYER
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Player\\Player.fbx", LAYER_TAG::LAYER_PLAYER, TEXT("ProtoTypeGameObjectPlayer"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Player\\Player.fbx", TEXT("ProtoTypeModelPlayer"), TEXT("ProtoTypeComponentAnimMesh"));
    // STATIC

#pragma region 2stBottom
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stBottom.fbx", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stBottom.fbx", TEXT("ProtoType2stBottom"), TEXT("ProtoTypeComponentDefaultMeshShader"));
#pragma endregion 2stBottom

#pragma region 2stWall
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stwall.fbx", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stwall.fbx", TEXT("ProtoType2stWall"), TEXT("ProtoTypeComponentDefaultMeshShader"));
#pragma endregion 2stWall

    _title = TEXT("Loading Successed");
    _IsFinished = true;


    RELEASE_INSTANCE(GameInstance);
    return S_OK;
}

HRESULT LevelHelper::LoadingTexture()
{
    GameInstance* gameInstance = GET_INSTANCE(GameInstance);


    if (_nextLevel >= LEVEL::LEVEL_END)
        return E_FAIL;

    switch (_nextLevel)
    {
    case Client::LEVEL::LOGO:
        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::LOGO), TEXT("ProtoTypeComponentTextureBackGround"),
            Texture::Create(_device, _deviceContext, TEXT("../Binaries/Resources/Textures/Default%d.jpg"), 2))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }
        break;
    case Client::LEVEL::GAME:
        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeComponentTextureTerrain"),
            Texture::Create(_device, _deviceContext, TEXT("..\\Binaries\\Resources\\Textures\\Terrain\\Tile0.jpg")))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }
        break;
    case Client::LEVEL::EDIT:
        break;
    default:
        break;
    }

    RELEASE_INSTANCE(GameInstance);

    return S_OK;
}

HRESULT LevelHelper::LoadingMesh()
{
    GameInstance* gameInstance = GET_INSTANCE(GameInstance);


    if (_nextLevel >= LEVEL::LEVEL_END)
        return E_FAIL;

    switch (_nextLevel)
    {
    case Client::LEVEL::LOGO:
        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::LOGO), TEXT("ProtoTypeComponentTextureBackGround"),
            Texture::Create(_device, _deviceContext, TEXT("../Binaries/Resources/Textures/Default%d.jpg"), 2))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }
        break;
    case Client::LEVEL::GAME:
        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeComponentVIBufferTerrain"),
            VIBufferTerrain::Create(_device, _deviceContext, TEXT("..\\Binaries\\Resources\\Textures\\Terrain\\Height1.bmp")))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        XMMATRIX modelInitializMatrix = ::XMMatrixIdentity();
        modelInitializMatrix = ::XMMatrixRotationY(::XMConvertToRadians(180.f));

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeModelPlayer"),
             Model::Create(_device, _deviceContext, Model::MODEL_TYPE::ANIM, "..\\Binaries\\Resources\\MyModels\\Player\\Player.fbx", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }
        break;
    case Client::LEVEL::EDIT:
    {
        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeComponentEditVIBufferTerrain"),
            VIBufferTerrain::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        Matrix modelInitializMatrix = ::XMMatrixIdentity();
        modelInitializMatrix = ::XMMatrixRotationY(::XMConvertToRadians(180.f));


         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeModelPlayer"),
            Model::Create(_device, _deviceContext, Model::MODEL_TYPE::ANIM, "..\\Binaries\\Resources\\MyModels\\Player\\Player.fbx", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeModelsurefire"),
             Model::Create(_device, _deviceContext, Model::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Player\\surefire.fbx", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stBottom"),
             Model::Create(_device, _deviceContext, Model::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stBottom.fbx", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stWall"),
             Model::Create(_device, _deviceContext, Model::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stwall.fbx", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

    }
        break;
    default:
        break;
    }

    RELEASE_INSTANCE(GameInstance);

    return S_OK;
}

HRESULT LevelHelper::LoadingShader()
{
    GameInstance* gameInstance = GET_INSTANCE(GameInstance);

    if (_nextLevel >= LEVEL::LEVEL_END)
        return E_FAIL;

    switch (_nextLevel)
    {
    case Client::LEVEL::LOGO:
        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::LOGO), TEXT("ProtoTypeComponentTextureBackGround"),
            Texture::Create(_device, _deviceContext, TEXT("../Binaries/Resources/Textures/Default%d.jpg"), 2))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }
        break;
    case Client::LEVEL::GAME:
        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeComponentHillsShader"),
            Shader::Create(_device, _deviceContext, TEXT("..\\Binaries\\Shaders\\HillsShader.fx"),
                VertexTextureNormalData::Elements, VertexTextureNormalData::numElements))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }
        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeComponentAnimMesh"),
            Shader::Create(_device, _deviceContext, TEXT("..\\Binaries\\Shaders\\AnimMesh.fx"),
                VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }
        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeComponentDefaultMeshShader"),
            Shader::Create(_device, _deviceContext, TEXT("..\\Binaries\\Shaders\\DefaultMeshShader.fx"), VTXMESH::Elements, VTXMESH::iNumElements))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        break;
    case Client::LEVEL::EDIT:
        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeComponentEditHillsShader"),
            Shader::Create(_device, _deviceContext, TEXT("../Binaries/Shaders/WireFrameHills.fx"),
                VertexTextureNormalData::Elements, VertexTextureNormalData::numElements))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeComponentDefaultMeshShader"),
            Shader::Create(_device, _deviceContext, TEXT("../Binaries/Shaders/DefaultMeshShader.fx"),
                VTXMESH::Elements, VTXMESH::iNumElements))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }
        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeComponentAnimMesh"),
            Shader::Create(_device, _deviceContext, TEXT("..\\Binaries\\Shaders\\AnimMesh.fx"),
                VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }
        break;
    default:
        break;
    }

    RELEASE_INSTANCE(GameInstance);

    return S_OK;
}

HRESULT LevelHelper::LoadingObject()
{
    GameInstance* gameInstance = GET_INSTANCE(GameInstance);

    if (_nextLevel >= LEVEL::LEVEL_END)
        return E_FAIL;

    switch (_nextLevel)
    {
    case Client::LEVEL::LOGO:
        if (FAILED(gameInstance->AddProtoType(TEXT("PrototypeBackGround"), BackGround::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }
        break;
    case Client::LEVEL::GAME:
        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeGameObjectTerrain"),
            Terrain::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }
        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeGameObjectCamera"),
            PlayerCamera::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeGameObjectPlayer"),
            Player::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }
        break;
    case Client::LEVEL::EDIT:
        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeGameObjectEditTerrain"),
            EditorTerrain::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

         if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeGameObjectEditCamera"),
            ToolCamera::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeGameObjectPlayer"),
            Player::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeGameObjectPlayerBody"),
            PlayerBody::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeGameObjectPlayerSurefire"),
            Surefire::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeStaticObject"),
            StaticObject::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }
        break;
    default:
        break;
    }

    RELEASE_INSTANCE(GameInstance);

    return S_OK;
}

LevelHelper* LevelHelper::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LEVEL nextLevel)
{
    LevelHelper* helper = new LevelHelper(device, deviceContext);

    if (FAILED(helper->Initialize(nextLevel)))
    {
        MSG_BOX("Failed Create Helper");
        Safe_Release<LevelHelper*>(helper);
        return nullptr;
    }

    return helper;
}

void LevelHelper::Free()
{
    __super::Free();

    WaitForSingleObject(_lodingThread, INFINITE);

    DeleteCriticalSection(&_criticalSection);

    CloseHandle(_lodingThread);


    Safe_Release<ID3D11Device*>(_device);
    Safe_Release<ID3D11DeviceContext*>(_deviceContext);
}

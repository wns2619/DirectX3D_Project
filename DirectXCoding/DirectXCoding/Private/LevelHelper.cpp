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
#include "Navigation.h"

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
    GameInstance* gameInstance = GET_INSTANCE(GameInstance);


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

    if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeNavigation"),
        Navigation::Create(_device, _deviceContext, TEXT("../Binaries/Data/Navigation.dat")))))
        return E_FAIL;


    RELEASE_INSTANCE(GameInstance);
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

    // Components
    //if(FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeNavigation"),
    //    Navigation::Create(_device, _deviceContext)


#pragma region 2stProbs
    
    // PLAYER
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Player\\Player.dat", LAYER_TAG::LAYER_PLAYER, TEXT("ProtoTypeGameObjectPlayer"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Player\\Player.dat", TEXT("ProtoTypeModelPlayer"), TEXT("ProtoTypeComponentAnimMesh"));
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Player\\surefire.dat", LAYER_TAG::LAYER_PLAYER, TEXT("ProtoTypeGameObjectPlayer"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Player\\surefire.dat", TEXT("ProtoTypeModelPlayer"), TEXT("ProtoTypeComponentAnimMesh"));
   
    // STATIC
    /* Botton */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stBottom.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stBottom.dat", TEXT("ProtoType2stBottom"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Wall */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stwall.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stwall.dat", TEXT("ProtoType2stWall"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Ceiling */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stCeiling.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stCeiling.dat", TEXT("ProtoType2stCeiling"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* DistributitonBox */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stDistributionBox.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stDistributionBox.dat", TEXT("ProtoType2stDistributionBox"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* SteelDesk */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stSteelDesk.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stSteelDesk.dat", TEXT("ProtoType2stSteelDesk"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* ModularBox */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stModularBox.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stModularBox.dat", TEXT("ProtoType2stModularBox"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* CabinetMeta; */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stCabinetMetal.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stCabinetMetal.dat", TEXT("ProtoType2stCabinetMetal"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* CeilingLamp */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stCeilingLamp.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stCeilingLamp.dat", TEXT("ProtoType2stCeilingLamp"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Column Lamp*/
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stColumnLamp.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stColumnLamp.dat", TEXT("ProtoType2stColumnLamp"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* CurvePipeLineGroup */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stCurvePipeGroup.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stCurvePipeGroup.dat", TEXT("ProtoType2stCurvePipeGroup"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* HandleBase */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stHandleBase.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stHandleBase.dat", TEXT("ProtoType2stHandleBase"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Hanlde Test */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stHandle.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stHandle.dat", TEXT("ProtoType2stHandle"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* HorizonPipe Group*/
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stHorizonPipeGroup.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stHorizonPipeGroup.dat", TEXT("ProtoType2stHorizonPipeGroup"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Kapak */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stKapak.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stKapak.dat", TEXT("ProtoType2stKapak"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Large Metal Cabinet */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stLargeMetalCabinet.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stLargeMetalCabinet.dat", TEXT("ProtoType2stLargeMetalCabinet"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* PipeGroup1 */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stPipeGroup1.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stPipeGroup1.dat", TEXT("ProtoType2stPipeGroup1"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Plastic Chair */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stPlasticChair.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stPlasticChair.dat", TEXT("ProtoType2stPlasticChair"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Radiator */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stRadiator.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stRadiator.dat", TEXT("ProtoType2stRadiator"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Shelf */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stShelf.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stShelf.dat", TEXT("ProtoType2stShelf"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Stair */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stStair.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stStair.dat", TEXT("ProtoType2stStair"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* StandLamp */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stStandLamp.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stStandLamp.dat", TEXT("ProtoType2stStandLamp"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* StandCabinet */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stStatndCabinet.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stStatndCabinet.dat", TEXT("ProtoType2stStatndCabinet"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Steel Desk */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stSteelDesk.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stSteelDesk.dat", TEXT("ProtoType2stSteelDesk"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Steel Door */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stSteelDoor.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stSteelDoor.dat", TEXT("ProtoType2stSteelDoor"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Switch Board */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stswitchBoard.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stswitchBoard.dat", TEXT("ProtoType2stswitchBoard"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Vertical Pipe */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stVerticalPipe.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stVerticalPipe.dat", TEXT("ProtoType2stVerticalPipe"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Wire Cabel */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stWireCabel.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stWireCabel.dat", TEXT("ProtoType2stWireCabel"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Wood */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stWood.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stWood.dat", TEXT("ProtoType2stWood"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Wood Chair */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stWoodChair.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stWoodChair.dat", TEXT("ProtoType2stWoodChair"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Wood Closet */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stWoodCloset.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stWoodCloset.dat", TEXT("ProtoType2stWoodCloset"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Wood Table */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stWoodTable.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stWoodTable.dat", TEXT("ProtoType2stWoodTable"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Event Steel Door */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stEventSteelDoor.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stEventSteelDoor.dat", TEXT("ProtoType2stEventSteelDoor"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stEventSteelDoorGroup.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stEventSteelDoorGroup.dat", TEXT("ProtoType2stEventSteelDoorGroup"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stEventSteelDoorGroup2.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stEventSteelDoorGroup2.dat", TEXT("ProtoType2stEventSteelDoorGroup2"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Pipe Group 2*/
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\2stPipeGroup2.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\2stPipeGroup2.dat", TEXT("ProtoType2stPipeGroup2"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Cage */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\Cage.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\Cage.dat", TEXT("ProtoTypeCage"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Chain */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\Chain.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\Chain.dat", TEXT("ProtoTypeChain"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Grid Door */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\GridDoor.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\GridDoor.dat", TEXT("ProtoTypeGridDoor"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Motor */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\Motor.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\Motor.dat", TEXT("ProtoTypeMotor"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Ventilator */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\Ventilator.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\Ventilator.dat", TEXT("ProtoTypeVentilator"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Column*/
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\Column.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\Column.dat", TEXT("ProtoTypeColumn"), TEXT("ProtoTypeComponentDefaultMeshShader"));

#pragma endregion 2stProbs

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
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::ANIM, "..\\Binaries\\Resources\\MyModels\\Player\\Player.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeModelsurefire"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Player\\surefire.dat", modelInitializMatrix))))
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
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::ANIM, "..\\Binaries\\Resources\\MyModels\\Player\\Player.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeModelsurefire"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Player\\surefire.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stBottom"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stBottom.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stWall"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stwall.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stSteelDesk"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stSteelDesk.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stDistributionBox"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stDistributionBox.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stCeiling"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stCeiling.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stModularBox"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stModularBox.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stCabinetMetal"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stCabinetMetal.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stCeilingLamp"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stCeilingLamp.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stColumnLamp"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stColumnLamp.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stCurvePipeGroup"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stCurvePipeGroup.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stHandleBase"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stHandleBase.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stHandle"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stHandle.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stHorizonPipeGroup"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stHorizonPipeGroup.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stKapak"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stKapak.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stLargeMetalCabinet"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stLargeMetalCabinet.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stPipeGroup1"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stPipeGroup1.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stPlasticChair"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stPlasticChair.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stRadiator"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stRadiator.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stShelf"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stShelf.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stStair"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stStair.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stStandLamp"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stStandLamp.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stStatndCabinet"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stStatndCabinet.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         //if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stSteelDesk"),
         //    Model::Create(_device, _deviceContext, Model::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stSteelDesk.fbx", modelInitializMatrix))))
         //{
         //    RELEASE_INSTANCE(GameInstance);
         //    return E_FAIL;
         //}

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stSteelDoor"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stSteelDoor.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stswitchBoard"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stswitchBoard.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stVerticalPipe"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stVerticalPipe.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stWireCabel"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stWireCabel.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stWood"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stWood.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stWoodChair"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stWoodChair.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stWoodCloset"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stWoodCloset.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stWoodTable"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stWoodTable.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stEventSteelDoor"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stEventSteelDoor.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }


         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stEventSteelDoorGroup"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stEventSteelDoorGroup.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stEventSteelDoorGroup2"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stEventSteelDoorGroup2.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType2stPipeGroup2"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stPipeGroup2.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeCage"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\Cage.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeChain"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\Chain.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeGridDoor"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\GridDoor.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeMotor"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\Motor.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeVentilator"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\Ventilator.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeColumn"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\Column.dat", modelInitializMatrix))))
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

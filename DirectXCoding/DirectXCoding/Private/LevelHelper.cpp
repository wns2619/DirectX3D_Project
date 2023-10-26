#include "pch.h"
#include "LevelHelper.h"
#include "GameInstance.h"
#include "BackGround.h"
#include "Terrain.h"
#include "PlayerCamera.h"
#include "EditorTerrain.h"
#include "Player.h"
#include "ImguiResourceHandler.h"
#include "ToolCamera.h"
#include "PlayerBody.h"
#include "Surefire.h"
#include "StaticObject.h"
#include "Navigation.h"
#include "DynamicObjectGroup.h"
#include "WallPainting.h"
#include "BodyCam.h"

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
        BinaryNavi::Create(_device, _deviceContext))))
        return E_FAIL;

    if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeStateMachine"),
        StateMachine::Create(_device, _deviceContext))))
        return E_FAIL;

    if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeAnimator"),
        Animator::Create(_device, _deviceContext))))
        return E_FAIL;

    if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeOBBCollider"),
        Collider::Create(_device, _deviceContext, Collider::COLLIDER_TYPE::OBB))))
        return E_FAIL;

    if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeAABBColider"),
        Collider::Create(_device, _deviceContext, Collider::COLLIDER_TYPE::AABB))))
        return E_FAIL;

    if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeSphereColider"),
        Collider::Create(_device, _deviceContext, Collider::COLLIDER_TYPE::SPHERE))))
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

    if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeNavigation"),
        BinaryNavi::Create(_device, _deviceContext))))
        return E_FAIL;

    if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeOBBCollider"),
        Collider::Create(_device, _deviceContext, Collider::COLLIDER_TYPE::OBB))))
        return E_FAIL;

    if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeAABBColider"),
        Collider::Create(_device, _deviceContext, Collider::COLLIDER_TYPE::AABB))))
        return E_FAIL;

    if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeSphereColider"),
        Collider::Create(_device, _deviceContext, Collider::COLLIDER_TYPE::SPHERE))))
        return E_FAIL;

    if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeStateMachine"),
        StateMachine::Create(_device, _deviceContext))))
        return E_FAIL;

    if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeAnimator"),
        Animator::Create(_device, _deviceContext))))
        return E_FAIL;


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
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\Cage.dat", LAYER_TAG::LAYER_PUZZLE, TEXT("ProtoTypeCage"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\Cage.dat", TEXT("ProtoTypeCage"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Chain */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\Chain.dat", LAYER_TAG::LAYER_DYNAMIC, TEXT("ProtoTypeCeilingChain"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\Chain.dat", TEXT("ProtoTypeChain"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Grid Door */
    //ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\GridDoor.dat", LAYER_TAG::LAYER_DYNAMIC, TEXT("ProtoTypeOldSteelGridMainWithKey"));
    //ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\GridDoor.dat", TEXT("ProtoTypeGridDoor"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Motor */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\Motor.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\Motor.dat", TEXT("ProtoTypeMotor"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Ventilator */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\Ventilator.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\Ventilator.dat", TEXT("ProtoTypeVentilator"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Column*/
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\Column.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\Column.dat", TEXT("ProtoTypeColumn"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Drum */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\Drum.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeDrum"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\Drum.dat", TEXT("ProtoTypeDrum"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* CoatRack */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\CoatRack.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\CoatRack.dat", TEXT("ProtoTypeCoatRack"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Basement Wall*/
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\BasementWall.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\BasementWall.dat", TEXT("ProtoTypeBasementWall"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* BasementDesk */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\BasementDesk.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\BasementDesk.dat", TEXT("ProtoTypeBasementDesk"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Electrical Box*/
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\electricalBox.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\electricalBox.dat", TEXT("ProtoTypeelectricalBox"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* OldSteelGridDoor */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\OldSteelGridDoor.dat", LAYER_TAG::LAYER_DYNAMIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\OldSteelGridDoor.dat", TEXT("ProtoTypeOldSteelGridDoor"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Supply Stands*/
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\supplyStands.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\supplyStands.dat", TEXT("ProtoTypesupplyStands"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* 1st Bed */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\1stBed.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\1stBed.dat", TEXT("ProtoType1stBed"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Basement Bottom*/
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\BasementBottom.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\BasementBottom.dat", TEXT("ProtoTypeBasementBottom"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Basement Open Door */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\basementOpenDoor.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\basementOpenDoor.dat", TEXT("ProtoTypebasementOpenDoor"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* OldBookShelf */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Basement\\OldBookShelf.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Basement\\OldBookShelf.dat", TEXT("ProtoTypeOldBookShelf"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* WasteGroup */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Basement\\WasteGroup.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Basement\\WasteGroup.dat", TEXT("ProtoTypeWasteGroup"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* BasementChain*/
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Basement\\BasementChain.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Basement\\BasementChain.dat", TEXT("ProtoTypeBasementChain"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* BasementWoodDoorFrame */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Basement\\BasementWoodDoorFrame.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Basement\\BasementWoodDoorFrame.dat", TEXT("ProtoTypeBasementWoodDoorFrame"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* BasementWoodDoorMain*/
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Basement\\BasementWoodDoorMain.dat", LAYER_TAG::LAYER_DYNAMIC, TEXT("ProtoTypeOldWoodDoor"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Basement\\BasementWoodDoorMain.dat", TEXT("ProtoTypeBasementWoodDoorMain"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* DoorFrame */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Basement\\DoorFrame.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Basement\\DoorFrame.dat", TEXT("ProtoTypeDoorFrame"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Door Piece*/
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Basement\\DoorPiece.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Basement\\DoorPiece.dat", TEXT("ProtoTypeDoorPiece"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Main Door */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Basement\\MainDoor.dat", LAYER_TAG::LAYER_DYNAMIC, TEXT("ProtoTypeBreakDoor"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Basement\\MainDoor.dat", TEXT("ProtoTypeMainDoor"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Valve Handler */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Basement\\ValveHandler.dat", LAYER_TAG::LAYER_DYNAMIC, TEXT("ProtoTypeValve"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Basement\\ValveHandler.dat", TEXT("ProtoTypeValveHandler"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* BasementCeiling */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\BasementCeiling.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\BasementCeiling.dat", TEXT("ProtoTypeBasementCeiling"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* HorrorMask */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\HorrorMask.dat", LAYER_TAG::LAYER_DYNAMIC, TEXT("ProtoTypeHorrorMask"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\HorrorMask.dat", TEXT("ProtoTypeHorrorMask"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* CoatRackBar */
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\CoatRackBar.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\CoatRackBar.dat", TEXT("ProtoTypeCoatRackBar"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Old Grid Steel Frame*/
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\OldSteelGridFrame.dat", LAYER_TAG::LAYER_STATIC, TEXT("ProtoTypeStaticObject"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\OldSteelGridFrame.dat", TEXT("ProtoTypeOldSteelGridFrame"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Old Grid Steel Main*/
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\OldSteelGridMainWithKey.dat", LAYER_TAG::LAYER_DYNAMIC, TEXT("ProtoTypeOldSteelGridMainWithKey"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\2stprob\\OldSteelGridMainWithKey.dat", TEXT("ProtoTypeOldSteelGridMainWithKey"), TEXT("ProtoTypeComponentDefaultMeshShader"));
    /* Graffiti List */

    // Evil
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Graffiti\\EvilGraffiti.dat", LAYER_TAG::LAYER_ENVIRONMENT, TEXT("ProtoWallPaint"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Graffiti\\EvilGraffiti.dat", TEXT("ProtoTypeEvilGraffiti"), TEXT("ProtoTypeComponentShaderVertexTextureData"));

    // Hip
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Graffiti\\HipGraffiti.dat", LAYER_TAG::LAYER_ENVIRONMENT, TEXT("ProtoWallPaint"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Graffiti\\HipGraffiti.dat", TEXT("ProtoTypeHipGraffiti"), TEXT("ProtoTypeComponentShaderVertexTextureData"));

    // Human
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Graffiti\\HumanGraffiti.dat", LAYER_TAG::LAYER_ENVIRONMENT, TEXT("ProtoWallPaint"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Graffiti\\HumanGraffiti.dat", TEXT("ProtoTypeHumanGraffiti"), TEXT("ProtoTypeComponentShaderVertexTextureData"));

    // Shoot
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Graffiti\\ShootGraffiti.dat", LAYER_TAG::LAYER_ENVIRONMENT, TEXT("ProtoWallPaint"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Graffiti\\ShootGraffiti.dat", TEXT("ProtoTypeShootGraffiti"), TEXT("ProtoTypeComponentShaderVertexTextureData"));

    // Way
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Graffiti\\WayGraffiti.dat", LAYER_TAG::LAYER_ENVIRONMENT, TEXT("ProtoWallPaint"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Graffiti\\WayGraffiti.dat", TEXT("ProtoTypeWayGraffiti"), TEXT("ProtoTypeComponentShaderVertexTextureData"));

    // Graffiti
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle1.dat", LAYER_TAG::LAYER_ENVIRONMENT, TEXT("ProtoWallPaint"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle1.dat", TEXT("ProtoTypeWayGraffiti"), TEXT("ProtoTypeComponentShaderVertexTextureData"));
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle2.dat", LAYER_TAG::LAYER_ENVIRONMENT, TEXT("ProtoWallPaint"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle2.dat", TEXT("ProtoTypeWayGraffiti"), TEXT("ProtoTypeComponentShaderVertexTextureData"));
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle3.dat", LAYER_TAG::LAYER_ENVIRONMENT, TEXT("ProtoWallPaint"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle3.dat", TEXT("ProtoTypeWayGraffiti"), TEXT("ProtoTypeComponentShaderVertexTextureData"));
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle4.dat", LAYER_TAG::LAYER_ENVIRONMENT, TEXT("ProtoWallPaint"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle4.dat", TEXT("ProtoTypeWayGraffiti"), TEXT("ProtoTypeComponentShaderVertexTextureData"));
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle5.dat", LAYER_TAG::LAYER_ENVIRONMENT, TEXT("ProtoWallPaint"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle5.dat", TEXT("ProtoTypeWayGraffiti"), TEXT("ProtoTypeComponentShaderVertexTextureData"));
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle6.dat", LAYER_TAG::LAYER_ENVIRONMENT, TEXT("ProtoWallPaint"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle6.dat", TEXT("ProtoTypeWayGraffiti"), TEXT("ProtoTypeComponentShaderVertexTextureData"));
    ImGuiResourceHandler::GetInstance()->AddProtoFilePath("..\\Binaries\\Resources\\MyModels\\2stprob\\Puddle7.dat", LAYER_TAG::LAYER_ENVIRONMENT, TEXT("ProtoWallPaint"));
    ImGuiResourceHandler::GetInstance()->AddProtoComponentName("..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle7.dat", TEXT("ProtoTypeWayGraffiti"), TEXT("ProtoTypeComponentShaderVertexTextureData"));

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
        break;
    case Client::LEVEL::GAME:
    {
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

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stBottom"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stBottom.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stWall"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stwall.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stSteelDesk"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stSteelDesk.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stDistributionBox"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stDistributionBox.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stCeiling"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stCeiling.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stModularBox"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stModularBox.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stCabinetMetal"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stCabinetMetal.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stCeilingLamp"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stCeilingLamp.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stColumnLamp"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stColumnLamp.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stCurvePipeGroup"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stCurvePipeGroup.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stHandleBase"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stHandleBase.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stHandle"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stHandle.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stHorizonPipeGroup"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stHorizonPipeGroup.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stKapak"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stKapak.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stLargeMetalCabinet"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stLargeMetalCabinet.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stPipeGroup1"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stPipeGroup1.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stPlasticChair"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stPlasticChair.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stRadiator"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stRadiator.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stShelf"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stShelf.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stStair"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stStair.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stStandLamp"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stStandLamp.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stStatndCabinet"),
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

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stSteelDoor"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stSteelDoor.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stswitchBoard"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stswitchBoard.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stVerticalPipe"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stVerticalPipe.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stWireCabel"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stWireCabel.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stWood"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stWood.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stWoodChair"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stWoodChair.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stWoodCloset"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stWoodCloset.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stWoodTable"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stWoodTable.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stEventSteelDoor"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stEventSteelDoor.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }


        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stEventSteelDoorGroup"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stEventSteelDoorGroup.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stEventSteelDoorGroup2"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stEventSteelDoorGroup2.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType2stPipeGroup2"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\2stPipeGroup2.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeCage"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\Cage.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeChain"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\Chain.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeGridDoor"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\GridDoor.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeMotor"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\Motor.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeVentilator"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\Ventilator.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeColumn"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\Column.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeDrum"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\Drum.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeCoatRack"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\CoatRack.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeBasementWall"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\BasementWall.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeBasementDesk"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\BasementDesk.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeelectricalBox"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\electricalBox.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeOldSteelGridDoor"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\OldSteelGridDoor.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypesupplyStands"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\supplyStands.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeBasementBottom"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\BasementBottom.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoType1stBed"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\1stBed.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypebasementOpenDoor"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\basementOpenDoor.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeOldBookShelf"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Basement\\OldBookShelf.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeWasteGroup"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Basement\\WasteGroup.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeBasementChain"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Basement\\BasementChain.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeBasementWoodDoorFrame"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Basement\\BasementWoodDoorFrame.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        // Dynamic 으로 바꿀거임.
        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeBasementWoodDoorMain"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Basement\\BasementWoodDoorMain.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeDoorFrame"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Basement\\DoorFrame.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeDoorPiece"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Basement\\DoorPiece.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }


        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeMainDoor"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Basement\\MainDoor.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        // 이것도 다이나믹으로 바꿀거임.
        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeValveHandler"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Basement\\ValveHandler.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeBasementCeiling"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\BasementCeiling.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeCoatRackBar"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\CoatRackBar.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeHorrorMask"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\HorrorMask.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }


        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeOldSteelGridMainWithKey"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\OldSteelGridMainWithKey.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }


        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeOldSteelGridFrame"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\OldSteelGridFrame.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeEvilGraffiti"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\EvilGraffiti.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeHipGraffiti"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\HipGraffiti.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeHumanGraffiti"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\HumanGraffiti.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeShootGraffiti"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\ShootGraffiti.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeWayGraffiti"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\WayGraffiti.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypePuddle1"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle1.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypePuddle2"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle2.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypePuddle3"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle3.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypePuddle4"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle4.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypePuddle5"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle5.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypePuddle6"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle6.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypePuddle7"),
            BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle7.dat", modelInitializMatrix))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }
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

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeDrum"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\Drum.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeCoatRack"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\CoatRack.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeBasementWall"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\BasementWall.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeBasementDesk"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\BasementDesk.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeelectricalBox"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\electricalBox.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeOldSteelGridDoor"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\OldSteelGridDoor.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypesupplyStands"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\supplyStands.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeBasementBottom"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\BasementBottom.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoType1stBed"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\1stBed.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypebasementOpenDoor"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\basementOpenDoor.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeOldBookShelf"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Basement\\OldBookShelf.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeWasteGroup"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Basement\\WasteGroup.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeBasementChain"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Basement\\BasementChain.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeBasementWoodDoorFrame"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Basement\\BasementWoodDoorFrame.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         // Dynamic 으로 바꿀거임.
         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeBasementWoodDoorMain"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Basement\\BasementWoodDoorMain.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeDoorFrame"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Basement\\DoorFrame.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeDoorPiece"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Basement\\DoorPiece.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }


         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeMainDoor"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Basement\\MainDoor.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         // 이것도 다이나믹으로 바꿀거임.
         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeValveHandler"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Basement\\ValveHandler.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeBasementCeiling"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\BasementCeiling.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeCoatRackBar"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\CoatRackBar.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeHorrorMask"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\HorrorMask.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }


         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeOldSteelGridMainWithKey"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\OldSteelGridMainWithKey.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }


         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeOldSteelGridFrame"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\2stprob\\OldSteelGridFrame.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeEvilGraffiti"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\EvilGraffiti.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeHipGraffiti"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\HipGraffiti.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeHumanGraffiti"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\HumanGraffiti.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeShootGraffiti"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\ShootGraffiti.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeWayGraffiti"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\WayGraffiti.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypePuddle1"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle1.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypePuddle2"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle2.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypePuddle3"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle3.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypePuddle4"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle4.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypePuddle5"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle5.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypePuddle6"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle6.dat", modelInitializMatrix))))
         {
             RELEASE_INSTANCE(GameInstance);
             return E_FAIL;
         }

         if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypePuddle7"),
             BinaryModel::Create(_device, _deviceContext, BinaryModel::MODEL_TYPE::NONE, "..\\Binaries\\Resources\\MyModels\\Graffiti\\Puddle7.dat", modelInitializMatrix))))
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

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::GAME), TEXT("ProtoTypeComponentShaderVertexTextureData"),
            Shader::Create(_device, _deviceContext, TEXT("../Binaries/Shaders/Shader_VtxPosTex.fx"), VertexTextureNormalData::Elements,
                VertexTextureNormalData::numElements))))
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

        if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::EDIT), TEXT("ProtoTypeComponentShaderVertexTextureData"),
            Shader::Create(_device, _deviceContext, TEXT("../Binaries/Shaders/Shader_VtxPosTex.fx"), VertexTextureNormalData::Elements,
                VertexTextureNormalData::numElements))))
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

        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeGameObjectBodyCam"),
            BodyCam::Create(_device, _deviceContext))))
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

        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeBreakDoor"),
            BreakDoor::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeHorrorMask"),
            HorrorMask::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        // 
        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeCeilingChain"),
            CeilingChain::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        // OK
        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeDrum"),
            Drum::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        // OK
        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeOldWoodDoor"),
            OldWoodDoor::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        // FAILED
        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeOldSteelGridMainWithKey"),
            GridDoor::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        // OK
        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeValve"),
            Valve::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoWallPaint"),
            WallPainting::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeCage"),
            Cage::Create(_device, _deviceContext))))
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

        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeBreakDoor"),
            BreakDoor::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeHorrorMask"),
            HorrorMask::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        // 
        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeCeilingChain"),
            CeilingChain::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        // OK
        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeDrum"),
            Drum::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        // OK
        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeOldWoodDoor"),
            OldWoodDoor::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        // FAILED
        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeOldSteelGridMainWithKey"),
            GridDoor::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        // OK
        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeValve"),
            Valve::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoWallPaint"),
            WallPainting::Create(_device, _deviceContext))))
        {
            RELEASE_INSTANCE(GameInstance);
            return E_FAIL;
        }

        if (FAILED(gameInstance->AddProtoType(TEXT("ProtoTypeCage"),
            Cage::Create(_device, _deviceContext))))
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

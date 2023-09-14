#include "pch.h"
#include "LevelHelper.h"
#include "GameInstance.h"
#include "BackGround.h"

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
    }


    LeaveCriticalSection(&_criticalSection);
    
    if (FAILED(hr))
        return -1;

    return 0;
}

HRESULT LevelHelper::LodingforLevelLogo()
{
    GameInstance* gameInstance = GameInstance::GetInstance();
    Safe_AddRef<GameInstance*>(gameInstance);

    _title = TEXT("Texture Loading");

    if (FAILED(gameInstance->AddProtoType(static_cast<uint32>(LEVEL::LOGO), TEXT("ProtoTypeComponentTextureBackGround"),
        Texture::Create(_device, _deviceContext, TEXT("../Binaries/Resources/Textures/Default%d.jpg"), 2))))
        return E_FAIL;

    _title = TEXT("Mesh Loading");
    _title = TEXT("Shader Loading");


    _title = TEXT("Object Loading");
    
    if (FAILED(gameInstance->AddProtoType(TEXT("PrototypeBackGround"), BackGround::Create(_device, _deviceContext))))
        return E_FAIL;

    Safe_Release<GameInstance*>(gameInstance);

    _title = TEXT("Loading Successed");
    _IsFinished = true;

    return S_OK;
}

HRESULT LevelHelper::LodingforLevelGame()
{
    _title = TEXT("Texture Loading");

    _title = TEXT("Mesh Loading");

    _title = TEXT("Shader Loading");

    _title = TEXT("Loading Successed");
    _IsFinished = true;

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

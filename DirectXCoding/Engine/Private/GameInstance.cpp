#include "pch.h"
#include "GameInstance.h"
#include "TimeManager.h"
#include "GraphicsManager.h"
#include "LevelManager.h"
#include "ObjectManager.h"

IMPLEMENT_SINGLETON(GameInstance)

GameInstance::GameInstance()
    : _timeManager(TimeManager::GetInstance()), _graphicManager(GraphicsManager::GetInstance()), 
    _levelManager(LevelManager::GetInstance()), _objectManager(ObjectManager::GetInstance()),
    _componentManager(ComponentManager::GetInstance()), _cameraHelper(CameraHelper::GetInstance())
    , _inputManager(InputManager::GetInstance())
{
    Safe_AddRef<InputManager*>(_inputManager);
    Safe_AddRef<CameraHelper*>(_cameraHelper);
    Safe_AddRef<ComponentManager*>(_componentManager);
    Safe_AddRef<ObjectManager*>(_objectManager);
    Safe_AddRef<LevelManager*>(_levelManager);
    Safe_AddRef<TimeManager*>(_timeManager);
    Safe_AddRef<GraphicsManager*>(_graphicManager);
}

HRESULT GameInstance::Initialize_Engine(uint32 levelNumbers, HINSTANCE instance, const GRAPHIC_DESC& graphicDesc, _Inout_ ID3D11Device** device, _Inout_ ID3D11DeviceContext** deviceContext)
{
    if (FAILED(_graphicManager->Initialize(graphicDesc, device, deviceContext)))
        return E_FAIL;

    if (FAILED(_inputManager->Ready_Input_Device(instance, graphicDesc._hwnd)))
        return E_FAIL;

    if (FAILED(_objectManager->ReserveManager(levelNumbers)))
        return E_FAIL;

    if (FAILED(_componentManager->ReserveManager(levelNumbers)))
        return E_FAIL;

    return S_OK;
}

void GameInstance::Tick(_float fTimeDelta)
{
    _inputManager->Tick();

    _objectManager->Tick(fTimeDelta);
    _levelManager->Tick(fTimeDelta);

    _cameraHelper->Tick();

    _objectManager->LateTick(fTimeDelta);
    _levelManager->LateTick(fTimeDelta);
}

void GameInstance::Clear(uint32 levelIndex)
{
    _objectManager->Clear(levelIndex);
}

_float GameInstance::ComputeTimeDelta(const wstring& timerTag)
{
    if (nullptr == _timeManager)
        return static_cast<_float>(E_FAIL);

    return _timeManager->Compute_TimeDelta(timerTag);
}

HRESULT GameInstance::AddTimer(const wstring& timerTag)
{
    if (nullptr == _timeManager)
        return E_FAIL;

    return _timeManager->Add_Timer(timerTag);
}

HRESULT GameInstance::ClearBackBufferView(Color color)
{
    if (nullptr == _graphicManager)
        return E_FAIL;

    return _graphicManager->ClearBackBuffer(color);
}

HRESULT GameInstance::ClearDepthStencilView()
{
    if (nullptr == _graphicManager)
        return E_FAIL;

    return _graphicManager->ClearDepthStencil();
}

HRESULT GameInstance::Present()
{
    if (nullptr == _graphicManager)
        return E_FAIL;

    return _graphicManager->Present();
}

_byte GameInstance::Get_DIKeyState(_ubyte byKeyID)
{
    if (nullptr == _inputManager)
        return 0;

    return _inputManager->Get_DIKeyState(byKeyID);
}

_byte GameInstance::Get_DIMouseState(InputManager::MOUSEKEYSTATE eMouse)
{
    if (nullptr == _inputManager)
        return 0;

    return _inputManager->Get_DIMouseState(eMouse);
}

_long GameInstance::Get_DIMouseMove(InputManager::MOUSEMOVESTATE eMouseState)
{
    return _inputManager->Get_DIMouseMove(eMouseState);
}

HRESULT GameInstance::OpenLevel(uint32 levelIndex, Level* newLevel)
{
    if (nullptr == _levelManager)
        return E_FAIL;

    return _levelManager->OpenLevel(levelIndex, newLevel);
}

uint32 GameInstance::GetCurrentLevelIndex()
{
    if (nullptr == _levelManager)
        return 0;

    return _levelManager->GetCurrentLevelIndex();
}

HRESULT GameInstance::AddProtoType(const wstring& protoTypeTag, GameObject* prototype)
{
    if (nullptr == _objectManager)
        return E_FAIL;

    return _objectManager->AddPrototype(protoTypeTag, prototype);
}

HRESULT GameInstance::AddGameObject(uint32 levelIndex, const wstring& layerTag, const wstring& prototypeTag, void* argument)
{
    if (nullptr == _objectManager)
        return E_FAIL;

    return _objectManager->AddGameObject(levelIndex, layerTag, prototypeTag, argument);
}

HRESULT GameInstance::AddProtoType(uint32 levelIndex, const wstring& PrototypeTag, Component* protoType)
{
    if (nullptr == _componentManager)
        return E_FAIL;


    return _componentManager->AddProtoType(levelIndex, PrototypeTag, protoType);
}

Component* GameInstance::CloneComponent(uint32 levelIndex, const wstring& protoTypeTag, void* argument)
{
    if (nullptr == _componentManager)
        return nullptr;

    return _componentManager->CloneComponent(levelIndex, protoTypeTag, argument);
}

HRESULT GameInstance::BindTransformToShader(Shader* shader, const char* constantName, CameraHelper::TRANSFORMSTATE state)
{
    return _cameraHelper->BindTransformToShader(shader, constantName, state);
}

void GameInstance::Release_Engine()
{
    InputManager::GetInstance()->DestroyInstance();
    CameraHelper::GetInstance()->DestroyInstance();
    GameInstance::GetInstance()->DestroyInstance();
    ObjectManager::GetInstance()->DestroyInstance();
    LevelManager::GetInstance()->DestroyInstance();
    ComponentManager::GetInstance()->DestroyInstance();
    TimeManager::GetInstance()->DestroyInstance();
    GraphicsManager::GetInstance()->DestroyInstance();
}

void GameInstance::Free()
{
    Safe_Release<InputManager*>(_inputManager);
    Safe_Release<CameraHelper*>(_cameraHelper);
    Safe_Release<ComponentManager*>(_componentManager);
    Safe_Release<ObjectManager*>(_objectManager);
    Safe_Release<LevelManager*>(_levelManager);
    Safe_Release<GraphicsManager*>(_graphicManager);
    Safe_Release<TimeManager*>(_timeManager);
}

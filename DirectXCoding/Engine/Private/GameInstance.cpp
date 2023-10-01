#include "pch.h"
#include "GameInstance.h"
#include "TimeManager.h"
#include "GraphicsManager.h"
#include "LevelManager.h"
#include "ObjectManager.h"
#include "Picking.h"

IMPLEMENT_SINGLETON(GameInstance)

GameInstance::GameInstance()
    : _timeManager(TimeManager::GetInstance()), _graphicManager(GraphicsManager::GetInstance()), 
    _levelManager(LevelManager::GetInstance()), _objectManager(ObjectManager::GetInstance()),
    _componentManager(ComponentManager::GetInstance()), _cameraHelper(CameraHelper::GetInstance())
    , _inputManager(InputManager::GetInstance()), _lightManager(LightManager::GetInstance())
    , _picking(Picking::GetInstance())
{
    Safe_AddRef<InputManager*>(_inputManager);
    Safe_AddRef<Picking*>(_picking);
    Safe_AddRef<CameraHelper*>(_cameraHelper);
    Safe_AddRef<LightManager*>(_lightManager);
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

    if (FAILED(_lightManager->ReserveManager(levelNumbers)))
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
    //_componentManager->Clear(levelIndex);
    _lightManager->Clear(levelIndex);
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

Viewport& GameInstance::GetViewPort()
{
    return _graphicManager->GetViewPort();
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

GameObject* GameInstance::GetLayerObject(const wstring& layertag, OBJECT_TYPE type)
{
    if (nullptr == _objectManager)
        return nullptr;

    return _objectManager->GetLayerObject(layertag, type);
}

HRESULT GameInstance::ComparisonAddObject(int32 levelIndex, const string& addObjectfile, const wstring& layertag, void* argument)
{
    if (nullptr == _objectManager)
        return E_FAIL;

    return _objectManager->ComparisonAddObject(levelIndex, addObjectfile, layertag, argument);
}

int32 GameInstance::GetLayerObjectCount()
{
    if (nullptr == _objectManager)
        return 0;

    return _objectManager->GetLayerObjectCount();
}

vector<GameObject*>* GameInstance::GetCurrentObjectList(wstring& layerTag)
{
    return _objectManager->GetCurrentObjectList(layerTag);
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
    if (nullptr == _cameraHelper)
        return E_FAIL;

    return _cameraHelper->BindTransformToShader(shader, constantName, state);
}

HRESULT GameInstance::BindCameraPosition(Shader* shader, const _char* constantName, uint32 legnth)
{
    if (nullptr == _cameraHelper)
        return E_FAIL;

    return _cameraHelper->BindCameraPosition(shader, constantName,legnth);
}

void GameInstance::SetTransform(CameraHelper::TRANSFORMSTATE state, FXMMATRIX trasnsformMatrix)
{
    if (nullptr == _cameraHelper)
        return;

    return _cameraHelper->SetTransform(state, trasnsformMatrix);
}

Matrix GameInstance::GetTransform(CameraHelper::TRANSFORMSTATE state) const
{
    if (nullptr == _cameraHelper)
        return ::XMMatrixIdentity();

    return _cameraHelper->GetTransform(state);
}

FXMMATRIX GameInstance::GetTransformCalculator(CameraHelper::TRANSFORMSTATE state) const
{
    if (nullptr == _cameraHelper)
        return FXMMATRIX();


    return _cameraHelper->GetTransformCalculator(state);
}

Matrix GameInstance::GetInverseTransform(CameraHelper::TRANSFORMSTATE state) const
{
    if (nullptr == _cameraHelper)
        return ::XMMatrixIdentity();

    return _cameraHelper->GetInverseTransform(state);
}

FXMMATRIX GameInstance::GetInverseTransformCalculator(CameraHelper::TRANSFORMSTATE state) const
{
    if (nullptr == _cameraHelper)
        return FXMMATRIX();

    return _cameraHelper->GetInverseTransformCalculator(state);
}

Vec4 GameInstance::GetCameraPosition() const
{
    if (nullptr == _cameraHelper)
        return Vec4();

    return _cameraHelper->GetCameraPosition();
}

HRESULT GameInstance::AddLightProtoType(uint32 levelIndex, Light::LightType type, const wstring& lighttag, Component* prototype)
{
    if (nullptr == _lightManager)
        return E_FAIL;

    _lightManager->AddLightProtoType(levelIndex, type, lighttag, prototype);

    return S_OK;
}

Component* GameInstance::CloneLight(uint32 levelIndex, Light::LightType type, const wstring& lighttag, void* argument)
{
    if(nullptr == _lightManager)
        return nullptr;

    return _lightManager->CloneLight(levelIndex, type, lighttag, argument);
}

_bool GameInstance::TerrainPicking(POINT pt, Vec3& pickPos, _float& distance, Transform* trans, VIBufferTerrain* buffer)
{
    if (nullptr == _picking)
        return false;

    return _picking->GetInstance()->TerrainPicking(pt, pickPos, distance, trans, buffer);
}

_bool GameInstance::PickObject(POINT pt)
{
    if (nullptr == _picking)
        return false;

    return _picking->PickObject(pt);
}

XMVECTOR GameInstance::GetCameraCaculator() const
{
    if(nullptr == _cameraHelper)
        return XMVECTOR();

    return _cameraHelper->GetCameraCaculator();
}

void GameInstance::Release_Engine()
{
    InputManager::GetInstance()->DestroyInstance();
    Picking::GetInstance()->DestroyInstance();
    CameraHelper::GetInstance()->DestroyInstance();
    GameInstance::GetInstance()->DestroyInstance();
    ObjectManager::GetInstance()->DestroyInstance();
    LevelManager::GetInstance()->DestroyInstance();
    LightManager::GetInstance()->DestroyInstance();
    ComponentManager::GetInstance()->DestroyInstance();
    TimeManager::GetInstance()->DestroyInstance();
    GraphicsManager::GetInstance()->DestroyInstance();
}

void GameInstance::Free()
{
    Safe_Release<InputManager*>(_inputManager);
    Safe_Release<Picking*>(_picking);
    Safe_Release<CameraHelper*>(_cameraHelper);
    Safe_Release<LightManager*>(_lightManager);
    Safe_Release<ComponentManager*>(_componentManager);
    Safe_Release<ObjectManager*>(_objectManager);
    Safe_Release<LevelManager*>(_levelManager);
    Safe_Release<GraphicsManager*>(_graphicManager);
    Safe_Release<TimeManager*>(_timeManager);
}

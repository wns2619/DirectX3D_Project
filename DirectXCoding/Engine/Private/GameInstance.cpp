#include "pch.h"
#include "GameInstance.h"
#include "TimeManager.h"
#include "GraphicsManager.h"
#include "LevelManager.h"
#include "Picking.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "SoundManager.h"
#include "RenderTargetManager.h"
#include "FontManager.h"
#include "Frustum.h"

IMPLEMENT_SINGLETON(GameInstance)

GameInstance::GameInstance()
    : _timeManager(TimeManager::GetInstance()), _graphicManager(GraphicsManager::GetInstance()), 
    _levelManager(LevelManager::GetInstance()), _objectManager(ObjectManager::GetInstance()),
    _componentManager(ComponentManager::GetInstance()), _cameraHelper(CameraHelper::GetInstance())
    , _inputManager(InputManager::GetInstance()), _lightManager(LightManager::GetInstance())
    , _picking(Picking::GetInstance()), _inputHandler(InputHandler::GetInstance()), _collisionManager(CollisionManager::GetInstance())
    , _pEventManager(EventManager::GetInstance()), _pTargetManager(RenderTargetManager::GetInstance())
    , _pSoundManager(SoundManager::GetInstance()), _pFontManager(FontManager::GetInstance())
    , _pFrustum(Frustum::GetInstance())
{
    Safe_AddRef<FontManager*>(_pFontManager);
    Safe_AddRef<RenderTargetManager*>(_pTargetManager);
    Safe_AddRef<SoundManager*>(_pSoundManager);
    Safe_AddRef<InputHandler*>(_inputHandler);
    Safe_AddRef<InputManager*>(_inputManager);
    Safe_AddRef<Picking*>(_picking);
    Safe_AddRef<CameraHelper*>(_cameraHelper);
    Safe_AddRef<Frustum*>(_pFrustum);
    Safe_AddRef<LightManager*>(_lightManager);
    Safe_AddRef<ComponentManager*>(_componentManager);
    Safe_AddRef<ObjectManager*>(_objectManager);
    Safe_AddRef<EventManager*>(_pEventManager);
    Safe_AddRef<CollisionManager*>(_collisionManager);
    Safe_AddRef<LevelManager*>(_levelManager);
    Safe_AddRef<TimeManager*>(_timeManager);
    Safe_AddRef<GraphicsManager*>(_graphicManager);
}

HRESULT GameInstance::Initialize_Engine(uint32 levelNumbers, HINSTANCE instance, const GRAPHIC_DESC& graphicDesc, _Inout_ ID3D11Device** device, _Inout_ ID3D11DeviceContext** deviceContext)
{
    if (FAILED(_graphicManager->Initialize(graphicDesc, device, deviceContext)))
        return E_FAIL;

    if (FAILED(_inputManager->Initialize(instance, graphicDesc._hwnd)))
        return E_FAIL;

    if (FAILED(_objectManager->ReserveManager(levelNumbers)))
        return E_FAIL;

    if (FAILED(_componentManager->ReserveManager(levelNumbers)))
        return E_FAIL;

    if (FAILED(_pFrustum->Initialize()))
        return E_FAIL;

    _pSoundManager->ReadySound();
        


    return S_OK;
}

void GameInstance::Tick(_float fTimeDelta)
{
    _inputManager->Tick();

    _objectManager->PriorityTick(fTimeDelta);

    _objectManager->Tick(fTimeDelta);
    _levelManager->Tick(fTimeDelta);

    _cameraHelper->Tick();
    _pFrustum->Tick();

    _objectManager->LateTick(fTimeDelta);
    _collisionManager->LateTick(fTimeDelta);
    _levelManager->LateTick(fTimeDelta);
}

void GameInstance::Clear(uint32 levelIndex)
{
    _objectManager->Clear(levelIndex);
    _componentManager->Clear(levelIndex);
    //_lightManager->Clear(levelIndex);
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

    return _inputManager->Get_DIKState(byKeyID);
}

_byte GameInstance::Get_DIMouseState(DIMK eMouse)
{
    if (nullptr == _inputManager)
        return 0;

    return _inputManager->Get_DIMKeyState(eMouse);
}

_long GameInstance::Get_DIMouseMove(DIMM eMouseState)
{
    return _inputManager->Get_DIMMoveState(eMouseState);
}

_bool GameInstance::keyDown(_byte byKeyID)
{
    return _inputManager->keyDown(byKeyID);
}

_bool GameInstance::KeyPressing(_byte byKeyID)
{
    return _inputManager->keyPressing(byKeyID);
}

_bool GameInstance::KeyUp(_byte byKeyID)
{
    return _inputManager->keyUp(byKeyID);
}

_bool GameInstance::mouseDown(DIMK eMouseKeyID)
{
    return _inputManager->mouseDown(eMouseKeyID);
}

_bool GameInstance::mousePreesing(DIMK eMouseKeyID)
{
    return _inputManager->mousePressing(eMouseKeyID);
}

_bool GameInstance::mouseUp(DIMK eMouseKeyID)
{
    return _inputManager->mouseUp(eMouseKeyID);
}

InputHandler* GameInstance::GetInputHandler()
{
    if (nullptr == _inputHandler)
        return nullptr;

    return _inputHandler->GetInstance();
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

HRESULT GameInstance::AddGameObject(uint32 levelIndex, const LAYER_TAG layerTag, const wstring& prototypeTag, void* argument)
{
    if (nullptr == _objectManager)
        return E_FAIL;

    return _objectManager->AddGameObject(levelIndex, layerTag, prototypeTag, argument);
}

vector<GameObject*>& GameInstance::GetLayerObject(const LAYER_TAG layertag)
{
    return _objectManager->GetLayerObject(layertag);
}

GameObject* GameInstance::GetLayerObjectTag(const LAYER_TAG layerag, const string& modelname)
{
    if (nullptr == _objectManager)
        return nullptr;

    return _objectManager->GetLayerObjectTag(layerag, modelname);
}

HRESULT GameInstance::ComparisonAddObject(int32 levelIndex, const string& addObjectfile, const LAYER_TAG layertag, void* argument)
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

vector<GameObject*>* GameInstance::GetCurrentObjectList(LAYER_TAG layerTag)
{
    return _objectManager->GetCurrentObjectList(layerTag);
}

HRESULT GameInstance::DeleteGameObject(uint32 levelIndex, const LAYER_TAG layertag, const uint32 objectNumber, const string& modelNames)
{
    if (nullptr == _objectManager)
        return E_FAIL;

    return _objectManager->DeleteObject(levelIndex, layertag, objectNumber, modelNames);
}

map<const LAYER_TAG, Layer*>* GameInstance::GetEntireObjectLayer()
{
    return _objectManager->GetEntireObjectLayer();
}

int32 GameInstance::GetEntireLevel()
{
    if (nullptr == _objectManager)
        return -1;

    return _objectManager->GetEntireLevel();
}

GameObject* GameInstance::CloneGameObject(const wstring& strPrototypeTag, void* pArg)
{
    if (nullptr == _objectManager)
        return nullptr;

    return _objectManager->CloneGameObject(strPrototypeTag, pArg);
}

Component* GameInstance::GetComponent(uint32 iLevelIndex, const LAYER_TAG eLayerTag, const wstring& strComponentTag, const string& strModelName, uint32 iIndex)
{
    if (nullptr == _objectManager)
        return nullptr;

    return _objectManager->GetComponent(iLevelIndex, eLayerTag, strComponentTag, strModelName, iIndex);
}

Component* GameInstance::GetComponentFromID(uint32 iLevelIndex, const LAYER_TAG tag, const wstring& strComponentTag, const string& modelName, uint32 id)
{
    if (nullptr == _objectManager)
        return nullptr;


    return _objectManager->GetComponentFromID(iLevelIndex, tag, strComponentTag, modelName, id);
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

const LIGHT_DESC* GameInstance::GetLightDesc(uint32 lightIndex)
{
    if (nullptr == _lightManager)
        return nullptr;

    return _lightManager->getLightDesc(lightIndex);
}

HRESULT GameInstance::AddLight(const LIGHT_DESC& lightdesc)
{
    if (nullptr == _lightManager)
        return E_FAIL;

    return _lightManager->AddLight(lightdesc);
}

vector<OtherLight*>* GameInstance::getLightList()
{
    return _lightManager->getLightList();
}

HRESULT GameInstance::DeleteLight(uint32 lightIndex, const string& lightName)
{
    if (nullptr == _lightManager)
        return E_FAIL;

    return _lightManager->DeleteLight(lightIndex, lightName);
}

OtherLight* GameInstance::FindLightFromID(const uint32 id)
{
    if (nullptr == _lightManager)
        return nullptr;


    return _lightManager->FindLightFromID(id);
}

HRESULT GameInstance::AllTurnOnLight()
{
    if (nullptr == _lightManager)
        return E_FAIL;

    return _lightManager->AllTurnOnLight();
}

HRESULT GameInstance::SelectTurnOffLight(uint32* pLightArray, uint32 arraySize)
{
    if (nullptr == _lightManager)
        return E_FAIL;

    return _lightManager->SelectTurnOffLight(pLightArray, arraySize);
}

Vec4 GameInstance::TerrainPicking(POINT pt, Transform* trans, VIBufferTerrain* buffer)
{
    if (nullptr == _picking)
        return Vec4();

    return _picking->GetInstance()->TerrainPicking(pt, trans, buffer);
}

_bool GameInstance::PickObject(POINT pt, Transform* trans, VIBuffer* objectBuffer, Vec4& vPos)
{
    if (nullptr == _picking)
        return false;

    return _picking->PickObject(pt, trans, objectBuffer, vPos);
}

void GameInstance::CreateObject(GameObject* pObj, LAYER_TAG _eLayer)
{
    if (nullptr == _pEventManager)
        return;

    _pEventManager->CreateObject(pObj, _eLayer);
}

void GameInstance::DeleteObject(GameObject* pObj)
{
    if (nullptr == _pEventManager)
        return;

    _pEventManager->DeleteObject(pObj);
}

void GameInstance::EventManagerTick()
{
    if (nullptr == _pEventManager)
        return;

    _pEventManager->Tick();
}

void GameInstance::PlaySound(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
    if (nullptr == _pSoundManager)
        return;

    _pSoundManager->PlaySound(pSoundKey, eID, fVolume);
}

void GameInstance::PlaySoundLoop(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
    if (nullptr == _pSoundManager)
        return;

    _pSoundManager->PlaySoundLoop(pSoundKey, eID, fVolume);
}

void GameInstance::PlayBGM(const TCHAR* pSoundKey, float fVolume)
{
    if (nullptr == _pSoundManager)
        return;

    _pSoundManager->PlayBGM(pSoundKey, fVolume);
}

void GameInstance::StopSound(CHANNELID eID)
{
    if (nullptr == _pSoundManager)
        return;

    _pSoundManager->StopSound(eID);
}

void GameInstance::StopAll()
{
    if (nullptr == _pSoundManager)
        return;

    _pSoundManager->StopAll();
}

void GameInstance::SetChannelVolume(CHANNELID eID, float fVolume)
{
    if (nullptr == _pSoundManager)
        return;

    _pSoundManager->SetChannelVolume(eID, fVolume);
}

HRESULT GameInstance::AddFont(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const wstring& strFontTag, const wstring& strFontFilePath)
{
    if (nullptr == _pFontManager)
        return E_FAIL;


    return _pFontManager->AddFont(pDevice, pDeviceContext, strFontTag, strFontFilePath);
}

HRESULT GameInstance::RenderFont(const wstring& strFontTag, const wstring& strText, const Vec2& vPos, FXMVECTOR color, _float rotation, Vec2 const& origin, _float scale)
{
    if (nullptr == _pFontManager)
        return E_FAIL;


    return _pFontManager->Render(strFontTag, strText, vPos, color,rotation, origin, scale);
}

_bool GameInstance::IsIn_FrustumWorld(FXMVECTOR vWorldPos, _float fRadius)
{
    if (nullptr == _pFrustum)
        return false;

    return _pFrustum->IsIn_FrustumWorld(vWorldPos, fRadius);
}

XMVECTOR GameInstance::GetCameraCaculator() const
{
    if(nullptr == _cameraHelper)
        return XMVECTOR();

    return _cameraHelper->GetCameraCaculator();
}

void GameInstance::Release_Engine()
{
    GameInstance::GetInstance()->DestroyInstance();

    FontManager::GetInstance()->DestroyInstance();
    RenderTargetManager::GetInstance()->DestroyInstance();
    SoundManager::GetInstance()->DestroyInstance();
    LevelManager::GetInstance()->DestroyInstance();
    ObjectManager::GetInstance()->DestroyInstance();
    EventManager::GetInstance()->DestroyInstance();
    CollisionManager::GetInstance()->DestroyInstance();
    ComponentManager::GetInstance()->DestroyInstance();
    TimeManager::GetInstance()->DestroyInstance();
    Frustum::GetInstance()->DestroyInstance();
    CameraHelper::GetInstance()->DestroyInstance();
    LightManager::GetInstance()->DestroyInstance();
    Picking::GetInstance()->DestroyInstance();
    InputHandler::GetInstance()->DestroyInstance();
    InputManager::GetInstance()->DestroyInstance();
    GraphicsManager::GetInstance()->DestroyInstance();
}

void GameInstance::Free()
{
    __super::Free();

    Safe_Release<Frustum*>(_pFrustum);
    Safe_Release<FontManager*>(_pFontManager);
    Safe_Release<RenderTargetManager*>(_pTargetManager);
    Safe_Release<SoundManager*>(_pSoundManager);
    Safe_Release<CameraHelper*>(_cameraHelper);
    Safe_Release<ComponentManager*>(_componentManager);
    Safe_Release<ObjectManager*>(_objectManager);
    Safe_Release<EventManager*>(_pEventManager);
    Safe_Release<CollisionManager*>(_collisionManager);
    Safe_Release<LevelManager*>(_levelManager);
    Safe_Release<TimeManager*>(_timeManager);
    Safe_Release<InputHandler*>(_inputHandler);
    Safe_Release<InputManager*>(_inputManager);
    Safe_Release<LightManager*>(_lightManager);
    Safe_Release<Picking*>(_picking);
    Safe_Release<GraphicsManager*>(_graphicManager);
}

#pragma once

#include "ComponentManager.h"
#include "CameraHelper.h"
#include "InputManager.h"
#include "LightManager.h"
#include "GameObject.h"
#include "Viewport.h"
#include "InputHandler.h"
#include "ObjectManager.h"
#include "EventManager.h"


BEGIN(Engine)

class ENGINE_DLL GameInstance final : public Base
{
	DECLARE_SINGLETON(GameInstance)

private:
	GameInstance();
	virtual ~GameInstance() = default;

public: // GameInstance Manager
	HRESULT	Initialize_Engine(uint32 levelNumbers, HINSTANCE instance, const GRAPHIC_DESC& graphicDesc, _Inout_ ID3D11Device** device, _Inout_ ID3D11DeviceContext** deviceContext);
	void Tick(_float fTimeDelta);
	void Clear(uint32 levelIndex);
public: // Timer Manager
	_float ComputeTimeDelta(const wstring& timerTag);
	HRESULT AddTimer(const wstring& timerTag);

public: // Graphic Device
	HRESULT	ClearBackBufferView(Color color);
	HRESULT ClearDepthStencilView();
	HRESULT Present();
	Viewport& GetViewPort();

public: // InputDevice
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(DIMK eMouse);
	_long	Get_DIMouseMove(DIMM eMouseState);

	_bool keyDown(_byte byKeyID);
	_bool KeyPressing(_byte byKeyID);
	_bool KeyUp(_byte byKeyID);

	_bool mouseDown(DIMK eMouseKeyID);
	_bool mousePreesing(DIMK eMouseKeyID);
	_bool mouseUp(DIMK eMouseKeyID);


public: // Custom Input
	class InputHandler* GetInputHandler();

public: // LevelManager
	HRESULT	OpenLevel(uint32 levelIndex, class Level* newLevel);
	uint32  GetCurrentLevelIndex();

public: // ObjectManager
	HRESULT AddProtoType(const wstring& prototypeTag, GameObject* prototype);
	HRESULT AddGameObject(uint32 levelIndex, const LAYER_TAG layerTag, const wstring& prototypeTag, void* argument = nullptr);
	vector<GameObject*>& GetLayerObject(const LAYER_TAG layertag);
	GameObject* GetLayerObjectTag(const LAYER_TAG layerag, const string& modelname);
	HRESULT ComparisonAddObject(int32 levelIndex, const string& addObjectfile = "", const LAYER_TAG layertag = LAYER_TAG::LAYER_END, void* argument = nullptr);
	int32	GetLayerObjectCount();
	vector<GameObject*>* GetCurrentObjectList(LAYER_TAG layerTag);
	HRESULT DeleteGameObject(uint32 levelIndex, const LAYER_TAG layertag, const uint32 objectNumber, const string& modelNames);
	map<const LAYER_TAG, Layer*>* GetEntireObjectLayer();
	int32 GetEntireLevel();
	GameObject* CloneGameObject(const wstring& strPrototypeTag, void* pArg);

	class Component* GetComponent(uint32 iLevelIndex, const LAYER_TAG eLayerTag, const wstring& strComponentTag, const string& strModelName, uint32 iIndex = 0);
	class Component* GetComponentFromID(uint32 iLevelIndex, const LAYER_TAG tag, const wstring& strComponentTag, const string& modelName, uint32 id);

public: // Component Manager
	HRESULT	AddProtoType(uint32 levelIndex, const wstring& PrototypeTag, class Component* protoType);
	class Component* CloneComponent(uint32 levelIndex, const wstring& protoTypeTag, void* argument = nullptr);

public: // Camera Helper
	HRESULT BindTransformToShader(Shader* shader, const char* constantName, CameraHelper::TRANSFORMSTATE state);
	HRESULT BindCameraPosition(Shader* shader, const _char* constantName, uint32 legnth);
	void SetTransform(CameraHelper::TRANSFORMSTATE state, FXMMATRIX trasnsformMatrix);
	Matrix GetTransform(CameraHelper::TRANSFORMSTATE state)						const;
	FXMMATRIX GetTransformCalculator(CameraHelper::TRANSFORMSTATE state)		const;
	Matrix GetInverseTransform(CameraHelper::TRANSFORMSTATE state)				const;
	FXMMATRIX GetInverseTransformCalculator(CameraHelper::TRANSFORMSTATE state)	const;
	XMVECTOR GetCameraCaculator()												const;
	Vec4 GetCameraPosition()													const;

public: // LightManager
	const LIGHT_DESC* GetLightDesc(uint32 lightIndex);
	HRESULT AddLight(const LIGHT_DESC& lightdesc);
	vector<OtherLight*>* getLightList();
	HRESULT DeleteLight(uint32 lightIndex, const string& lightName);
	class OtherLight* FindLightFromID(const uint32 id);
	HRESULT	AllTurnOnLight();
	HRESULT SelectTurnOffLight(uint32* pLightArray, uint32 arraySize);

public: // Picking
	Vec4 TerrainPicking(POINT pt, Transform* trans, VIBufferTerrain* buffer);
	_bool PickObject(POINT pt, Transform* trans, VIBuffer* objectBuffer, Vec4& vPos);

public: // EventManager
	void CreateObject(class GameObject* pObj, LAYER_TAG _eLayer);
	void DeleteObject(class GameObject* pObj);
	void EventManagerTick();

public: // SoundManager
	void PlaySound(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	void PlaySoundLoop(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	void PlayBGM(const TCHAR* pSoundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);

public: // FontManger
	HRESULT AddFont(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT RenderFont(const wstring& strFontTag, const wstring& strText, const Vec2& vPos, FXMVECTOR color = ::XMVectorZero(), _float rotation = 0.f, Vec2 const& origin = Vec2(0.f, 0.f), _float scale = 1.f);
	
public: // Frustum
	_bool IsIn_FrustumWorld(FXMVECTOR vWorldPos, _float fRadius = 0.f);

private:
	class TimeManager* _timeManager = nullptr;
	class GraphicsManager* _graphicManager = nullptr;
	class LevelManager* _levelManager = nullptr;
	class ObjectManager* _objectManager = nullptr;
	class ComponentManager* _componentManager = nullptr;
	class CameraHelper* _cameraHelper = nullptr;
	class InputManager* _inputManager = nullptr;
	class LightManager* _lightManager = nullptr;
	class Picking* _picking = nullptr;
	class InputHandler* _inputHandler = nullptr;
	class CollisionManager* _collisionManager = nullptr;
	class EventManager* _pEventManager = nullptr;
	class RenderTargetManager* _pTargetManager = nullptr;
	class SoundManager* _pSoundManager = nullptr;
	class FontManager* _pFontManager = nullptr;
	class Frustum* _pFrustum = nullptr;

public:
	static void Release_Engine();
	virtual void Free() override;
};

END


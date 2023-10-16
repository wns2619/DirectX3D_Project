#pragma once

#include "ComponentManager.h"
#include "CameraHelper.h"
#include "InputManager.h"
#include "LightManager.h"
#include "GameObject.h"
#include "Viewport.h"
#include "InputHandler.h"
#include "ObjectManager.h"

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
	GameObject* GetLayerObject(const LAYER_TAG layertag, OBJECT_TYPE type);
	GameObject* GetLayerObjectTag(const LAYER_TAG layerag, const string& modelname);
	HRESULT ComparisonAddObject(int32 levelIndex, const string& addObjectfile = "", const LAYER_TAG layertag = LAYER_TAG::LAYER_END, void* argument = nullptr);
	int32	GetLayerObjectCount();
	vector<GameObject*>* GetCurrentObjectList(LAYER_TAG layerTag);
	HRESULT DeleteGameObject(uint32 levelIndex, const LAYER_TAG layertag, const uint32 objectNumber, const string& modelNames);
	map<const LAYER_TAG, Layer*>* GetEntireObjectLayer();
	int32 GetEntireLevel();
	GameObject* CloneGameObject(const wstring& strPrototypeTag, void* pArg);

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
	/*HRESULT AddLightProtoType(uint32 levelIndex, Light::LightType type, const wstring& lighttag, Component* prototype);
	Component* CloneLight(uint32 levelIndex, Light::LightType type, const wstring& lighttag, void* argument);*/
	const LIGHT_DESC* GetLightDesc(uint32 lightIndex);
	HRESULT AddLight(const LIGHT_DESC& lightdesc);
	vector<OtherLight*>* getLightList();
	HRESULT DeleteLight(uint32 lightIndex, const string& lightName);

public: // Picking
	Vec4 TerrainPicking(POINT pt, Transform* trans, VIBufferTerrain* buffer);
	_bool PickObject(POINT pt, Transform* trans, VIBuffer* objectBuffer, Vec4& vPos);

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

public:
	static void Release_Engine();
	virtual void Free() override;
};

END


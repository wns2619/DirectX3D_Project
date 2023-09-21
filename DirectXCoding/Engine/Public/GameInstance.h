#pragma once

#include "ComponentManager.h"
#include "CameraHelper.h"
#include "InputManager.h"

BEGIN(Engine)

class GameObject;

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

public: // InputDevice
	_byte	Get_DIKeyState(_ubyte byKeyID);
	_byte	Get_DIMouseState(InputManager::MOUSEKEYSTATE eMouse);
	_long	Get_DIMouseMove(InputManager::MOUSEMOVESTATE eMouseState);

public: // LevelManager
	HRESULT	OpenLevel(uint32 levelIndex, class Level* newLevel);
	uint32  GetCurrentLevelIndex();

public: // ObjectManager
	HRESULT AddProtoType(const wstring& prototypeTag, GameObject* prototype);
	HRESULT AddGameObject(uint32 levelIndex, const wstring& layerTag, const wstring& prototypeTag, void* argument = nullptr);

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

private:
	class TimeManager* _timeManager = nullptr;
	class GraphicsManager* _graphicManager = nullptr;
	class LevelManager* _levelManager = nullptr;
	class ObjectManager* _objectManager = nullptr;
	class ComponentManager* _componentManager = nullptr;
	class CameraHelper* _cameraHelper = nullptr;
	class InputManager* _inputManager = nullptr;

public:
	static void Release_Engine();
	virtual void Free() override;
};

END


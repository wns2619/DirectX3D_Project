#pragma once

#include "ComponentManager.h"

BEGIN(Engine)

class GameObject;

class ENGINE_DLL GameInstance final : public Base
{
	DECLARE_SINGLETON(GameInstance)

private:
	GameInstance();
	virtual ~GameInstance() = default;

public: // GameInstance Manager
	HRESULT	Initialize_Engine(uint32 levelNumbers, const GRAPHIC_DESC& graphicDesc, _Inout_ ID3D11Device** device, _Inout_ ID3D11DeviceContext** deviceContext);
	void Tick(_float fTimeDelta);
	void Clear(uint32 levelIndex);
public: // Timer Manager
	_float ComputeTimeDelta(const wstring& timerTag);
	HRESULT AddTimer(const wstring& timerTag);

public: // Graphic Device
	HRESULT	ClearBackBufferView(Color color);
	HRESULT ClearDepthStencilView();
	HRESULT Present();

public: // LevelManager
	HRESULT	OpenLevel(uint32 levelIndex, class Level* newLevel);


public: // ObjectManager
	HRESULT AddProtoType(const wstring& prototypeTag, GameObject* prototype);
	HRESULT AddGameObject(uint32 levelIndex, const wstring& layerTag, const wstring& prototypeTag, void* argument = nullptr);

public:
	HRESULT	AddProtoType(uint32 levelIndex, const wstring& PrototypeTag, class Component* protoType);
	class Component* CloneComponent(uint32 levelIndex, const wstring& protoTypeTag, void* argument = nullptr);

private:
	class TimeManager* _timeManager = nullptr;
	class GraphicsManager* _graphicManager = nullptr;
	class LevelManager* _levelManager = nullptr;
	class ObjectManager* _objectManager = nullptr;
	class ComponentManager* _componentManager = nullptr;
public:
	static void Release_Engine();
	virtual void Free() override;
};

END


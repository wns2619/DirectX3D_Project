#pragma once


#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

class Layer;

class ObjectManager final : public Base
{
	DECLARE_SINGLETON(ObjectManager)

private:
	ObjectManager();
	virtual ~ObjectManager() = default;

public:
	HRESULT	ReserveManager(uint32 levelNumbers);
	HRESULT	AddPrototype(const wstring& prototypeTag, GameObject* prototype);
	HRESULT AddGameObject(uint32 levelIndex, const wstring& layerTag, const wstring& prototypeTag, void* argument);
	void Tick(const _float& timeDelta);
	void LateTick(const _float& timeDelta);
	void Clear(uint32 levelIndex);

	GameObject* GetLayerObject(const wstring& layertag, OBJECT_TYPE type);
	uint32 GetLayerObjectCount();
	vector<GameObject*>* GetCurrentObjectList(wstring& layerTag);

public: // IMGUI ¿ëµµ
	HRESULT ComparisonAddObject(int32 levelIndex, const string& addObjectfile = "", const wstring& layertag = L"", void* argument = nullptr);

private:
	map<const wstring, GameObject*> _protoTypes;


private:
	map<const wstring, Layer*>* _Layers = nullptr;
	int32	_levelNumber = 0;
	int32	_currenlevel = 0;
	typedef map<const wstring, Layer*>	LAYERS;

	int32  _levelObjectCount = 0;

private:
	GameObject* FindPrototype(const wstring& prototypeTag);
	Layer* FindLayer(int32 levelIndex, const wstring& layerTag);

public:
	virtual void Free() override;
};

END

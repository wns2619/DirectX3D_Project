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
	HRESULT AddGameObject(uint32 levelIndex, const LAYER_TAG layerTag, const wstring& prototypeTag, void* argument);

	void PriorityTick(const _float& timeDelta);
	void Tick(const _float& timeDelta);
	void LateTick(const _float& timeDelta);
	void Clear(uint32 levelIndex);

	GameObject* GetLayerObject(const LAYER_TAG layertag, OBJECT_TYPE type);
	GameObject* GetLayerObjectTag(const LAYER_TAG layerag, const string& modelname);
	uint32 GetLayerObjectCount();
	vector<GameObject*>* GetCurrentObjectList(LAYER_TAG layerTag);

	GameObject* CloneGameObject(const wstring& strPrototypeTag, void* pArg);

public:
	Component* GetComponent(uint32 iLevelIndex, const LAYER_TAG eTag, const wstring& strComponentTag, const string& modelName, uint32 iIndex);

public: // IMGUI ¿ëµµ
	HRESULT ComparisonAddObject(int32 levelIndex, const string& addObjectfile = "", LAYER_TAG layertag = LAYER_TAG::LAYER_END , void* argument = nullptr);
	HRESULT DeleteObject(uint32 levelIndex, const LAYER_TAG layertag, const uint32 objectNumber, const string& modelnames);
	map<const LAYER_TAG, Layer*>* GetEntireObjectLayer() { return _Layers; }
	int32 GetEntireLevel() { return _levelNumber; }
private:
	map<const wstring, GameObject*> _protoTypes;


private:
	map<const LAYER_TAG, Layer*>* _Layers = nullptr;
	int32	_levelNumber = 0;
	int32	_currenlevel = 0;
	typedef map<const LAYER_TAG, Layer*>	LAYERS;

	int32  _levelObjectCount = 0;
private:
	GameObject* FindPrototype(const wstring& prototypeTag);
	Layer* FindLayer(int32 levelIndex, const LAYER_TAG layerTag);

public:
	virtual void Free() override;
};

END

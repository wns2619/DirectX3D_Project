#pragma once


#include "Base.h"

BEGIN(Engine)

class GameObject;
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

private:
	map<const wstring, GameObject*> _protoTypes;


private:
	map<const wstring, Layer*>* _Layers = nullptr;
	int32	_levelNumber = 0;
	typedef map<const wstring, Layer*>	LAYERS;

private:
	GameObject* FindPrototype(const wstring& prototypeTag);
	Layer* FindLayer(int32 levelIndex, const wstring& layerTag);

public:
	virtual void Free() override;
};

END

#pragma once

#include "Level.h"

BEGIN(Client)

class GameScene final : public Level
{
private:
	explicit GameScene(ID3D11Device* _device, ID3D11DeviceContext* _deviceConetxt);
	virtual ~GameScene();

public:
	virtual HRESULT	Initialize()						override;
	virtual HRESULT Tick(const _float& timeDelta)		override;
	virtual HRESULT LateTick(const _float& timeDelata)	override;

public:
	HRESULT ReadyMyMap();
	HRESULT ReadyLandObjects(const LAYER_TAG layerTag);
	HRESULT	ReadyLayerTerrain(const LAYER_TAG layerTag);
	HRESULT ReadyLayerCamera(const LAYER_TAG layerTag);
	HRESULT ReadyLayerPlayer(const LAYER_TAG layerTag, void* pArg);
	HRESULT ReadyLayerMonster(const LAYER_TAG layerTag, void* pArg);
	HRESULT ReadyLight();

public:
	static GameScene* Create(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext);
	virtual void Free() override;
};

END


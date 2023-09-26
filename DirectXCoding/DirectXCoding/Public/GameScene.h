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
	HRESULT	ReadyLayerTerrain(const wstring& layerTag);
	HRESULT ReadyLayerCamera(const wstring& layerTag);
	HRESULT ReadyLayerPlayer(const wstring& layerTag);

public:
	static GameScene* Create(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext);
	virtual void Free() override;
};

END


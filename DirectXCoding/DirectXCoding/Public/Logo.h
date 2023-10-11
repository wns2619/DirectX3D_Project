#pragma once

#include "Level.h"

BEGIN(Client)

class Logo final : public Level
{
private:
	Logo(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~Logo() = default;
	
public:
	virtual HRESULT	Initialize()						override;
	virtual HRESULT	Tick(const _float& timeDelta)		override;
	virtual HRESULT LateTick(const _float& timeDelta)	override;

private:
	HRESULT	ReadyLayerBackGround(const LAYER_TAG layerTag);

public:
	static Logo* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual void Free() override;
};


END
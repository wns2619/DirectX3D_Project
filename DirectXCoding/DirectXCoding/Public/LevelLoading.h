#pragma once

#include "Level.h"


BEGIN(Client)

class LevelLoading final : public Level
{
private:
	explicit LevelLoading(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~LevelLoading();

public:
	virtual HRESULT	Initialize(LEVEL nextLevel);
	virtual HRESULT Tick(const _float& timeDelta) override;
	virtual HRESULT LateTick(const _float& timeDelta) override;

private:
	LEVEL _nextLevel = LEVEL::LEVEL_END;
	class LevelHelper* _helper = nullptr;
public:
	static LevelLoading* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LEVEL nextLevel);
	virtual void Free() override;
};


END


#pragma once

#include "Base.h"

BEGIN(Engine)

class GameInstance;
class Renderer;

END


BEGIN(Client)

class Game final : public Base
{
private:
	explicit Game();
	virtual ~Game();

public:
	HRESULT	Initialize(HWND hwnd);
	void Tick(const _float& timeDelta);
	HRESULT Render();

private:
	GameInstance* _gameInstance			= nullptr;
	Renderer* _renderer					= nullptr;

	ID3D11Device* _device				= nullptr;
	ID3D11DeviceContext* _devicecontext = nullptr;
	
private:
	HRESULT	OpenLevel(LEVEL levelID);
	HRESULT	ReadyProtoTypeComponents();

public:
	static Game* Create(HWND hwnd);
public:
	virtual void Free() override;
};

END
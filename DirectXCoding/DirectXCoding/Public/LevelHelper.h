#pragma once

#include "Base.h"

BEGIN(Client)

class LevelHelper final : public Base
{
private:
	explicit LevelHelper(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~LevelHelper();

public:
	const wstring& GetLoadingText() const { return _title; }
	_bool IsFinished() const { return _IsFinished; }

public:
	HRESULT Initialize(LEVEL nextLevel);
	int32 Loading();
private:
	ID3D11Device*			_device = nullptr;
	ID3D11DeviceContext*	_deviceContext = nullptr;
	LEVEL					_nextLevel = LEVEL::LEVEL_END;
	wstring					_title = TEXT("");
	_bool					_IsFinished = false;

private:
	HANDLE					_lodingThread = nullptr;
	CRITICAL_SECTION		_criticalSection = { 0 };

private:
	HRESULT	LodingforLevelLogo();
	HRESULT LodingforLevelGame();
	HRESULT LodingforLevelEdit();

public:
	static LevelHelper* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, LEVEL nextLevel);
	virtual void Free() override;
};

END


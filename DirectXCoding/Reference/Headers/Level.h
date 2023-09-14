#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL Level abstract : public Base
{
protected:
	explicit Level(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~Level() = default;

public:
	virtual HRESULT	Initialize();
	virtual HRESULT Tick(const _float& timeDelta);
	virtual HRESULT LateTick(const _float& timeDelta);

protected:
	ID3D11Device* _device = nullptr;
	ID3D11DeviceContext* _deviceContext = nullptr;
	HANDLE	_hwnd = nullptr;

public:
	virtual void Free() override;
};



END
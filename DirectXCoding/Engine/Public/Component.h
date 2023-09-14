#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL Component abstract : public Base
{
protected:
	explicit Component(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Component(const Component& rhs);
	virtual ~Component() = default;

public:
	virtual HRESULT	InitializePrototype();
	virtual HRESULT Initialize(void* argument);


protected:
	ID3D11Device*			_device			= nullptr;
	ID3D11DeviceContext*	_deviceContext	= nullptr;

	_bool					_isCloned		= false;

public:
	virtual Component* Clone(void* argument) = 0;
	virtual void Free() override;
};

END


#pragma once

#include "Base.h"

BEGIN(Engine)

enum class COMPONENT_TYPE 
{ BUFFER, TRANSFORM, TEXTURE, SHADER, RENDERER, MODEL, LIGHT, COLLIDER, PHYSICS, COMPONENT_END };


class ENGINE_DLL Component abstract : public Base
{
protected:
	explicit Component(ID3D11Device* device, ID3D11DeviceContext* deviceContext, COMPONENT_TYPE type);
	explicit Component(const Component& rhs);
	virtual ~Component() = default;

public:
	virtual HRESULT	InitializePrototype();
	virtual HRESULT Initialize(void* argument);

public: // IMGUI
	COMPONENT_TYPE GetComponentType() { return _type; }

protected:
	ID3D11Device*			_device			= nullptr;
	ID3D11DeviceContext*	_deviceContext	= nullptr;

	_bool					_isCloned		= false;

	COMPONENT_TYPE	_type					= COMPONENT_TYPE::COMPONENT_END;

public:
	virtual Component* Clone(void* argument) = 0;
	virtual void Free() override;
};

END


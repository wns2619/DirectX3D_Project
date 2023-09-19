#pragma once

#include "Component.h"

BEGIN(Engine)

class Shader;

class ENGINE_DLL Light final : public Component
{
private:
	explicit Light(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Light(const Light& rhs);
	virtual ~Light() = default;

public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT	Initialize(void* argument) override;

public:

private:
	LightGroup _lightGroup;

public:
	static Light* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual Component* Clone(void* argument) override;
	virtual void Free() override;
};

END


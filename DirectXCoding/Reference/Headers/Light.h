#pragma once

#include "Component.h"

BEGIN(Engine)

class Shader;

class ENGINE_DLL Light final : public Component
{
public:
	enum class LightType { AMBIENT, CAPSULE, DIRECTIONAL, MULTIPLE, POINT, SPOT, LIGHT_END };

	struct DirectinoalLight
	{
		DirectinoalLight() { ::ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT4 Specular;
		XMFLOAT3 Direction;

		float Pad;
	};

private:
	explicit Light(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Light(const Light& rhs);
	virtual ~Light() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* argument)		override;

public:
	const Vec3 GammaToLinear(const Vec3& color) { return Vec3(color.x * color.x, color.y * color.y, color.z * color.z); }
	void FowardSetup(ID3D11Buffer* constantbuffer, LightType type);

public: // Light Bind
	HRESULT BindingLightToShader(Shader* shader, const _char* constantName, LightType type, uint32 legnth, uint32 offset = 0);

private:
	DirectinoalLight _dirLight;

	Vec3 DirectionalDir = ::XMVectorSet(1.f, 1.f, 1.f, 1.f);

public:
	static Light* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual Component* Clone(void* argument = nullptr);
	virtual void Free() override;

};




END


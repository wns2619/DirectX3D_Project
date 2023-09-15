#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL Transform final : public Component
{
public:
	enum class STATE { RIGHT, UP, LOOK, POSITION, STATE_END };

public:
	struct TRANSFORM_DESC
	{
		_float	speedPerSec = 0;
		_float	rotationRadianPerSec = 0;
	};

private:
	explicit Transform(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Transform(const Transform& rhs);
	virtual ~Transform() = default;

public:
	XMVECTOR GetState(STATE state) { return XMLoadFloat4x4(&_worldMatrix).r[static_cast<uint32>(state)]; }
	Vec3 GetScaled();

	void SetState(STATE state, FXMVECTOR vectorState);
	void SetScaling(const Vec3& vectorScale);
public:
	virtual HRESULT InitializePrototype()		override;
	virtual HRESULT Initialize(void* argument)	override;

public:
	HRESULT	BindShaderResources(class Shader* shader, const _char* constantName);

public:
	void Forward(const _float& timeDelta);
	void Backward(const _float& timeDelta);
	void Left(const _float& timeDelta);
	void Right(const _float& timeDelta);
	void FixRotation(XMVECTOR axis, const _float radian);
	void Turn(XMVECTOR axis, const _float& timeDelta);
	void LookAt(FXMVECTOR point);
	void Chase(FXMVECTOR point, _float const& timeDelta, _float distance = 0.1f);

public: // camera test
	void Strafe(const _float& timeDelta);
	void Pitch(const _float& timeDelta);
	void RotateY(_float angle);

private:
	Matrix _worldMatrix;

	TRANSFORM_DESC _transformDesc = {};

public:
	static Transform* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual Component* Clone(void* argument) override;
	virtual void Free() override;
};

END


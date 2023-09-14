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
	

	void SetState(STATE state, FXMVECTOR vectorState);

public:
	virtual HRESULT InitializePrototype()		override;
	virtual HRESULT Initialize(void* argument)	override;

public:
	void Forward(const _float& timeDelta);
	void Backward(const _float& timeDelta);
	void Left(const _float& timeDelta);
	void Right(const _float& timeDelta);
	void FixRotation(XMVECTOR axis, const _float radian);
	void Turn(XMVECTOR axis, const _float& timeDelta);
private:
	Matrix _worldMatrix;

	TRANSFORM_DESC _transformDesc = {};

public:
	static Transform* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual Component* Clone(void* argument) override;
	virtual void Free() override;
};

END


#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL Transform final : public Component
{
public:
	enum class STATE { RIGHT, UP, LOOK, POSITION, STATE_END };
	enum class Direction { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };
public:
	struct TRANSFORM_DESC
	{
		_float	speedPerSec = 0;
		_float	rotationRadianPerSec = 0;
	};

	struct DirectionVectors
	{
		XMVECTOR forward = ::XMVectorSet(0.f, 0.f, 1.f, 0.f);
		XMVECTOR left = ::XMVectorSet(-1.f, 0.f, 0.f, 0.f);
		XMVECTOR right = ::XMVectorSet(1.f, 0.f, 0.f, 0.f);
		XMVECTOR backward = ::XMVectorSet(0.f, 0.f, -1.f, 0.f);
		XMVECTOR up = ::XMVectorSet(0.f, 1.f, 0.f, 0.f);
		XMVECTOR down = ::XMVectorSet(0.f, -1.f, 0.f, 0.f);
	};

private:
	explicit Transform(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Transform(const Transform& rhs);
	virtual ~Transform() = default;

public:
	XMVECTOR GetState(STATE state) { return XMLoadFloat4x4(&_worldMatrix).r[static_cast<uint32>(state)]; }
	Vec3 GetScaled();
	
	Matrix GetWorldMatrix() const { return _worldMatrix; }
	XMMATRIX GetWorldMatrixCaculator() const { return ::XMLoadFloat4x4(&_worldMatrix); }
	Matrix GetInverseWorldMatrix() const
	{
		Matrix inverseWorldMatrix;
		::XMStoreFloat4x4(&inverseWorldMatrix, ::XMMatrixInverse(nullptr, _worldMatrix));
		return inverseWorldMatrix;
	}
	XMMATRIX GetInverseMatrixCaculator() const { return ::XMMatrixInverse(nullptr, ::XMLoadFloat4x4(&_worldMatrix)); }


	void SetState(STATE state, FXMVECTOR vectorState);
	void SetScaling(const Vec3& vectorScale);
	void SetWorldMatrix(Matrix WorldMatrix) { _worldMatrix = WorldMatrix; }

	void SetSpeedPerSec(_float speed) { _transformDesc.speedPerSec = speed; }

public:
	virtual HRESULT InitializePrototype()		override;
	virtual HRESULT Initialize(void* argument)	override;

public:
	HRESULT	BindShaderResources(class Shader* shader, const _char* constantName);

public:
	void Forward(const _float& timeDelta, class BinaryNavi* pNavigation = nullptr);
	void Backward(const _float& timeDelta, class BinaryNavi* pNavigation = nullptr);
	void Left(const _float& timeDelta, class BinaryNavi* pNavigation = nullptr);
	void Right(const _float& timeDelta, class BinaryNavi* pNavigation = nullptr);
	void FixRotation(XMVECTOR axis, const _float radian);
	void FixRotation(_float x, _float y, _float z);
	void BillBoardRotate(XMVECTOR axis, const _float fRadian);
	void Turn(XMVECTOR axis, const _float& timeDelta, _float fAngle = 1.f);
	void TurnTo(XMVECTOR vPoint, const _float& timeDelta);
	void LookAt(FXMVECTOR point);
	void Chase(FXMVECTOR point, _float const& timeDelta, _float distance = 0.1f);

	void Translate(Vec3& vTranslation);
	void SetLook(Vec4 vChangeLook);

public: // custom Movement
	void Move(XMVECTOR moveVector);
	void Move(Direction direction);
	void UpdateDirVectors();

	Vec3 CustomGetPositionV3() const;
	XMMATRIX CustomGetWorldMatrix() const;

	void SetPosition(XMVECTOR position) { _position = position; }
	XMVECTOR GetPosition() { return _position; }
	void SetWorldRotation(XMVECTOR rotation) { _rotation = rotation; }
	XMVECTOR GetWorldRotation() { return _rotation; }

	const DirectionVectors& GetDirectionalVectors() { return _directionalVectors; }
private:
	Matrix _worldMatrix;
	Matrix _fixRotation;

	XMVECTOR _rotation = ::XMVectorSet(0.f, 0.f, 0.f, 1.f);
	XMVECTOR _scale = ::XMVectorSet(1.f, 1.f, 1.f, 0.f);
	XMVECTOR _position = ::XMVectorSet(0.f, 0.f, 0.f, 1.f);

	_float speed = 0.1f;

	TRANSFORM_DESC _transformDesc = {};
	DirectionVectors _directionalVectors = {};

public:
	static Transform* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual Component* Clone(void* argument) override;
	virtual void Free() override;
};

END


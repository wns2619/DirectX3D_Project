#pragma once

#include "GameObject.h"

BEGIN(Engine)

class Transform;

class ENGINE_DLL Camera abstract : public GameObject
{
public:
	enum class CameraMode { FirstCamera, Camera_END };
public:
	struct CAMERA_DESC
	{
		_float _near	= 0;
		_float _far		= 0;
		_float _aspect	= 0;
		_float _fov		= 0;
	};
protected:
	explicit Camera(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Camera(const Camera& rhs);
	virtual ~Camera() = default;

public:
	virtual HRESULT InitializePrototype() override;
	virtual HRESULT Initialize(void* argument) override;
	virtual void Tick(const _float& timeDelta) override;
	virtual void LateTick(const _float& timeDelta) override;

public:


protected:
	Transform* _transform = nullptr;

	CAMERA_DESC _cameraDesc = {};
	Matrix _viewMatrix = ::XMMatrixIdentity(), _projMatrix = ::XMMatrixIdentity();

public:
	virtual GameObject* Clone(void* argument) = 0;
	virtual void Free() override;
};


END


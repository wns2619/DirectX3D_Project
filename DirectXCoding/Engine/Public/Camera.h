#pragma once

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class Transform;
class CameraHelper;

class ENGINE_DLL Camera abstract : public GameObject
{
public:
	enum class CameraMode { FirstCamera, Camera_END };
public:
	struct CAMERA_DESC : public Transform::TRANSFORM_DESC
	{
		Vec4 _eye, _at;
		_float _fovy, _aspect, _near, _far;
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

protected:
	Transform* _transform = nullptr;
	CameraHelper* _cameraHelper = nullptr;

	CAMERA_DESC _cameraDesc = {};

public:
	virtual GameObject* Clone(void* argument) = 0;
	virtual void Free() override;
};


END


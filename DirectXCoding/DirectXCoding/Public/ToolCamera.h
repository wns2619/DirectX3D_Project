#pragma once
#include "Camera.h"

BEGIN(Client)

class ToolCamera final : public Camera
{
public:
	typedef struct CAMERATOOL_DESC : public Camera::CAMERA_DESC
	{
		_float			fMouseSensitive = { 0.0f };
	};

private:
	explicit ToolCamera(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit ToolCamera(const ToolCamera& rhs);
	virtual ~ToolCamera() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& timeDelta)		override;
	virtual void LateTick(const _float& timeDelta)	override;

	void MouseMove(const _float& timeDelta);

private:
	_float _mouseSensitive = 0.f;

public:
	static ToolCamera* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
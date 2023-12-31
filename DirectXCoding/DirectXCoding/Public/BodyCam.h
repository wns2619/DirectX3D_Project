#pragma once

#include "PartObject.h"


BEGIN(Engine)
class CameraHelper;
END

BEGIN(Client)

class BodyCam final : public PartObject
{
public:
	struct BODYCAM_DESC : public PartObject::PART_DESC
	{
		Matrix* SocketMatrix;
		Matrix	SocketPivot;

		Vec4 vEye, vAt;
		_float fFov, fAspect, fNear, fFar;
	};

private:
	explicit  BodyCam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit BodyCam(const BodyCam& rhs); /* 복사 생성자. */

	virtual ~BodyCam() = default;

public:
	virtual HRESULT InitializePrototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void Tick(const _float& fTimeDelta)			override;
	virtual void LateTick(const _float& fTimeDelta)		override;
	virtual HRESULT Render()							override;

private:
	Matrix* m_SocketMatrix;
	Matrix	m_SocketPivotMatrix;

	Vec4 _vEye, _vAt;
	_float _fFov, _fAspect, _fNear, _fFar;

	CameraHelper* _pCameraHelper = nullptr;
private:
	HRESULT Ready_Components();

public:
	static BodyCam* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual GameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;

};

END

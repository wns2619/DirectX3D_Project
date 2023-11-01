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
	explicit BodyCam(const BodyCam& rhs); /* ���� ������. */

	virtual ~BodyCam() = default;

public:
	virtual HRESULT InitializePrototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void Tick(const _float& fTimeDelta)			override;
	virtual void LateTick(const _float& fTimeDelta)		override;
	virtual HRESULT Render()							override;

public:
	void AddPlayerCameraFov(_float fov) { _fFov += fov; }
	_float PlayerCameraFov() { return _fFov; }

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
	virtual GameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;

};

END

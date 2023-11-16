#pragma once

#include "PartObject.h"

BEGIN(Engine)
class OtherLight;
END

BEGIN(Client)

class Surefire final : public PartObject
{
public:
	typedef struct tagPartDesc : public PartObject::PART_DESC
	{
		Matrix*			SocketMatrix;
		Matrix			SocketPivot;

	}FLASHLIGHT_DESC;

protected:
	/* ������ ������ �� */
	explicit Surefire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	explicit Surefire(const Surefire& rhs); /* ���� ������. */

	virtual ~Surefire() = default;

public:
	virtual HRESULT InitializePrototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void Tick(const _float& fTimeDelta)			override;
	virtual void LateTick(const _float& fTimeDelta)		override;
	virtual HRESULT Render()							override;

private:
	Matrix*					m_SocketMatrix;
	Matrix					m_SocketPivotMatrix;

private:
	HRESULT Ready_Components();

private:
	OtherLight* _pLight = nullptr;

public:
	static Surefire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual GameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;

};

END

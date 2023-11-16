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
	/* 원형을 생성할 때 */
	explicit Surefire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	explicit Surefire(const Surefire& rhs); /* 복사 생성자. */

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
	virtual GameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;

};

END

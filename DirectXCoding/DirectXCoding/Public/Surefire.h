#pragma once

#include "GameObject.h"


BEGIN(Client)

class Surefire final : public GameObject
{
public:
	typedef struct tagPartDesc
	{
		Transform*		pParentTransform = { nullptr };
		Matrix*			SocetMatrix = nullptr;
		Matrix			SocketPivot;

	}PART_DESC;

protected:
	/* 원형을 생성할 때 */
	Surefire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	Surefire(const GameObject& rhs); /* 복사 생성자. */

	virtual ~Surefire() = default;

public:
	virtual HRESULT InitializePrototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void Tick(const _float& fTimeDelta)			override;
	virtual void LateTick(const _float& fTimeDelta)		override;
	virtual HRESULT Render()							override;

private: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	Shader*				m_pShaderCom = { nullptr };	


private:
	Transform*				m_pParentTransform = { nullptr };
	Matrix*					m_SocketMatrix;
	Matrix					m_SocketPivotMatrix;
	Matrix					m_WorldMatrix;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static Surefire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual GameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;

};

END

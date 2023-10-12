#pragma once

#include "GameObject.h"

BEGIN(Client)

class Surefire final : public GameObject
{
public:
	typedef struct tagPartDesc
	{
		Transform*		pParentTransform = { nullptr };
		FXMMATRIX		SocetMatrix;
		Matrix			SocketPivot;
	}PART_DESC;

protected:
	/* ������ ������ �� */
	Surefire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	Surefire(const GameObject& rhs); /* ���� ������. */

	virtual ~Surefire() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private: /* �ش� ��ü�� ����ؾ��� ������Ʈ���� �����ϳ�. */
	Shader*				m_pShaderCom = { nullptr };	


private:
	Transform*				m_pParentTransform = { nullptr };
	FXMMATRIX				m_pSocketMatrix;
	Matrix					m_SocketPivotMatrix;
	Matrix				m_WorldMatrix;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static Surefire* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual GameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;

};

END

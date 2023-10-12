#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class PlayerBody final : public GameObject
{
public:
	typedef struct tagPartDesc
	{
		Transform* pParentTransform;
	}PART_DESC;

protected:
	/* ������ ������ �� */
	PlayerBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	PlayerBody(const GameObject& rhs); /* ���� ������. */

	virtual ~PlayerBody() = default;

public:
	FXMMATRIX Get_SocketBonePtr(const char* pBoneName);
	Matrix Get_SocketPivotMatrix();

public:
	void Set_AnimationIndex(_bool isLoop, uint32 iAnimIndex);



public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private: /* �ش� ��ü�� ����ؾ��� ������Ʈ���� �����ϳ�. */
	Shader* m_pShaderCom = { nullptr };

private:
	Transform* m_pParentTransform = { nullptr };
	Matrix	   m_WorldMatrix;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static PlayerBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual GameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END


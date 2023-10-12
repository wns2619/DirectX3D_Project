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
	Matrix* Get_SocketBonePtr(const char* pBoneName);
	Matrix Get_SocketPivotMatrix();

public:
	void Set_AnimationIndex(_bool isLoop, uint32 iAnimIndex);



public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& fTimeDelta)		override;
	virtual void LateTick(const _float& fTimeDelta) override;
	virtual HRESULT Render()						override;

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


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
	/* 원형을 생성할 때 */
	PlayerBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	PlayerBody(const GameObject& rhs); /* 복사 생성자. */

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

private: /* 해당 객체가 사용해야할 컴포넌트들을 저장하낟. */
	Shader* m_pShaderCom = { nullptr };

private:
	Transform* m_pParentTransform = { nullptr };
	Matrix	   m_WorldMatrix;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static PlayerBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual GameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END


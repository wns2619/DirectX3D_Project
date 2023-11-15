#pragma once

#include "Client_Define.h"
#include "PartObject.h"

BEGIN(Engine)
class Texture;
END

BEGIN(Client)

class PlayerBody final : public PartObject
{
public:
	typedef struct tagPartDesc
	{
		Transform* pParentTransform;
	}PART_DESC;

protected:
	/* ������ ������ �� */
	explicit PlayerBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	explicit PlayerBody(const PlayerBody& rhs); /* ���� ������. */

	virtual ~PlayerBody() = default;

public:
	Matrix* Get_SocketBonePtr(const char* pBoneName);
	Matrix Get_SocketPivotMatrix();

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& fTimeDelta)		override;
	virtual void LateTick(const _float& fTimeDelta) override;
	virtual HRESULT Render()						override;

private:
	_float _fLiftTime = 0.f;
	Texture* _pTexture = nullptr;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static PlayerBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual GameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END


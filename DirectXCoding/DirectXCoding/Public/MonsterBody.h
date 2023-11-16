#pragma once

#include "MonsterPartObject.h"

BEGIN(Client)

class MonsterBody final : public MonsterPartObject
{
private:
	explicit MonsterBody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit MonsterBody(const MonsterBody& rhs);
	virtual ~MonsterBody() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& fTimeDelta)		override;
	virtual void LateTick(const _float& fTimeDelta) override;
	virtual HRESULT Render()						override;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	static MonsterBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual GameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END


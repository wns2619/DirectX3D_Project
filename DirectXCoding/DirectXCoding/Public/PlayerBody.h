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
	/* 원형을 생성할 때 */
	explicit PlayerBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	explicit PlayerBody(const PlayerBody& rhs); /* 복사 생성자. */

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
	virtual GameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END


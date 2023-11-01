#pragma once

#include "DynamicObject.h"


BEGIN(Client)

class Bullet final : public DynamicObject
{
public:
	struct BULLET_DESC
	{
		_float fBulletSpeed = 0.f;
		Vec3 vDir;
		Vec4 vPos;
	};

private:
	explicit Bullet(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Bullet(const Bullet& rhs);
	virtual ~Bullet() = default;

public:
	virtual HRESULT InitializePrototype()				override;
	virtual HRESULT Initialize(void* pArg)				override;
	virtual void Tick(const _float& timeDelta)			override;
	virtual void LateTick(const _float& timeDelta)		override;
	virtual HRESULT Render()							override;

public:
	virtual HRESULT ReadyCollider()						override;

private:
	BULLET_DESC _BulletDesc = {};

public:
	static Bullet* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END
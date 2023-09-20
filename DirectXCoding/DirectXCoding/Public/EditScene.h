#pragma once

#include "Level.h"

BEGIN(Client)

class EditScene final : public Level
{
private:
	explicit EditScene(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~EditScene();

public:
	virtual HRESULT	Initialize()						override;
	virtual HRESULT Tick(const _float& timeDelta)		override;
	virtual HRESULT LateTick(const _float& timeDelata)	override;

public:
	static EditScene* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual void Free() override;
};

END


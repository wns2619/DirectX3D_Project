#pragma once

#include "Client_Define.h"
#include "Base.h"

BEGIN(Client)

class ImGuiManager final : Base
{
	DECLARE_SINGLETON(ImGuiManager)

private:
	ImGuiManager();
	virtual ~ImGuiManager() = default;

public:
	HRESULT	Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void	Tick(const _float& timeDelta);
	void	LateTick(const _float& timeDelta);
	HRESULT Render();

public:
	void GuiStyle();

private:
	_bool _someCondition = true;

public:
	virtual void Free() override;
};

END


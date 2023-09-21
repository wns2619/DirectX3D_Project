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
	_bool _someCondition		= true;

	_bool m_windowMoveFlag		= false;
	_bool m_windowResizeFlag	= false;
	_bool m_mouseCameraRotation = true;

	_float m_windowContentHeight		= g_iWinSizeY;
	_float m_objectsWindowWidth			= 450.f;
	_float m_objectWdowHeaderHeight		= 70.f;
	_float m_gameObjectSectionHeight	= 420.f;
	_float m_lightSectionHeight			= 275.f;
	_float m_sectionSeperatorHeight		= 25.f;
	_float m_splitterButtonPadding		= 10.f;

public:
	virtual void Free() override;
};

END


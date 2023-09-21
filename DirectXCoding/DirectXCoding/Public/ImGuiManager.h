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
	void DrawSplitter(_bool split_vertically, _float thickness, _float* size0, _float* size1,
		_float min_size0, _float min_size1, _float size = -1.f, _float buttonPadding = 0.f);

private:
	_bool _someCondition		= true;

	_bool _windowMoveFlag		= false;
	_bool _windowResizeFlag		= false;
	_bool _mouseCameraRotation	= true;

	_float _windowContentHeight			= 400.f;
	_float _objectsWindowWidth			= 450.f;
	_float _objectWdowHeaderHeight		= 70.f;
	_float _gameObjectSectionHeight		= 420.f;
	_float _lightSectionHeight			= 275.f;
	_float _sectionSeperatorHeight		= 25.f;
	_float _splitterButtonPadding		= 10.f;
	
	_float _TerrainSectionHeight		= 130.f;

public:
	virtual void Free() override;
};

END


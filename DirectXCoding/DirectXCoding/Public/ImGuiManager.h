#pragma once

#include "Client_Define.h"
#include "VIBufferTerrain.h"
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
	void MouseMove();

private:
	// IMGUI
	_bool _someCondition		= true;
	_bool _Isterrain = false;

	_bool _windowMoveFlag		= false;
	_bool _windowResizeFlag		= false;
	_bool _mouseCameraRotation	= true;

	_float _windowContentHeight			= 900.f;
	_float _objectsWindowWidth			= 450.f;
	_float _objectWdowHeaderHeight		= 70.f;
	_float _gameObjectSectionHeight		= 420.f;
	_float _lightSectionHeight			= 275.f;
	_float _sectionSeperatorHeight		= 25.f;
	_float _splitterButtonPadding		= 10.f;
	
	_float _TerrainSectionHeight		= 130.f;


	// GameObjects


	// Camera


	// Lights


	// Load && Save
	void LoadModelList(string path = "../Binaries/Resources/MyModels/");


	vector<pair<string, _bool>> _modelNames;
	vector<_bool> _modelNameHoveringState;
	
	const string _rootModelDirection = "../Binaries/Resources/MyModels/";
	// Selection

	// Terrain
	VIBufferTerrain::TERRAIN_DESC desc = {};

public:
	virtual void Free() override;
};

END


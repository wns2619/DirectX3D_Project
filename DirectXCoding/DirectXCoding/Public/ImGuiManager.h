#pragma once

#include "Client_Define.h"
#include "VIBufferTerrain.h"
#include "Base.h"
#include "GameObject.h"


BEGIN(Client)

struct TexturePathPBR
{
	wstring diffusePath = L"Diffuse";
	wstring normalPath = L"Normal";
	wstring metallicPath = L"Metallic";
	wstring roughnessPath = L"Roughness";
	wstring emissivePath = L"Emissive";
	wstring ambientOcclusionPath = L"Ambient";
	wstring displacmentPath = L"Displacement";
};

static const char* LightTypeName[] = { "Point", "Spot", "Directional" };


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

public: // Section
	void MainSection();
	void AddLightSection();
	HRESULT MainLightSection();
	HRESULT ModelNameCardSection();
	HRESULT ObjectsSection();

public: // Save && Load
	void FileSave();
	void FileLoad();


public: // Binary
	void BinaryModelSave(const string& fpxPath, const wstring& binaryDirectory);

private:
	// Device && DeviceContext
	ID3D11Device* _device				= nullptr;
	ID3D11DeviceContext* _deviceContext = nullptr;

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

	_bool  _IsButtonPreesd = false;

	int32  _selectedIndex = -1;

	WCHAR	_tempName[64] = L"Collect";
	uint32  _tempSize = 64;

	
	aiTextureType _textureType = aiTextureType::aiTextureType_TRANSMISSION;
	_float _imageSize = 50.f;
	_float _offset = 60.f;
	_float _checkBoxOffset = 28.f;
	string _fileguipath = "..\\Binaries\\Resources\\Textures\\DDS\\";
	const string _rootTextureDirection = "..\\Binaries\\Resources\\Textures\\DDS\\";
	TexturePathPBR _texturePath = {};

	// GameObjects
	GameObject* _SelectGameObject = nullptr;

	void GameObjectUpdate(int32 vectorIndex);
	void UpdateModelUI(int32 vectorIndex);
	void UpdateMaterialUI(int32 vectorIndex);

	// Camera


	// Lights
	Vec4 _lightposition = Vec4(1.f, 1.f, 1.f, 1.f);
	Vec3 _LightColor = Vec3(1.f,1.f,1.f);
	Vec3 _rotationDeg = Vec3(1.f,1.f,1.f);
	_float _lightIntensity = 1.f;
	_float _lightrange = 1.f;

	// Load && Save
	void LoadModelList(string path = "..\\Binaries\\Resources\\MyModels\\");
	

	vector<pair<string, _bool>> _modelNames; // string = name, bool = Isdirectory
	vector<_bool> _modelNameHoveringState;
	string _currentDirectoryPath;
	string _currentDirectoryName;

	const string _rootModelDirection = "..\\Binaries\\Resources\\MyModels\\";
	wstring _editlayerTag = TEXT("LayerEntireObject");
	// Selection

	// Terrain
	VIBufferTerrain::TERRAIN_DESC desc = {};

public:
	virtual void Free() override;
};

END


#pragma once

#include "Base.h"

BEGIN(Client)

class ImGuiResourceHandler final : public Base
{
	DECLARE_SINGLETON(ImGuiResourceHandler)

private:
	ImGuiResourceHandler();
	virtual ~ImGuiResourceHandler() = default;

public:
	HRESULT	Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void	Tick(const _float& timeDelta);
	void	LateTick(const _float& timeDelta);
	HRESULT Render();


public:
	ID3D11ShaderResourceView* GetResourceTexture(const WCHAR* texturePath, _bool isCubeMap = false);
	HRESULT ComparisonPathwithObject(const string& filePath);
	void LoadModelTexture(string path = "..\\Binaries\\Resources\\Textures\\");

public:
	_bool CreateTextureFormFile(const WCHAR* texturePath, _bool isCubeMap = false);
	const pair<LAYER_TAG, wstring>& FindProtoFilePath(const string& filePathKey);
	HRESULT AddProtoFilePath(const string& filePathKey, LAYER_TAG layertag, const wstring& filePathValue);

public:
	const pair<const wstring, const wstring>& FindProtoComponentName(const string& filePathKey);
	HRESULT AddProtoComponentName(const string& filePathKey, const wstring& modelNameKey, const wstring& ShaderValue);

private:
	ID3D11Device* _device				= nullptr;
	ID3D11DeviceContext* _deviceContext = nullptr;

	map<const wstring, ID3D11ShaderResourceView*> _imguitextures;
	map<const string, pair<LAYER_TAG, wstring>> _prototypefilepath;
	map<const string, pair<const wstring, const wstring>> _prototypeComponentName;

	const wstring rootTexturePath = TEXT("..\\Binaries\\Resources\\Textures\\");
public:
	virtual void Free() override;

};

END

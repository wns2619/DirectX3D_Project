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


public:
	_bool CreateTextureFormFile(const WCHAR* texturePath, _bool isCubeMap = false);
	const wstring& FindProtoFilePath(const string& filePathKey);
	HRESULT AddProtoFilePath(const string& filePathKey, const wstring& filePathValue);
private:
	ID3D11Device* _device				= nullptr;
	ID3D11DeviceContext* _deviceContext = nullptr;

	map<const wstring, ID3D11ShaderResourceView*> _imguitextures;
	map<const string, wstring> _prototypefilepath;

	const wstring rootTexturePath = TEXT("../Binaries/Resources/Textures/");
public:
	virtual void Free() override;

};

END

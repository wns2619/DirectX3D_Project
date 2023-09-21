#pragma once

#include "Base.h"

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

private:
	_bool CreateTextureFormFile(const WCHAR* texturePath, _bool isCubeMap = false);


private:
	ID3D11Device* _device				= nullptr;
	ID3D11DeviceContext* _deviceContext = nullptr;

	map<const wstring, ID3D11ShaderResourceView*> _imguitextures;

	const wstring rootTexturePath = L"Resources\\";
public:
	virtual void Free() override;

};



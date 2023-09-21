#pragma once
#include "pch.h"
#include "Client_Define.h"
#include "Base.h"

class ImGuiResourceHandler final : public Base
{
	DECLARE_SINGLETON(ImGuiResourceHandler)

private:
	ImGuiResourceHandler(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual ~ImGuiResourceHandler() = default;

private:
	HRESULT	Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void	Tick(const _float& timeDelta);
	void	LateTick(const _float& timeDelta);
	HRESULT Render();

private:
	ID3D11Device* _device				= nullptr;
	ID3D11DeviceContext* _deviceContext = nullptr;

public:
	virtual void Free() override;
};
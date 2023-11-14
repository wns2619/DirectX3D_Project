#pragma once

#include "Base.h"

BEGIN(Engine)

class CustomFont final : public Base
{
private:
	explicit CustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CustomFont() = default;

public:
	HRESULT Initialize(const wstring& strFontFilePath);
	HRESULT Render(const wstring& strText,
	const Vec2& vPos,
		FXMVECTOR color,
		_float rotation,
		Vec2 const& origin,
		_float scale);

private:
	ID3D11Device* _pDevice					= nullptr;
	ID3D11DeviceContext* _pDeviceContext	= nullptr;
	SpriteBatch* _pBatch					= nullptr;
	SpriteFont* _pFont						= nullptr;

public:
	static CustomFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const wstring& strFontFilePath);
	virtual void Free() override;
};

END


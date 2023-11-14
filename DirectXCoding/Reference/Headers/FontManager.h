#pragma once

#include "Base.h"

BEGIN(Engine)

class FontManager final : public Base
{
	DECLARE_SINGLETON(FontManager)

private:
	FontManager();
	virtual ~FontManager() = default;

public:
	HRESULT AddFont(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const wstring& strFontTag, const wstring& strFontFilePath);
	HRESULT Render(const wstring& strFontTag, const wstring& strText,
		const Vec2& vPos,
		FXMVECTOR color,
		_float rotation,
		Vec2 const& origin,
		_float scale);

private:
	map<const wstring, class CustomFont*> _Fonts;

private:
	class CustomFont* FindFont(const wstring& strFontTag);

public:
	virtual void Free() override;
};

END


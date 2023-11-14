#include "pch.h"
#include "FontManager.h"
#include "CustomFont.h"

IMPLEMENT_SINGLETON(FontManager)

FontManager::FontManager()
{
}

HRESULT FontManager::AddFont(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const wstring& strFontTag, const wstring& strFontFilePath)
{
	if (nullptr != FindFont(strFontTag))
		return E_FAIL;

	CustomFont* pFont = CustomFont::Create(pDevice, pDeviceContext, strFontFilePath);
	if (nullptr == pFont)
		return E_FAIL;

	_Fonts.emplace(strFontTag, pFont);

	return S_OK;
}

HRESULT FontManager::Render(const wstring& strFontTag, const wstring& strText, const Vec2& vPos, FXMVECTOR color, _float rotation, Vec2 const& origin, _float scale)
{
	CustomFont* pFont = FindFont(strFontTag);

	if (nullptr == pFont)
		return E_FAIL;

	return pFont->Render(strText, vPos, color, rotation, origin, scale);
}

CustomFont* FontManager::FindFont(const wstring& strFontTag)
{
	auto iter = _Fonts.find(strFontTag);

	if (iter == _Fonts.end())
		return nullptr;

	return iter->second;
}

void FontManager::Free()
{
	__super::Free();

	for (auto& Pair : _Fonts)
		Safe_Release<CustomFont*>(Pair.second);

	_Fonts.clear();
}

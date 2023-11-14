#include "pch.h"
#include "CustomFont.h"

CustomFont::CustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: _pDevice(pDevice), _pDeviceContext(pDeviceContext)
{
	Safe_AddRef<ID3D11Device*>(_pDevice);
	Safe_AddRef<ID3D11DeviceContext*>(_pDeviceContext);
}

HRESULT CustomFont::Initialize(const wstring& strFontFilePath)
{
	_pFont = new SpriteFont(_pDevice, strFontFilePath.c_str());
	_pBatch = new SpriteBatch(_pDeviceContext);

	return S_OK;
}

HRESULT CustomFont::Render(const wstring& strText, const Vec2& vPos, FXMVECTOR color, _float rotation, Vec2 const& origin, _float scale)
{
	_pBatch->Begin();

	_pFont->DrawString(_pBatch, strText.c_str(), vPos, color, rotation, origin, scale);

	_pBatch->End();

	return S_OK;
}

CustomFont* CustomFont::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const wstring& strFontFilePath)
{
	CustomFont* pInstance = new CustomFont(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize(strFontFilePath)))
	{
		MSG_BOX("Failed to Created : CustomFont");
		Safe_Release<CustomFont*>(pInstance);
	}

	return pInstance;
}

void CustomFont::Free()
{
	__super::Free();

	Safe_Release<ID3D11Device*>(_pDevice);
	Safe_Release<ID3D11DeviceContext*>(_pDeviceContext);

	Safe_Delete<SpriteBatch*>(_pBatch);
	Safe_Delete<SpriteFont*>(_pFont);
}

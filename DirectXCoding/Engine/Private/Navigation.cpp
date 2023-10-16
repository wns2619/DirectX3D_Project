#include "pch.h"
#include "Navigation.h"
#include "Cell.h"

Navigation::Navigation(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Component(device, deviceContext, COMPONENT_TYPE::NAVIGATION)
{

}

Navigation::Navigation(const Navigation& rhs)
	: Component(rhs)
	, _currentIndex(rhs._currentIndex)
	, _cells(rhs._cells)
{
	for (auto& pCell : _cells)
		Safe_AddRef<Cell*>(pCell);
}

HRESULT Navigation::InitializePrototype(const wstring& strNavigationFilePath)
{
	

	return S_OK;
}

HRESULT Navigation::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

Navigation* Navigation::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wstring& strNavigationFilePath)
{
	Navigation* pInstance = new Navigation(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype(strNavigationFilePath)))
	{
		MSG_BOX("Failed to Created : Navigation");
		Safe_Release<Navigation*>(pInstance);
	}

	return pInstance;
}

Component* Navigation::Clone(void* pArg)
{
	Navigation* pInstance = new Navigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : Navigation");
		Safe_Release<Navigation*>(pInstance);
	}

	return pInstance;
}

void Navigation::Free()
{
	__super::Free();

	for (auto& pCell : _cells)
		Safe_Release<Cell*>(pCell);
}

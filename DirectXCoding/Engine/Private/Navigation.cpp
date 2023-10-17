#include "pch.h"
#include "Navigation.h"
#include "Cell.h"
#include "Shader.h"
#include "CameraHelper.h"

Navigation::Navigation(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Component(device, deviceContext, COMPONENT_TYPE::NAVIGATION)
{

}

Navigation::Navigation(const Navigation& rhs)
	: Component(rhs)
	, _currentIndex(rhs._currentIndex)
	, _cells(rhs._cells)
#ifdef _DEBUG
	, _shader(rhs._shader)
#endif // _DEBUG

{
	for (auto& pCell : _cells)
		Safe_AddRef<Cell*>(pCell);

#ifdef _DEBUG
	Safe_AddRef<Shader*>(_shader);
#endif // _DEBUG

}

HRESULT Navigation::InitializePrototype(const wstring& strNavigationFilePath)
{
	// TODO 
	// 파일 경로 읽어오면 됨 


#ifdef _DEBUG
	_shader = Shader::Create(_device, _deviceContext, TEXT("..\\Binaries\\Shaders\\Shader_Cell.fx"), VertexPos::Elements, VertexPos::VertexPosElementCount);
	if (nullptr == _shader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT Navigation::Initialize(void* pArg)
{
	return S_OK;
}

void Navigation::Update(FXMMATRIX worldMatrix)
{
	_worldMatrix = worldMatrix;

	for (auto& pCell : _cells)
		if (nullptr != pCell)
			pCell->Update(worldMatrix);
}

#ifdef _DEBUG

HRESULT Navigation::Render()
{
	if (FAILED(_shader->BindMatrix("W", &_worldMatrix)))
		return E_FAIL;

	CameraHelper* Camerahelp = GET_INSTANCE(CameraHelper);


	Matrix viewMatrix = Camerahelp->GetTransform(CameraHelper::TRANSFORMSTATE::D3DTS_VIEW);
	Matrix projMatrix = Camerahelp->GetTransform(CameraHelper::TRANSFORMSTATE::D3DTS_PROJ);

	if (FAILED(_shader->BindMatrix("V", &viewMatrix)))
		return E_FAIL;
	if (FAILED(_shader->BindMatrix("P", &projMatrix)))
		return E_FAIL;

	RELEASE_INSTANCE(CameraHelper);

	if (FAILED(_shader->Begin(0)))
		return E_FAIL;

	for (auto& pCell : _cells)
		if (nullptr != pCell)
			pCell->Render();


	return S_OK;
}

#endif // _DEBUG




HRESULT Navigation::SetUp_Neighbors()
{
	return S_OK;
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

#ifdef _DEBUG
	Safe_Release<Shader*>(_shader);
#endif // _DEBUG


	for (auto& pCell : _cells)
		Safe_Release<Cell*>(pCell);
}

#include "pch.h"
#include "Navigation.h"
#include "Cell.h"
#include "Shader.h"
#include "CameraHelper.h"

Matrix Navigation::_worldMatrix = Matrix::Identity;

Navigation::Navigation(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Component(device, deviceContext, COMPONENT_TYPE::NAVIGATION)
{

}

Navigation::Navigation(const Navigation& rhs)
	: Component(rhs)
	, _iCurrentIndex(rhs._iCurrentIndex)
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
	// ���� ��� �о���� �� 

	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(strNavigationFilePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	while (true)
	{
		Vec3		vPoints[Cell::POINT_END] = {};

		ReadFile(hFile, vPoints, sizeof(Vec3) * Cell::POINT_END, &dwByte, nullptr);

		if (0 == dwByte)
			break;

		Cell* pCell = Cell::Create(_device, _deviceContext, vPoints, _cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		_cells.push_back(pCell);
	}

	CloseHandle(hFile);


	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;

#ifdef _DEBUG
	_shader = Shader::Create(_device, _deviceContext, TEXT("..\\Binaries\\Shaders\\Shader_Cell.fx"), VertexPos::Elements, VertexPos::VertexPosElementCount);
	if (nullptr == _shader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT Navigation::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	NAVIGATION_DESC* pNeviDesc = static_cast<NAVIGATION_DESC*>(pArg);

	_iCurrentIndex = pNeviDesc->_iCurrentIndex;

	return S_OK;
}

void Navigation::Update(FXMMATRIX worldMatrix)
{
	_worldMatrix = worldMatrix;

	for (auto& pCell : _cells)
		if (nullptr != pCell)
			pCell->Update(worldMatrix);
}

_bool Navigation::IsMove(XMVECTOR vPoint)
{
	int32 iNeighborIndex = 0;

	// �������� �Ǵ��� ���´�. IsOut���� �Ǵ�.
	if (true == _cells[_iCurrentIndex]->IsOut(vPoint, ::XMLoadFloat4x4(&_worldMatrix), &iNeighborIndex))
	{
		// �̵��� ����� ���Դµ� -1�� �ƴ϶��. �̿����� �ִٰ� �Ǵ��ؼ� ����������.
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				// ���࿡ ����� -1�ε� ���Դٸ� �׳� false
				if (-1 == iNeighborIndex)
					return false;

				// �ش� ���� �������� �� ������ ���� �� ���� �ִ� �� �ٽ� �� �� �Ǵ��ϰ�, ���� �߸� �Ǵ� ������ �ִ� ��ġ�� �ִ´�.
				if (false == _cells[iNeighborIndex]->IsOut(vPoint, ::XMLoadFloat4x4(&_worldMatrix), &iNeighborIndex))
				{
					_iCurrentIndex = iNeighborIndex;
					break;
				}

			}
				return true;
		}
		else
			return false;
	}
	else
		return true;
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
	// �ڱ� �ڽ��� ���Ϳ� ��.
	// pSourCell�� A���а� pDescCell�� �ڱ� �ڽ��� A������ ���ϰ� 
	// ���� ��ġ��� �̿� �������� Ȯ���ϰ� ���� �ε����� ����.
	for (auto& pSourCell : _cells)
	{
		for (auto& pDescCell : _cells)
		{
			if (pSourCell == pDescCell)
				continue;

			if (true == pDescCell->ComparePoints(pSourCell->GetPoint(Cell::POINT_A), pSourCell->GetPoint(Cell::POINT_B)))
				pSourCell->SetUp_Neighbor(Cell::LINE_AB, pDescCell);
			else if (true == pDescCell->ComparePoints(pSourCell->GetPoint(Cell::POINT_B), pSourCell->GetPoint(Cell::POINT_C)))
				pSourCell->SetUp_Neighbor(Cell::LINE_BC, pDescCell);
			else if (true == pDescCell->ComparePoints(pSourCell->GetPoint(Cell::POINT_C), pSourCell->GetPoint(Cell::POINT_A)))
				pSourCell->SetUp_Neighbor(Cell::LINE_CA, pDescCell);

		}
	}


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

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
	// 파일 경로 읽어오면 됨 

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

_bool Navigation::IsMove(XMVECTOR& vPoint, FXMVECTOR look)
{
	int32 iNeighborIndex = 0;

	// 움직여도 되는지 묻는다. IsOut에서 판단.
	if (true == _cells[_iCurrentIndex]->IsOut(vPoint, ::XMLoadFloat4x4(&_worldMatrix), &iNeighborIndex))
	{
		// 이동의 결과가 나왔는데 -1이 아니라면. 이웃셀이 있다고 판단해서 움직여야해.
		if (-1 != iNeighborIndex)
		{
			while (true)
			{
				// 만약에 결과가 -1인데 들어왔다면 그냥 false
				if (-1 == iNeighborIndex)
					return false;

				// 해당 셀에 도착했을 때 실제로 내가 그 셀에 있는 지 다시 한 번 판단하고, 만약 잘못 판단 됐으면 있는 위치에 넣는다.
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
		// -1 일 때 동작.
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
	// 자기 자신의 벡터와 비교.
	// pSourCell의 A선분과 pDescCell의 자기 자신의 A선분을 비교하고 
	// 같은 위치라면 이웃 선분임을 확인하고 셀의 인덱스를 저장.
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

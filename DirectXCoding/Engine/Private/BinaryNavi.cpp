#include "pch.h"
#include "BinaryNavi.h"
#include "Cell.h"
#include "Shader.h"
#include "CameraHelper.h"
#include "FileUtils.h"

Matrix BinaryNavi::_worldMatrix = Matrix::Identity;

BinaryNavi::BinaryNavi(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Component(device, deviceContext, COMPONENT_TYPE::NAVIGATION)
{

}

BinaryNavi::BinaryNavi(const BinaryNavi& rhs)
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

HRESULT BinaryNavi::InitializePrototype()
{
	//// TODO 
	//// ���� ��� �о���� �� 
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(L"..\\Binaries\\Data\\Navigation.dat", FileMode::Read);

	uint32 CellSize;
	file->Read<uint32>(CellSize);

	for (uint32 i = 0; i < CellSize; ++i)
	{
		Vec3 vPoint[3] = {};

		for (uint32 j = 0; j < Cell::POINTS::POINT_END; ++j)
			file->Read<Vec3>(vPoint[j]);

		Cell* pCell = Cell::Create(_device, _deviceContext, vPoint, _cells.size());
		if (nullptr == pCell)
			return E_FAIL;

		_cells.push_back(pCell);
	}

	if (FAILED(SetUp_Neighbors()))
		return E_FAIL;
	

#ifdef _DEBUG
	_shader = Shader::Create(_device, _deviceContext, TEXT("..\\Binaries\\Shaders\\Shader_Cell.fx"), VertexPos::Elements, VertexPos::VertexPosElementCount);
	if (nullptr == _shader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT BinaryNavi::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return S_OK;

	BinaryNavi_DESC* pNeviDesc = static_cast<BinaryNavi_DESC*>(pArg);

	_iCurrentIndex = pNeviDesc->_iCurrentIndex;

	return S_OK;
}

void BinaryNavi::Update(FXMMATRIX worldMatrix)
{
	_worldMatrix = worldMatrix;

	for (auto& pCell : _cells)
		if (nullptr != pCell)
			pCell->Update(worldMatrix);
}

_bool BinaryNavi::IsMove(XMVECTOR& vPoint, FXMVECTOR look)
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
		// -1 �� �� ����.
	}
	else
		return true;
}

#ifdef _DEBUG

HRESULT BinaryNavi::Render()
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

	if (-1 == _iCurrentIndex)
	{
		Color vColor = Color(0.f, 1.f, 0.f, 1.f);

		if (FAILED(_shader->BindRawValue("vLineColor", &vColor, sizeof(Color))))
			return E_FAIL;
		
		_float fHeight = 0.2f;

		if(FAILED(_shader->BindRawValue("fHeight", &fHeight, sizeof(_float))))
			return E_FAIL;

		if (FAILED(_shader->Begin(0)))
			return E_FAIL;

		for (auto& pCell : _cells)
			if (nullptr != pCell)
				pCell->Render();
	}
	else
	{
		Color vColor = Color(1.f, 0.f, 0.f, 1.f);

		if (FAILED(_shader->BindRawValue("vLineColor", &vColor, sizeof(Color))))
			return E_FAIL;

		_float fHeight = 0.1f;

		if (FAILED(_shader->BindRawValue("fHeight", &fHeight, sizeof(_float))))
			return E_FAIL;

		if (FAILED(_shader->Begin(0)))
			return E_FAIL;

		_cells[_iCurrentIndex]->Render();
	}


	return S_OK;
}

#endif // _DEBUG

HRESULT BinaryNavi::SetUp_Neighbors()
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

void BinaryNavi::StartAStar(const int32& vGoal)
{
	Reset();

	if (_cells.empty())
		return;

	int32 iGoalIndex = vGoal;

	if (0 > _iCurrentIndex || 0 > iGoalIndex || static_cast<uint32>(_iCurrentIndex) >= _cells.size() || static_cast<uint32>(iGoalIndex) >= _cells.size())
		return;

	if (0 > _iCurrentIndex == iGoalIndex)
		return;

	if (MakeRoute(_iCurrentIndex, iGoalIndex))
		MakeBestList(_iCurrentIndex, iGoalIndex);
}

void BinaryNavi::MakeBestList(int32 iStartIndex, int32 iGoalIndex)
{
	vector<Cell*>& vecCell = _cells;

	if (vecCell.empty())
		return;

	_bestList.push_back(vecCell[iGoalIndex]);

	int32 iRouteIndex = vecCell[iGoalIndex]->GetParentIndex();

	while (true)
	{
		if (iRouteIndex == iStartIndex)
			break;

		_bestList.push_front(vecCell[iRouteIndex]);
		iRouteIndex = vecCell[iRouteIndex]->GetParentIndex();
	}
}

void BinaryNavi::Reset()
{
	_closeList.clear();
	_openList.clear();

	_bestList.clear();
}

_bool BinaryNavi::MakeRoute(int32 iStartIndex, int32 iGoalIndex)
{
	vector<Cell*>& vecCell = _cells;

	if (vecCell.empty())
		return false;

	if (!_openList.empty())
		_openList.pop_front();

	_closeList.push_back(iStartIndex);

	for (uint32 i = 0; i < Cell::LINE::LINE_END; ++i)
	{
		int32 iIndex = vecCell[iStartIndex]->GetNeighbor(static_cast<Cell::LINE>(i));
		if (iIndex != -1)
		{
			if (iGoalIndex == iIndex)
			{
				vecCell[iIndex]->SetParentIndex(iStartIndex);
				return true;
			}

			if (false == CheckClose(iIndex) && false == CheckOpen(iIndex))
			{
				vecCell[iIndex]->SetParentIndex(iStartIndex);
				_openList.push_back(iIndex);
			}
		}
	}

	if (_openList.empty())
		return false;

	int32 iOriginStart = _iCurrentIndex;

	_openList.sort([&vecCell, &iGoalIndex, &iOriginStart](int32 iDst, int32 iSrc)->bool {
		Vec3 vPCost1 = vecCell[iOriginStart]->GetPosition() - vecCell[iDst]->GetPosition();
		Vec3 vPCost2 = vecCell[iOriginStart]->GetPosition() - vecCell[iSrc]->GetPosition();

		Vec3 vGCost1 = vecCell[iGoalIndex]->GetPosition() - vecCell[iDst]->GetPosition();
		Vec3 vGCost2 = vecCell[iGoalIndex]->GetPosition() - vecCell[iSrc]->GetPosition();

		_float fCost1 = vPCost1.Length() + vGCost1.Length();
		_float fCost2 = vPCost2.Length() + vGCost2.Length();

		return fCost1 < fCost2;
		});

	return MakeRoute(_openList.front(), iGoalIndex);
}

_bool BinaryNavi::CheckClose(int32 iIndex)
{
	for (int32& iCloseIndex : _closeList)
	{
		if (iIndex == iCloseIndex)
			return true;
	}

	return false;
}

_bool BinaryNavi::CheckOpen(int32 iIndex)
{
	for (int32& iOpenIndex : _openList)
	{
		if (iIndex == iOpenIndex)
			return true;
	}

	return false;
}

BinaryNavi* BinaryNavi::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	BinaryNavi* pInstance = new BinaryNavi(device, deviceContext);

	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : BinaryNavi");
		Safe_Release<BinaryNavi*>(pInstance);
	}

	return pInstance;
}

Component* BinaryNavi::Clone(void* pArg)
{
	BinaryNavi* pInstance = new BinaryNavi(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : BinaryNavi");
		Safe_Release<BinaryNavi*>(pInstance);
	}

	return pInstance;
}

void BinaryNavi::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Release<Shader*>(_shader);
#endif // _DEBUG

	for (auto& pCell : _cells)
		Safe_Release<Cell*>(pCell);
}

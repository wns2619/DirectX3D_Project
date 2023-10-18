#include "pch.h"
#include "CellDraw.h"
#include "Cell.h"

CellDraw::CellDraw(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: GameObject(device, deviceContext, OBJECT_TYPE::TERRAIN)
{
	_modelName = "CellDraw";
}

CellDraw::CellDraw(const CellDraw& rhs)
	: GameObject(rhs)
{
}

HRESULT CellDraw::InitializePrototype()
{
	return S_OK;
}

HRESULT CellDraw::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CellDraw::Render()
{
	return S_OK;
}

void CellDraw::PriorityTick(const _float& timeDelta)
{
}

void CellDraw::Tick(const _float& fTimeDelta)
{
}

void CellDraw::LateTick(const _float& fTimeDelta)
{
}

HRESULT CellDraw::Ready_Components()
{
	return S_OK;
}

HRESULT CellDraw::Bind_ShaderResources()
{
	return S_OK;
}

GameObject* CellDraw::Clone(void* argument)
{
	CellDraw* cellDraw = new CellDraw(*this);

	if (FAILED(cellDraw->Initialize(argument)))
	{
		MSG_BOX("Failed to Clone : CellDraw");
		Safe_Release<CellDraw*>(cellDraw);
		return nullptr;
	}

	return cellDraw;
}

void CellDraw::Free()
{
	__super::Free();


	Safe_Release<Renderer*>(_pRenderer);
	Safe_Release<Shader*>(_pShader);
	Safe_Release<Cell*>(_pCell);
}

CellDraw* CellDraw::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	CellDraw* cellDraw = new CellDraw(device, deviceContext);

	if (FAILED(cellDraw->InitializePrototype()))
	{
		MSG_BOX("Failed to Create : CellDraw");
		Safe_Release<CellDraw*>(cellDraw);
		return nullptr;
	}

	return cellDraw;
}

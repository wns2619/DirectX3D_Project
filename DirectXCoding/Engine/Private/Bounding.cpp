#include "pch.h"
#include "Bounding.h"

Bounding::Bounding()
{
}

HRESULT Bounding::Initialize(BOUNDING_DESC* pDesc)
{
	return S_OK;
}

void Bounding::Free()
{
	__super::Free();
}

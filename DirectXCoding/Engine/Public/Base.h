#pragma once

#include "Defines.h"

BEGIN(Engine)

class ENGINE_DLL Base abstract
{
protected:
	Base();
	virtual ~Base() = default;

public:
	unsigned long AddRef();
	unsigned long Release();

private:
	unsigned long m_dwRefCnt = 0;

public:
	virtual void Free();
};

END
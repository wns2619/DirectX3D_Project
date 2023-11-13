#include "pch.h"
#include "..\Public\Base.h"

Base::Base()
{
}

unsigned long Base::AddRef()
{
    return ++m_dwRefCnt;
}

unsigned long Base::Release()
{
    if (0 == m_dwRefCnt)
    {
        Free();

        delete this;
        
        return 0;
    }
    else
        return m_dwRefCnt--;
}

void Base::Free()
{
}

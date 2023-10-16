#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL Navigation final : public Component 
{
private:
	explicit Navigation(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Navigation(const Navigation& rhs);
	virtual ~Navigation() = default;

public:
	virtual HRESULT InitializePrototype(const wstring& strNavigationFilePath);
	virtual HRESULT Initialize(void* pArg) override;

private:
	uint32 _currentIndex = 0;
	vector<class Cell*> _cells;

public:
	static Navigation* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wstring& strNavigationFilePath);
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};

END


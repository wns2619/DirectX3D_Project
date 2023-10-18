#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL Navigation final : public Component 
{
public:
	struct NAVIGATION_DESC
	{
		int32 _iCurrentIndex = -1;
	};

private:
	explicit Navigation(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Navigation(const Navigation& rhs);
	virtual ~Navigation() = default;

public:
	virtual HRESULT InitializePrototype(const wstring& strNavigationFilePath);
	virtual HRESULT Initialize(void* pArg) override;
	void Update(FXMMATRIX worldMatrix);

	_bool IsMove(XMVECTOR& vPoint, FXMVECTOR look);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	static Matrix _worldMatrix;
	int32 _iCurrentIndex = -1;
	vector<class Cell*> _cells;

#ifdef _DEBUG
private:
	class Shader* _shader = nullptr;
#endif // _DEBUG

private:
	HRESULT SetUp_Neighbors();


public:
	static Navigation* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wstring& strNavigationFilePath);
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};

END


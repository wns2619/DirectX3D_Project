#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL BinaryNavi final : public Component
{
public:
	struct BinaryNavi_DESC
	{
		int32 _iCurrentIndex = -1;
	};

private:
	explicit BinaryNavi(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit BinaryNavi(const BinaryNavi& rhs);
	virtual ~BinaryNavi() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg) override;
	void Update(FXMMATRIX worldMatrix);

	_bool IsMove(XMVECTOR& vPoint, FXMVECTOR look);

public:
	vector<class Cell*>& GetCell() { return _cells; }
	HRESULT SetUp_Neighbors();

	// Çª½¬ÇÒ ¶§¸¶´Ù SetUp_NeighborsÇÏ°í ·»´õ.
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

public:



public:
	static BinaryNavi* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual Component* Clone(void* pArg) override;
	virtual void Free() override;
};

END
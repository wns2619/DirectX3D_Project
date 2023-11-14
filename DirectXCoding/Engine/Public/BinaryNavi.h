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
	int32 GetCurrentIndex() { return _iCurrentIndex; }
	void SetCurrentIndex(int32 iIndex) { _iCurrentIndex = iIndex; }
public:
	vector<class Cell*>& GetCell() { return _cells; }
	HRESULT SetUp_Neighbors();

public: // A*
	void StartAStar(const int32& vGoal);
	void MakeBestList(int32 iStartIndex, int32 iGoalIndex);
	void Reset();
	_bool MakeRoute(int32 iStartIndex, int32 iGoalIndex);
	_bool CheckClose(int32 iIndex);
	_bool CheckOpen(int32 iIndex);

	// Çª½¬ÇÒ ¶§¸¶´Ù SetUp_NeighborsÇÏ°í ·»´õ.
#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private: // A*
	list<class Cell*> _bestList;
	list<int32> _openList;
	list<int32> _closeList;

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
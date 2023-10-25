#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL LandObject abstract : public GameObject
{
public:
	struct LANDOBJET_DESC
	{
		class Transform* pCellTransform = nullptr;
		vector<class Cell*>* pCells = nullptr;
	};

protected:
	explicit LandObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext, OBJECT_TYPE etype);
	explicit LandObject(const LandObject& rhs);
	virtual ~LandObject() = default;

public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT	Initialize(void* argument);
	virtual void PriorityTick(const _float& timeDelta);
	virtual void Tick(const _float& timeDelta);
	virtual void LateTick(const _float& timeDelta);
	virtual HRESULT Render();

public:
	BinaryNavi* GetNavigation() { return _pNavigation; }

protected:
	XMVECTOR SetUp_OnCell(FXMVECTOR vWorldPos, int32 currentIndex);

private:
	class Transform* _pCellTransform = nullptr;

	// 셀의 벡터를 들고와서 현재 커런트 셀을 받아서. 셀을 태우자.?
	vector<class Cell*>* _pCells = nullptr;

protected:
	class BinaryNavi* _pNavigation = nullptr;

public:
	virtual GameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};


END


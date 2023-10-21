#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL LandObject abstract : public GameObject
{
public:
	struct LANDOBJET_DESC
	{
		class Transform* pCellTransform = nullptr;
		class VIBufferCell* pCellBuffer = nullptr;
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

protected:
	XMVECTOR SetUp_OnCell(FXMVECTOR vWorldPos);

private:
	class Transform* _pCellTransform = nullptr;
	class VIBufferCell* _pCellBuffer = nullptr;

public:
	virtual GameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};


END


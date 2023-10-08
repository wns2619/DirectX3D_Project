#pragma once

#include "GameObject.h"

BEGIN(Engine)
class Shader;
class Renderer;
END

BEGIN(Client)


// 해당 파일의 정보를 읽어들이자..
class EmptyObject final : public GameObject
{
private:
	explicit EmptyObject(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit EmptyObject(const EmptyObject& rhs);
	virtual ~EmptyObject() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& timeDelta)		override;
	virtual void LateTick(const _float& timeDelta)	override;
	virtual HRESULT Render()						override;

public:
	
private:
	Renderer* _render = nullptr;
	Shader* _shader = nullptr;

	

public:
	static EmptyObject* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* argument) override;
	virtual void Free() override;
};


END


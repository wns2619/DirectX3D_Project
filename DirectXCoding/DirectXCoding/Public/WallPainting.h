#pragma once

#include "StaticObject.h"

BEGIN(Engine)
class Texture;
class VIBufferRect;
class Shader;
class Renderer;
END

BEGIN(Client)

class WallPainting final : public StaticObject
{
public:
	enum PAINT_TYPE { SHOOT, EVIL, HUMAN, WAY, ART, PAINT_END };


private:
	explicit WallPainting(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit WallPainting(const WallPainting& rhs);
	virtual ~WallPainting() = default;

public:
	virtual HRESULT InitializePrototype()			override;
	virtual HRESULT Initialize(void* pArg)			override;
	virtual void Tick(const _float& timeDelta)		override;
	virtual void LateTick(const _float& timeDelta)	override;
	virtual HRESULT Render()						override;

public:
	static WallPainting* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual GameObject* Clone(void* argument) override;
	virtual void Free() override;
};

END

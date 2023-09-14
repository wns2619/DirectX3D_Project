#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL Shader final : public Component
{
private:
	explicit Shader(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Shader(const Shader& rhs);
	virtual ~Shader() = default;

public:
	virtual HRESULT InitializePrototype(const wstring& shaderPath, const D3D11_INPUT_ELEMENT_DESC* pElements, uint32 iNumElements);
	virtual HRESULT Initialize(void* argument) override;

public:
	HRESULT Begin(uint32 passIndex);
	HRESULT	BindMatrix(const _char* constantName, const Matrix* matrix) const ;
	HRESULT BindMatrices(const _char* constantName, const Matrix* matrix, uint32 numMatrices) const;
	HRESULT BindTexture(const _char* constantName, ID3D11ShaderResourceView* srv) const;
	HRESULT BindTextures(const _char* constantName, ID3D11ShaderResourceView** srv, uint32 numTextures) const;

private:
	ID3DX11Effect*				_effect = nullptr;
	vector<ID3D11InputLayout*> _inputLayouts;

public:
	static Shader* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wstring& shaderPath, const D3D11_INPUT_ELEMENT_DESC* pElements, uint32 iNumElements);
	virtual Component* Clone(void* agument) override;
	virtual void Free() override;
};

END


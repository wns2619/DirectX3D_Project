#include "pch.h"
#include "Shader.h"

Shader::Shader(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	: Component(device, deviceContext)
{
}

Shader::Shader(const Shader& rhs)
	: Component(rhs)
	, _effect(rhs._effect)
	, _inputLayouts(rhs._inputLayouts)
{
	for (auto& pInputLayout : _inputLayouts)
		Safe_AddRef<ID3D11InputLayout*>(pInputLayout);

	Safe_AddRef<ID3DX11Effect*>(_effect);
}

HRESULT Shader::InitializePrototype(const wstring& shaderPath, const D3D11_INPUT_ELEMENT_DESC* pElements, uint32 iNumElements)
{
	uint32 shaderFlag = 0;

#if _DEBUG
	shaderFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	shaderFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif // _DEBUG


	if (FAILED(D3DX11CompileEffectFromFile(shaderPath.data(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, shaderFlag, 0, _device, &_effect, nullptr)))
		return E_FAIL;

	ID3DX11EffectTechnique* technique = _effect->GetTechniqueByIndex(0);
	if (nullptr == technique)
		return E_FAIL;

	D3DX11_TECHNIQUE_DESC	techniqueDesc;
	technique->GetDesc(&techniqueDesc);
	
	for (uint32 i = 0; i < techniqueDesc.Passes; ++i)
	{
		ID3DX11EffectPass* pass = technique->GetPassByIndex(i);

		D3DX11_PASS_DESC passDesc;

		pass->GetDesc(&passDesc);

		ID3D11InputLayout* Inputlayout = nullptr;

		if (FAILED(_device->CreateInputLayout(pElements, iNumElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, &Inputlayout)))
			return E_FAIL;

		_inputLayouts.push_back(Inputlayout);
	}

	return S_OK;
}

HRESULT Shader::Initialize(void* argument)
{
	return S_OK;
}

HRESULT Shader::Begin(uint32 passIndex)
{
	if (passIndex >= _inputLayouts.size())
		return E_FAIL;

	_deviceContext->IASetInputLayout(_inputLayouts[passIndex]);

	ID3DX11EffectTechnique* technique = _effect->GetTechniqueByIndex(0);
	if (nullptr == technique)
		return E_FAIL;

	technique->GetPassByIndex(passIndex)->Apply(0, _deviceContext);

	return S_OK;
}

HRESULT Shader::BindRawValue(const _char* constantName, const void* data, uint32 legnth, uint32 offset)
{
	ID3DX11EffectVariable* variable = _effect->GetVariableByName(constantName);

	if (nullptr == variable)
		return E_FAIL;

	return variable->SetRawValue(data, offset, legnth);
}

HRESULT Shader::BindMatrix(const _char* constantName, const Matrix* matrix) const
{
	ID3DX11EffectVariable* variable = _effect->GetVariableByName(constantName);
	if (nullptr == variable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* matrixvariable = variable->AsMatrix();
	if (nullptr == matrixvariable)
		return E_FAIL;

	return matrixvariable->SetMatrix(reinterpret_cast<const _float*>(matrix));
}

HRESULT Shader::BindMatrices(const _char* constantName, const Matrix* matrix, uint32 numMatrices) const
{
	ID3DX11EffectVariable* variable = _effect->GetVariableByName(constantName);
	if (nullptr == variable)
		return E_FAIL;

	ID3DX11EffectMatrixVariable* matrixVariable = variable->AsMatrix();
	if (nullptr == matrixVariable)
		return E_FAIL;

	return matrixVariable->SetMatrixArray(reinterpret_cast<const _float*>(matrix), 0, numMatrices);
}

HRESULT Shader::BindTexture(const _char* constantName, ID3D11ShaderResourceView* srv) const
{
	ID3DX11EffectVariable* variable = _effect->GetVariableByName(constantName);
	if (nullptr == variable)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* resourceVariable = variable->AsShaderResource();
	if (nullptr == resourceVariable)
		return E_FAIL;

	return resourceVariable->SetResource(srv);
}

HRESULT Shader::BindTextures(const _char* constantName, ID3D11ShaderResourceView** srv, uint32 numTextures) const
{
	ID3DX11EffectVariable* variable = _effect->GetVariableByName(constantName);
	if (nullptr == variable)
		return E_FAIL;
	
	ID3DX11EffectShaderResourceVariable* resourceVariable = variable->AsShaderResource();
	if (nullptr == resourceVariable)
		return E_FAIL;

	return resourceVariable->SetResourceArray(srv, 0, numTextures);
}


Shader* Shader::Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wstring& shaderPath, const D3D11_INPUT_ELEMENT_DESC* pElements, uint32 iNumElements)
{
	Shader* shader = new Shader(device, deviceContext);

	if (FAILED(shader->InitializePrototype(shaderPath, pElements, iNumElements)))
	{
		MSG_BOX("Failed Create Shader");
		Safe_Release<Shader*>(shader);
		return nullptr;
	}

	return shader;
}

Component* Shader::Clone(void* agument)
{
	Shader* shader = new Shader(*this);

	if (FAILED(shader->Initialize(agument)))
	{
		MSG_BOX("Failed Create Cloned : Shader");
		Safe_Release<Shader*>(shader);
		return nullptr;
	}

	return shader;
}

void Shader::Free()
{
	__super::Free();

	for (auto& iter : _inputLayouts)
		Safe_Release<ID3D11InputLayout*>(iter);

	Safe_Release<ID3DX11Effect*>(_effect);
}

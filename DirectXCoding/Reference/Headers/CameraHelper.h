#pragma once

#include "Defines.h"

BEGIN(Engine)

class Transform;

class CameraHelper : public Base
{
	DECLARE_SINGLETON(CameraHelper)

public:
	enum class TRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

private:
	CameraHelper();
	virtual ~CameraHelper() = default;

public:
	void SetTransform(TRANSFORMSTATE state, FXMMATRIX trasnsformMatrix);
	Matrix GetTransform(TRANSFORMSTATE state)						const;
	FXMMATRIX GetTransformCalculator(TRANSFORMSTATE state)			const;
	Matrix GetInverseTransform(TRANSFORMSTATE state)				const;
	FXMMATRIX GetInverseTransformCalculator(TRANSFORMSTATE state)	const;
	XMVECTOR GetCameraCaculator()									const;
	Vec4 GetCameraPosition()										const;

public:
	HRESULT BindTransformToShader(Shader* shader, const char* constantName, CameraHelper::TRANSFORMSTATE state);
	HRESULT BindCameraPosition(Shader* shader, const _char* constantName, uint32 legnth);

public:
	void Tick();

private:
	Matrix _transformMatrices[static_cast<uint32>(TRANSFORMSTATE::D3DTS_END)];
	Matrix _transformInverseMatrices[static_cast<uint32>(TRANSFORMSTATE::D3DTS_END)];
	Vec4   _camPosition = { 0.f,0.f,0.f,1.f };

public:
	virtual void Free() override;
};

END


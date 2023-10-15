#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL BinaryBone final : public Base
{
public:
	struct BONE_DESC
	{
		// 부모 기준으로 표현된 내 뼈의 상태변환 행렬
		// 애니메이션에 저장된 정보대로 갱신해야하는 행렬.
		Matrix _transformationMatrix;

		// 부모의 본 기준 행렬의 상태?
		Matrix _combinedTransformationMatrix;

		// 본인의 부모의 뼈 인덱스, -1은 부모 없는 단독뼈 or 제일 처음 기준이 되는 뼈. root 뼈 .
		int32 _parentBoneIndex = -1;
	};

private:
	BinaryBone();
	explicit BinaryBone(const BinaryBone& rhs);
	virtual ~BinaryBone() = default;

public:
	const char* GetBoneName() const { return _szName; }
	const Matrix GetCombinedTransformMatrix() const { return _BoneDesc._combinedTransformationMatrix; }
	const XMMATRIX GetCombinedTransformCaculator() const { return ::XMLoadFloat4x4(&_BoneDesc._combinedTransformationMatrix); }
	int32 GetBoneIndex() { return _BoneDesc._parentBoneIndex; }

	void SetTransformationMatrix(FXMMATRIX transformationMatrix) { ::XMStoreFloat4x4(&_BoneDesc._transformationMatrix, transformationMatrix); }

	Matrix GetTransformMatrix() { return _BoneDesc._transformationMatrix; }
	Matrix* GetCombientTransformMatrixPoint() { return &_BoneDesc._combinedTransformationMatrix; }

	BONE_DESC& GetBoneDesc() { return _BoneDesc; }

public:
	HRESULT Initialize(const BONE_DESC boneInfo, const _char* nameData, int32 parentBoneIndex);
	HRESULT UpdateCombinedTransformMatrix(const vector<class BinaryBone*>& bones);

private:
	_char _szName[MAX_PATH] = "";

	BONE_DESC _BoneDesc = {};

public:
	static BinaryBone* Cretae(BONE_DESC boneInfo, const _char* nameData, int32 parentBoneIndex);
	BinaryBone* Clone();
	virtual void Free() override;
};


END


#pragma once

#include "Base.h"

BEGIN(Engine)

class Bone final : public Base
{
private:
	Bone();
	virtual ~Bone() = default;

public:
	HRESULT Initialize(const aiNode* node, int32 parentBoneIndex);
	HRESULT UpdateCombinedTransformMatrix(const vector<class Bone*>& bones);

private:
	_char _szName[MAX_PATH] = "";

	// 부모 기준으로 표현된 내 뼈의 상태변환 행렬
	// 애니메이션에 저장된 정보대로 갱신해야하는 행렬.
	Matrix _transformationMatrix;

	// 부모의 본 기준 행렬의 상태?
	Matrix _combinedTransformationMatrix;

	// 본인의 부모의 뼈 인덱스, -1은 부모 없는 단독뼈 or 제일 처음 기준이 되는 뼈. root 뼈 .
	int32 _parentBoneIndex = -1;

public:
	static Bone* Create(const aiNode* node, int32 parentBoneIndex);
	virtual void Free() override;
};


END


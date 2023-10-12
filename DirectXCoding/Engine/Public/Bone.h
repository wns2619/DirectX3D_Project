#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL Bone final : public Base
{
public:
	struct BONE_DESC
	{
		// �θ� �������� ǥ���� �� ���� ���º�ȯ ���
		// �ִϸ��̼ǿ� ����� ������� �����ؾ��ϴ� ���.
		Matrix _transformationMatrix;

		// �θ��� �� ���� ����� ����?
		Matrix _combinedTransformationMatrix;

		// ������ �θ��� �� �ε���, -1�� �θ� ���� �ܵ��� or ���� ó�� ������ �Ǵ� ��. root �� .
		int32 _parentBoneIndex = -1;
	};

private:
	Bone();
	explicit Bone(const Bone& rhs);
	virtual ~Bone() = default;

public:
	const char* GetBoneName() const { return _szName; }
	Matrix GetCombinedTransformMatrix() const { return _BoneDesc._combinedTransformationMatrix; }
	const XMMATRIX GetCombinedTransformCaculator() const { return ::XMLoadFloat4x4(&_BoneDesc._combinedTransformationMatrix); }
	int32 GetBoneIndex() { return _BoneDesc._parentBoneIndex; }
	
	void SetTransformationMatrix(FXMMATRIX transformationMatrix) { ::XMStoreFloat4x4(&_BoneDesc._transformationMatrix, transformationMatrix); }

	Matrix GetTransformMatrix() { return _BoneDesc._transformationMatrix; }

	BONE_DESC& GetBoneDesc() { return _BoneDesc; }
public:
	HRESULT Initialize(const aiNode* node, int32 parentBoneIndex);
	HRESULT UpdateCombinedTransformMatrix(const vector<class Bone*>& bones);

private:
	_char _szName[MAX_PATH] = "";

	BONE_DESC _BoneDesc = {};

public:
	static Bone* Create(const aiNode* node, int32 parentBoneIndex);
	Bone* Clone();
	virtual void Free() override;
};


END


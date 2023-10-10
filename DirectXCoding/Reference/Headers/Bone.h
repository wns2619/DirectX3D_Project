#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL Bone final : public Base
{
private:
	Bone();
	explicit Bone(const Bone& rhs);
	virtual ~Bone() = default;

public:
	const char* GetBoneName() const { return _szName; }
	const Matrix GetCombinedTransformMatrix() const { return _combinedTransformationMatrix; }
	const XMMATRIX GetCombinedTransformCaculator() const { return ::XMLoadFloat4x4(&_combinedTransformationMatrix); }
	int32 GetBoneIndex() { return _parentBoneIndex; }
	
	void SetTransformationMatrix(FXMMATRIX transformationMatrix) { ::XMStoreFloat4x4(&_transformationMatrix, transformationMatrix); }

	Matrix GetTransformMatrix() { return _transformationMatrix; }
public:
	HRESULT Initialize(const aiNode* node, int32 parentBoneIndex);
	HRESULT UpdateCombinedTransformMatrix(const vector<class Bone*>& bones);

private:
	_char _szName[MAX_PATH] = "";

	// �θ� �������� ǥ���� �� ���� ���º�ȯ ���
	// �ִϸ��̼ǿ� ����� ������� �����ؾ��ϴ� ���.
	Matrix _transformationMatrix;

	// �θ��� �� ���� ����� ����?
	Matrix _combinedTransformationMatrix;

	// ������ �θ��� �� �ε���, -1�� �θ� ���� �ܵ��� or ���� ó�� ������ �Ǵ� ��. root �� .
	int32 _parentBoneIndex = -1;

public:
	static Bone* Create(const aiNode* node, int32 parentBoneIndex);
	Bone* Clone();
	virtual void Free() override;
};


END


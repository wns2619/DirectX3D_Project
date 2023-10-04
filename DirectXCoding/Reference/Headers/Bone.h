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

	// �θ� �������� ǥ���� �� ���� ���º�ȯ ���
	// �ִϸ��̼ǿ� ����� ������� �����ؾ��ϴ� ���.
	Matrix _transformationMatrix;

	// �θ��� �� ���� ����� ����?
	Matrix _combinedTransformationMatrix;

	// ������ �θ��� �� �ε���, -1�� �θ� ���� �ܵ��� or ���� ó�� ������ �Ǵ� ��. root �� .
	int32 _parentBoneIndex = -1;

public:
	static Bone* Create(const aiNode* node, int32 parentBoneIndex);
	virtual void Free() override;
};


END


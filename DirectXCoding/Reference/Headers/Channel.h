#pragma once


// Channel�� Ư�� �ִϸ��̼��� ����ϴ� ���� �����̴�.

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL Channel final : public Base
{
private:
	Channel();
	virtual ~Channel() = default;

public:
	HRESULT Initialize(const class Model* pModel, const aiNodeAnim* pAIChannel);
	void UpdateTransformationMatrix(uint32* pCurrentKeyFrame, vector<class Bone*>& Bones, _float trackPosition);

private:
	_char _szName[MAX_PATH] = ""; // ���� �̸�.
	uint32 _numKeyFrames = 0;

	uint32 _boneIndex = 0;

	vector<KEYFRAME> _keyFrames;

public:
	static Channel* Create(const class Model* pModel, const aiNodeAnim* pAIChannel);
	virtual void Free() override;
};

END


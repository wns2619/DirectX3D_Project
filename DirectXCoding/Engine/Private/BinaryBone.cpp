#include "pch.h"
#include "BinaryBone.h"

BinaryBone::BinaryBone()
{
}

BinaryBone::BinaryBone(const BinaryBone& rhs)
	: _BoneDesc(rhs._BoneDesc)
{
	::strcpy_s(_szName, rhs._szName);
}

HRESULT BinaryBone::Initialize(BONE_DESC boneInfo, const _char* nameData, int32 parentBoneIndex)
{
	_BoneDesc._parentBoneIndex = parentBoneIndex;

	// �ҷ��� ���� �̸��� �����Ѵ�. ���� �̸��� ���� ������ Ȱ��ȴ�.
	::strcpy_s(_szName, nameData);

	// �ִϸ��̼� ������ ����� ���� ����� �����Ѵ�.
	::memcpy(&_BoneDesc._transformationMatrix, &boneInfo._transformationMatrix, sizeof(Matrix));

	// ��Ʈ������ �ҷ��� �� ������ Column major�̴�. �׷��� row major�� �ٲٱ� ���� ��ġ��ķ� �ٲ㼭 ������.
	//::XMStoreFloat4x4(&_BoneDesc._transformationMatrix, ::XMMatrixTranspose(::XMLoadFloat4x4(&_BoneDesc._transformationMatrix)));

	return S_OK;
}

HRESULT BinaryBone::UpdateCombinedTransformMatrix(const vector<class BinaryBone*>& bones)
{
	if (-1 == _BoneDesc._parentBoneIndex)
		_BoneDesc._combinedTransformationMatrix = _BoneDesc._transformationMatrix;
	else
	{
		::XMStoreFloat4x4(&_BoneDesc._combinedTransformationMatrix,
			::XMLoadFloat4x4(&_BoneDesc._transformationMatrix) *
			::XMLoadFloat4x4(&bones[_BoneDesc._parentBoneIndex]->_BoneDesc._combinedTransformationMatrix));

		

		if (strcmp("slider", _szName) == 0)
			_vSliderPos = _BoneDesc._combinedTransformationMatrix;
	}
		
	// �θ� ���� ��� �θ��� ��� �������� ������ ��ġ�� �����Ǿ���.
	// ���� ��ġ * �θ� ����� ���ϸ� �� ��ġ�� ��������. ������ ��ġ ������ ������ �θ��� ��ġ�� �� ������ ��ġ��.
	// ���� ������ ���¸� combine ��Ʈ������ ����.

	return S_OK;
}

BinaryBone* BinaryBone::Cretae(BONE_DESC boneInfo, const _char* nameData, int32 parentBoneIndex)
{
	BinaryBone* bone = new BinaryBone();

	if (FAILED(bone->Initialize(boneInfo, nameData, parentBoneIndex)))
	{
		MSG_BOX("Failed to Created : Bone");
		Safe_Release<BinaryBone*>(bone);
	}

	return bone;
}

BinaryBone* BinaryBone::Clone()
{
	return new BinaryBone(*this);
}

void BinaryBone::Free()
{
	__super::Free();
}

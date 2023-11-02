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

	// 불러온 뼈의 이름을 저장한다. 뼈의 이름은 여러 곳에서 활용된다.
	::strcpy_s(_szName, nameData);

	// 애니메이션 정보가 저장된 뼈의 행렬을 저장한다.
	::memcpy(&_BoneDesc._transformationMatrix, &boneInfo._transformationMatrix, sizeof(Matrix));

	// 매트릭스를 불러올 때 정보가 Column major이다. 그래서 row major로 바꾸기 위해 전치행렬로 바꿔서 저장함.
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
		
	// 부모가 있을 경우 부모의 행렬 기준으로 본인의 위치가 지정되야함.
	// 본인 위치 * 부모 행렬을 곱하면 그 위치가 정해진다. 본인의 위치 정보가 없으면 부모의 위치가 곧 본인의 위치임.
	// 최종 본인의 상태를 combine 매트릭스에 저장.

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

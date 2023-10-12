#include "pch.h"
#include "Bone.h"

Bone::Bone()
{
}

Bone::Bone(const Bone& rhs)
    : _BoneDesc(rhs._BoneDesc)
{
    ::strcpy_s(_szName, rhs._szName);
}

HRESULT Bone::Initialize(const aiNode* node, int32 parentBoneIndex)
{
    // �θ��� �ε����� ���ڷ� �޴´�.
    _BoneDesc._parentBoneIndex = parentBoneIndex;

    // �ҷ��� ���� �̸��� �����Ѵ�. ���� �̸��� ���� ������ Ȱ��ȴ�.
    ::strcpy_s(_szName, node->mName.data);

    // �ִϸ��̼� ������ ����� ���� ����� �����Ѵ�.
    ::memcpy(&_BoneDesc._transformationMatrix, &node->mTransformation, sizeof(Matrix));

    // ��Ʈ������ �ҷ��� �� ������ Column major�̴�. �׷��� row major�� �ٲٱ� ���� ��ġ��ķ� �ٲ㼭 ������.
    ::XMStoreFloat4x4(&_BoneDesc._transformationMatrix, ::XMMatrixTranspose(::XMLoadFloat4x4(&_BoneDesc._transformationMatrix)));
    _BoneDesc._combinedTransformationMatrix = Matrix::Identity;

    return S_OK;
}

HRESULT Bone::UpdateCombinedTransformMatrix(const vector<class Bone*>& bones)
{
    // -1�� ��� �θ���� �ܵ� ���� ���. ������ ��������� �� ���� ������.
    if (-1 == _BoneDesc._parentBoneIndex)
        _BoneDesc._combinedTransformationMatrix = _BoneDesc._transformationMatrix;
    else
        ::XMStoreFloat4x4(&_BoneDesc._combinedTransformationMatrix,
            ::XMLoadFloat4x4(&_BoneDesc._transformationMatrix) *
            ::XMLoadFloat4x4(&bones[_BoneDesc._parentBoneIndex]->_BoneDesc._combinedTransformationMatrix));

    // �θ� ���� ��� �θ��� ��� �������� ������ ��ġ�� �����Ǿ���.
    // ���� ��ġ * �θ� ����� ���ϸ� �� ��ġ�� ��������. ������ ��ġ ������ ������ �θ��� ��ġ�� �� ������ ��ġ��.
    // ���� ������ ���¸� combine ��Ʈ������ ����.

    return S_OK;
}

Bone* Bone::Create(const aiNode* node, int32 parentBoneIndex)
{
    Bone* bone = new Bone();

    if (FAILED(bone->Initialize(node, parentBoneIndex)))
    {
        MSG_BOX("Failed to Created : Bone");
        Safe_Release<Bone*>(bone);
    }

    return bone;
}

Bone* Bone::Clone()
{
    return new Bone(*this);
}

void Bone::Free()
{
    __super::Free();
}

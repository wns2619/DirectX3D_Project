#include "pch.h"
#include "Bone.h"

Bone::Bone()
{
}

HRESULT Bone::Initialize(const aiNode* node, int32 parentBoneIndex)
{
    // 부모의 인덱스를 인자로 받는다.
    _parentBoneIndex = parentBoneIndex;

    // 불러온 뼈의 이름을 저장한다. 뼈의 이름은 여러 곳에서 활용된다.
    ::strcpy_s(_szName, node->mName.data);

    // 애니메이션 정보가 저장된 뼈의 행렬을 저장한다.
    ::memcpy(&_transformationMatrix, &node->mTransformation, sizeof(Matrix));

    // 매트릭스를 불러올 때 정보가 Column major이다. 그래서 row major로 바꾸기 위해 전치행렬로 바꿔서 저장함.
    ::XMStoreFloat4x4(&_transformationMatrix, ::XMMatrixTranspose(::XMLoadFloat4x4(&_transformationMatrix)));

    return S_OK;
}

HRESULT Bone::UpdateCombinedTransformMatrix(const vector<class Bone*>& bones)
{
    // -1인 경우 부모없이 단독 뼈일 경우. 본인의 행렬정보가 곧 최종 정보임.
    if (-1 == _parentBoneIndex)
        _combinedTransformationMatrix = _transformationMatrix;
    else
        ::XMStoreFloat4x4(&_combinedTransformationMatrix,
            ::XMLoadFloat4x4(&_transformationMatrix) * 
            ::XMLoadFloat4x4(&bones[_parentBoneIndex]->_combinedTransformationMatrix));

    // 부모가 있을 경우 부모의 행렬 기준으로 본인의 위치가 지정되야함.
    // 본인 위치 * 부모 행렬을 곱하면 그 위치가 정해진다. 본인의 위치 정보가 없으면 부모의 위치가 곧 본인의 위치임.
    // 최종 본인의 상태를 combine 매트릭스에 저장.

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

void Bone::Free()
{
    __super::Free();
}

#pragma once

#include "Component.h"
#include "State.h"

BEGIN(Engine)

class ENGINE_DLL Animator final : public Component 
{
private:
	explicit Animator(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	explicit Animator(const Animator& rhs);
	virtual ~Animator() = default;

public:
	HRESULT	Initialize(void* pArg);
	HRESULT InitializePrototype();

	virtual void UpdateAnimator(const _float& timeDelta, State::STATE eState);
	virtual void LateUpdateAnimator(const _float& timeDelta);

public:
	void SetAnimator(State::STATE eState);
	class BinaryAnimation* GetAnimation() { return _pCurAnimation; }

public:
	HRESULT AddAnimation(State::STATE eState, class BinaryAnimation* pAnimation, 
		vector<class BinaryBone*>* pBone = nullptr, _bool loop = false);
	HRESULT ChangeAnimation(_float duration, const _float& timeDelta, State::STATE eState);

private:
	map<State::STATE, class BinaryAnimation*> _mapAnimation;

	class BinaryAnimation* _pCurAnimation = nullptr;
	class BinaryAnimation* _pPrevAnimation = nullptr;
	vector<class BinaryBone*>* _pBinaryBone = nullptr;

	State::STATE _eCurState;
	State::STATE _ePrevState;

	_bool _bAnimationChange = false;
	_bool _bFirstSetting = true;

	_bool _IsLoop = false;
	_float _fChangeTrackPosition = 0.f;
public:
	static Animator* Create(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	virtual Component* Clone(void* pArg);
	virtual void Free() override;
};


END


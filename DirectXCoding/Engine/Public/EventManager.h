#pragma once

#include "Base.h"

BEGIN(Engine)

class EventManager final : public Base
{
	DECLARE_SINGLETON(EventManager)

public:
	enum EVENT_TYPE { CREATE_OBJECT, DELETE_OBJECT, EVENT_END };


	struct EVENT_DESC
	{
		EVENT_TYPE eEvent;
		DWORD_PTR lParam;
		DWORD_PTR wParam;
	};

private:
	EventManager();
	virtual ~EventManager() = default;

public:
	void Tick();

	void AddEvent(const EVENT_DESC& _eve) { _vecEvent.push_back(_eve); }

	void CreateObject(class GameObject* pObj, LAYER_TAG _eLayer);
	void DeleteObject(class GameObject* pObj);

public:
	void Execute(const EVENT_DESC& _eve);

private:
	vector<EVENT_DESC> _vecEvent;
	vector<GameObject*> _vecCreate;
	vector<GameObject*> _vecDead;

public:
	virtual void Free() override;
};

END


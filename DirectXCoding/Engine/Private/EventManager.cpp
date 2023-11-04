#include "pch.h"
#include "EventManager.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(EventManager)

EventManager::EventManager()
{
}

void EventManager::Tick()
{
	for (size_t i = 0; i < _vecDead.size(); i++)
		Safe_Release<GameObject*>(_vecDead[i]);

	_vecDead.clear();

	for (size_t i = 0; i < _vecEvent.size(); i++)
		Execute(_vecEvent[i]);

	_vecEvent.clear();
}

void EventManager::CreateObject(GameObject* pObj, LAYER_TAG _eLayer)
{
	EVENT_DESC event = {};
	event.eEvent = EVENT_TYPE::CREATE_OBJECT;
	event.lParam = (DWORD_PTR)pObj;
	event.wParam = (DWORD_PTR)_eLayer;

	AddEvent(event);
}

void EventManager::DeleteObject(GameObject* pObj)
{
	EVENT_DESC event = {};
	event.eEvent = EVENT_TYPE::DELETE_OBJECT;
	event.lParam = (DWORD_PTR)pObj;

	AddEvent(event);
}

void EventManager::Execute(const EVENT_DESC& _eve)
{
	switch (_eve.eEvent)
	{
	case EVENT_TYPE::CREATE_OBJECT:
	{
		GameObject* pNewObject = (GameObject*)_eve.lParam;
		LAYER_TAG eType = (LAYER_TAG)_eve.wParam;

		GameInstance* pGameInstance = GET_INSTANCE(GameInstance);
		ObjectManager* pObjectManager = GET_INSTANCE(ObjectManager);
		
		uint32 iCurrentLevel = pGameInstance->GetCurrentLevelIndex();

		Layer* pLayer = pObjectManager->FindLayer(iCurrentLevel, eType);

		if (nullptr == pLayer)
		{
			pLayer = Layer::Create();

			pLayer->AddGameObject(pNewObject);
			pLayer->SetLayerTag(eType);

			pGameInstance->GetEntireObjectLayer()[iCurrentLevel].emplace(eType, pLayer);
		}
		else
			pLayer->AddGameObject(pNewObject);

		RELEASE_INSTANCE(GameInstance);
		RELEASE_INSTANCE(ObjectManager);
	}
		break;
	case EVENT_TYPE::DELETE_OBJECT:
	{
		GameObject* pDeadObject = (GameObject*)_eve.lParam;

		pDeadObject->SetDead(true);
		_vecDead.push_back(pDeadObject);
	}
		break;
	default:
		break;
	}
}

void EventManager::Free()
{
	__super::Free();
}

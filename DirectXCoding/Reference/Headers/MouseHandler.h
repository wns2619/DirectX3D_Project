#pragma once

#include "Base.h"

BEGIN(Engine)

enum class MouseEventType { LPress, LRelease, RPress, RRelease, MPress, MRelease, WheelUp, WheelDown, Move, RawMove, Invalid };

struct MousePoint
{
	int32 x;
	int32 y;
};

struct MouseEvent
{
	MouseEventType type;
	MousePoint point;
	
public:
	MouseEvent()
	{
		type = MouseEventType::Invalid;
		point.x = 0;
		point.y = 0;
	}
	explicit MouseEvent(MouseEventType type, int32 x, int32 y)
	{
		this->type = type;
		this->point.x = x;
		this->point.y = y;
	}
};


class ENGINE_DLL MouseHandler : public Base
{
private:
	queue<MouseEvent> _eventBuffer;
	_bool _leftIsDown;
	_bool _rightIsDown;
	_bool _middleIsDown;
	MousePoint _position;

public:
	MouseHandler()
	{
		_leftIsDown = false;
		_rightIsDown = false;
		_middleIsDown = false;
		_position.x = 0;
		_position.y = 0;
	}

	// Mouse Click
	void onLeftPressed(int32 x, int32 y)
	{
		_leftIsDown = true;
		_eventBuffer.push(MouseEvent(MouseEventType::LPress, x, y));
	}
	void onLeftReleased(int32 x, int32 y)
	{
		_leftIsDown = false;
		_eventBuffer.push(MouseEvent(MouseEventType::LRelease, x, y));
	}
	void onRightPressed(int32 x, int32 y)
	{
		_rightIsDown = true;
		_eventBuffer.push(MouseEvent(MouseEventType::RPress, x, y));
	}
	void onRightReleased(int32 x, int32 y)
	{
		_rightIsDown = false;
		_eventBuffer.push(MouseEvent(MouseEventType::RRelease, x, y));
	}
	void onMiddlePressed(int32 x, int32 y)
	{
		_middleIsDown = true;
		_eventBuffer.push(MouseEvent(MouseEventType::MPress, x, y));
	}
	void onMiddleReleased(int32 x, int32 y)
	{
		_middleIsDown = false;
		_eventBuffer.push(MouseEvent(MouseEventType::MRelease, x, y));
	}

	// Wheel Scrolling
	void onWheelUp(int32 x, int32 y)
	{
		_eventBuffer.push(MouseEvent(MouseEventType::WheelUp, x, y));
	}
	void onWheelDown(int32 x, int32 y)
	{
		_eventBuffer.push(MouseEvent(MouseEventType::WheelDown, x, y));
	}
	void onMouseMove(int32 x, int32 y)
	{
		_position.x = x;
		_position.y = y;
		_eventBuffer.push(MouseEvent(MouseEventType::Move, x, y));
	}
	void onMouseRawMove(int32 x, int32 y)
	{
		_eventBuffer.push(MouseEvent(MouseEventType::RawMove, x, y));
	}

	_bool isLeftDown() { return _leftIsDown; }
	_bool isRightDown() { return _rightIsDown; }
	_bool isMiddleDown() { return _middleIsDown; }

	// Get Mouse Position
	int32 getPosY() { return _position.y; }
	int32 getPosX() { return _position.x; }
	MousePoint getPos() { return _position; }

	_bool eventBufferIsEmpty() { return _eventBuffer.empty(); }
	MouseEvent readEvent()
	{
		if (_eventBuffer.empty())
			return MouseEvent();
		else
		{
			MouseEvent mouseEvent = _eventBuffer.front();
			_eventBuffer.pop();
			return mouseEvent;
		}
	}

public:
	virtual void Free() override
	{
		__super::Free();
	}
};

END
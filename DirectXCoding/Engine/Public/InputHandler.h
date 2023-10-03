#pragma once

#include "Base.h"
#include "KeyBoardHandler.h"
#include "MouseHandler.h"

BEGIN(Engine)

class ENGINE_DLL InputHandler : public Base
{
	DECLARE_SINGLETON(InputHandler)

private:
	InputHandler();
	virtual ~InputHandler() = default;

public:
	_bool keyBufferIsEmpty() { return _keyBoard.keyBufferIsEmpty(); }
	_bool charBufferIsEmpty() { return _keyBoard.charBufferIsEmpty(); }

	_bool keyIsPressed(const _ubyte key) { return _keyBoard.keyIsPressed(key); }
	KeyBoardEvent readKey() { return _keyBoard.readKey(); }
	_ubyte readChar() { return _keyBoard.readChar(); }

	void onKeyPressed(const _ubyte key) { _keyBoard.onKeyPressed(key); }
	void onKeyReleased(const _ubyte key) { _keyBoard.onKeyReleased(key); }
	void onChar(const _ubyte key) { _keyBoard.onChar(key); }

	void enableAutoRepeatKeys() { _keyBoard.enableAutoRepeatKeys(); }
	void disableAutoRepeatKeys() { _keyBoard.disableAutoRepeatKeys(); }
	_bool isAutoRepeatingKeys() { return _keyBoard.isAutoRepeatingKeys(); }

	void enableAutoRepeatChars() { _keyBoard.enableAutoRepeatChars(); }
	void disableAutoRepeatChars() { _keyBoard.disableAutoRepeatChars(); }
	_bool isAutoRepeatingChars() { return _keyBoard.isAutoRepeatingChars(); }

	// Mouse Pass Through
	_bool mouseBufferIsEmpty() { return _mouse.eventBufferIsEmpty(); }
	void onMouseMove(int32 x, int32 y) { _mouse.onMouseMove(x, y); }
	void onMouseRawMove(int32 x, int32 y) { _mouse.onMouseRawMove(x, y); }

	MouseEvent readMouseEvent() { return _mouse.readEvent(); }
	_bool isMouseLeftDown() { return _mouse.isLeftDown(); }
	_bool isMouseRightDown() { return _mouse.isRightDown(); }
	_bool isMouseMiddleDown() { return _mouse.isMiddleDown(); }

	void onLeftPressed(int32 x, int32 y) { _mouse.onLeftPressed(x, y); }
	void onRightPressed(int32 x, int32 y) { _mouse.onRightPressed(x, y); }
	void onMiddlePressed(int32 x, int32 y) { _mouse.onMiddlePressed(x, y); }

	void onLeftReleased(int32 x, int32 y) { _mouse.onLeftReleased(x, y); }
	void onRightReleased(int32 x, int32 y) { _mouse.onRightReleased(x, y); }
	void onMiddleReleased(int32 x, int32 y) { _mouse.onMiddleReleased(x, y); }

	void onWheelUp(int32 x, int32 y) { _mouse.onWheelUp(x, y); }
	void onWheelDown(int32 x, int32 y) { _mouse.onWheelDown(x, y); }

private:
	KeyBoardHandler _keyBoard;
	MouseHandler _mouse;

public:
	virtual void Free() override;
};

END

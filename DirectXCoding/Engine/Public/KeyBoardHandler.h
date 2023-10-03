#pragma once


BEGIN(Engine)

enum class KeyboardEventType { Press, Release, Invalid };

struct KeyBoardEvent
{
	KeyboardEventType type;
	_ubyte key;
	
public:
	KeyBoardEvent()
	{
		type = KeyboardEventType::Invalid;
		key = 0u;
	}
	explicit KeyBoardEvent(KeyboardEventType type, _ubyte key)
	{
		this->type = type;
		this->key = key;
	}
};


class ENGINE_DLL KeyBoardHandler : public Base
{
private:
	_bool _autoRepeatKeys;
	_bool _autoRepeatChars;
	_bool _keyStates[256];
	queue<KeyBoardEvent> _keyBuffer;
	queue<_ubyte> _charBuffer;

public:
	KeyBoardHandler()
	{
		_autoRepeatKeys = false;
		_autoRepeatChars = false;
		for (size_t i = 0; i < 256; i++)
			_keyStates[i] = false;
	}

	// Is Buffer Empty
	_bool keyBufferIsEmpty() { return _keyBuffer.empty(); }
	_bool charBufferIsEmpty() { return _charBuffer.empty(); }

	// Read
	_bool keyIsPressed(const _ubyte key) { return _keyStates[key]; }
	KeyBoardEvent readKey()
	{
		if (_keyBuffer.empty())
			return KeyBoardEvent();
		else
		{
			KeyBoardEvent event = _keyBuffer.front();
			_keyBuffer.pop();
			return event;
		}
	}
	_ubyte readChar()
	{
		if (_charBuffer.empty())
			return 0u;
		else
		{
			_ubyte eventChar = _charBuffer.front();
			_charBuffer.pop();
			return eventChar;
		}
	}

	// On Event
	void onKeyPressed(const _ubyte key)
	{
		_keyStates[key] = true;
		_keyBuffer.push(KeyBoardEvent(KeyboardEventType::Press, key));
	}
	void onKeyReleased(const _ubyte key)
	{
		_keyStates[key] = true;
		_keyBuffer.push(KeyBoardEvent(KeyboardEventType::Release, key));
	}
	void onChar(const _ubyte key)
	{
		_charBuffer.push(key);
	}

	// Auto Repeat keys
	void enableAutoRepeatKeys() { _autoRepeatKeys = true; }
	void disableAutoRepeatKeys() { _autoRepeatKeys = false; }
	_bool isAutoRepeatingKeys() { return _autoRepeatKeys; }

	// Auto Repeat Chars
	void enableAutoRepeatChars() { _autoRepeatChars = true; }
	void disableAutoRepeatChars() { _autoRepeatChars = false; }
	_bool isAutoRepeatingChars() { return _autoRepeatChars; }

public:
	virtual void Free() override
	{
		__super::Free();
	}
};

END
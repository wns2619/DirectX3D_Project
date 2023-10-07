#pragma once


enum FileMode : uint8
{
	Write,
	Read,
};


class FileUtils
{
public:
	FileUtils();
	~FileUtils();

public:
	void Open(wstring filePath, FileMode mode); // 핸들을 만들어서 데이터를 꺼내쓴다.
	
	// 참조 값으로 넣고, 데이터를 넣어주겠다.
	template<typename T>
	void Write(const T& data)
	{
		DWORD numOfBytes = 0;
		assert(::WriteFile(_handle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr));
	}

	template<> // 템플릿 특수화 
	void Write<string>(const string& data)
	{
		return Write(data);
	}

	void Write(void* data, uint32 dataSize);
	void Write(const string& data);

	template<typename T>
	void Read(OUT T& data)
	{
		DWORD numOfBytes = 0;
		assert(::ReadFile(_handle, &data, sizeof(T), (LPDWORD)&numOfBytes, nullptr));
	}

	template<typename T>
	T Read()
	{
		T data;
		Read(data);
		return data;
	}

	void Read(void** data, uint32 dataSize);
	void Read(OUT string& data);

private:
	HANDLE _handle = INVALID_HANDLE_VALUE;
};


#pragma once

namespace Engine
{
	class CTag_Finder
	{
	public:
		explicit CTag_Finder(const wchar_t* pTag) : m_pTargetTag(pTag) {}
		~CTag_Finder() {};

	public:
		template<typename T>
		bool operator()(const T& pair)
		{
			if (0 == lstrcmpW(m_pTargetTag, pair.first))
				return true;

			return false;
		}

	private:
		const wchar_t* m_pTargetTag = nullptr;
	};


	class DeleteMap
	{
	public:
		DeleteMap() = default;
		~DeleteMap() = default;
		
	public:
		template<typename T>
		void operator()(T& pair)
		{
			uint32 dwRefCnt = 0;
			
			dwRefCnt = pair.second->Release();

			if (0 == dwRefCnt)
				pair.second = nullptr;
		}
	};
}
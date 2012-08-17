#include "ThreadTypes.h"

namespace sqr
{
	struct CCallBackData
	{
		uint64	m_uBeginTime;
		uint64	m_uSpendTime;
		uint64	m_uCount;
	};

	class CCallBackProfiler
	{
	public:
		CCallBackProfiler();
		~CCallBackProfiler();

		void Init(HTHREADID TID);

		static void StartCallBackProfiler();
		static void StopCallBackProfiler();

		void OpenProfileFile(HTHREADID TID);
		void WriteProfileFile();

		void ProfileBegin( const char* szFunName );
		void ProfileEnd( const char* szFunName );

		bool IsOpen();

	private:
		bool	m_bOpen;
		wchar_t	 m_szProfileFileName[256];
		FILE*	m_fdProfile;
		uint64	m_uAllSpendTime;

		typedef map<string, CCallBackData*> MapCallBackData;
		MapCallBackData m_mapCallBackData;

		typedef map<HTHREADID, CCallBackProfiler*> MapCallBackProfiler;
		static MapCallBackProfiler m_mapCallBackProfiler;
	};

}


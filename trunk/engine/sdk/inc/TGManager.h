#pragma once

#include "CDynamicObject.h"
#include "TSingleton.h"
#include "ThinkBase.h"
#include "CLock.h"

extern "C"
{
	#include "tg/thinkgear.h"
}
#include <vector>

#define Min_PkgVal_Num 20

// Think gear manager
namespace sqr
{
	class TGManager : public virtual CDynamicObject, public Singleton<TGManager>
	{
	public:
		TGManager();
		~TGManager();

		// init the think gear set
		void		InitTGManager(string Pt_Name);
		// get think gear data
		float		GetThinkgearData(int Tg_Data);

		
		// set think gear baud rate
		int			SetThinkgearBaudRate(const int Rate);
		// set data format
		int			SetThinkgearDataFormat(const int DFormat);

	private:
		// start think gear tick
		bool		StartThinkgear(const int baud_rate = TG_BAUD_9600, const int stream_format = TG_STREAM_PACKETS );
		// end think gear tick
		bool		EndThinkgear();
		// do some init before connect to the think gear
		bool		PreStartThinkgear(const int baud_rate, const int stream_format );
		// set the port name can be used
		void		SetThePortNameUsed(string Pt_Name);
		// end the connect all
		bool		TotalEndThinkgear();

		// thread used
		int 							Begin();
		void							End();
		static void						EntryProc( void* pArg);
		int								TGDataRefresh();
		void							TGDataToRefResh();
		void							Execute();

		bool				m_beTGRun;			// is the think gear started

		// var for think gear connect
		string				m_comPortName;		// Port name
		int					m_dllVersion;		// dll version
		int					m_connectionId;		// connection id

		int					m_baud_rate;		// 波特率
		int					m_stream_format;	// 流结构

		TGData				m_bufData;			// 缓存上次读取数据


		// thread used
		HTHREAD				m_hThread;
		CLock				m_ConBeginEnd;
		uint64				m_LastNoDataTime;
		bool				m_beSuc;
	};

}
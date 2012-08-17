#include "stdafx.h"
#include "ErrLogHelper.h"
#include "StringHelper.h"
#include "ThreadHelper.h"
#include "TimeHelper.h"

#include "TGManager.h"
TGManager::TGManager() 
: m_beTGRun(false)
, m_dllVersion(0)
, m_connectionId(-1)
, m_baud_rate(TG_BAUD_9600)
, m_stream_format(TG_STREAM_PACKETS)
, m_hThread(0)
, m_LastNoDataTime(0)
{
	m_comPortName = "\\\\.\\COM1";
}

TGManager::~TGManager() 
{
	if (m_hThread)
	{
		End();
	}
	if (m_connectionId != -1)
	{
		TotalEndThinkgear();
	}
}


// 初始化设备
void TGManager::InitTGManager(string Pt_Name)
{
	SetThePortNameUsed(Pt_Name);
}

// 设置访问端口名称 
void TGManager::SetThePortNameUsed(string Pt_Name)
{
	m_comPortName = Pt_Name;
}

// 这个时候已经新建了链接ID
bool TGManager::StartThinkgear(const int baud_rate, const int stream_format )
{
	PreStartThinkgear(baud_rate, stream_format );

	//string	errmsg;
	int		errCode = 0;
	if ( m_connectionId == -1 )
	{
		if ( !PreStartThinkgear(baud_rate, stream_format ) )
		{
			return false;
		}

	}

	static bool stat = false;

	if (!stat)
	{
		errCode = TG_Connect( m_connectionId, m_comPortName.c_str(), baud_rate, stream_format );
		if( errCode < 0 ) 
		{
			// Attempt to connect error
			m_connectionId = -1;
			//errmsg =format("TG_Connect() returned %d.", errCode);
			//cout << errmsg << endl;
			return false;
		}
		m_beTGRun = true;
	}

	return m_beTGRun;

}



// 开始 think gear 数据读取
bool TGManager::PreStartThinkgear(const int baud_rate, const int stream_format )
{
	// 保存波特率和流格式
	m_baud_rate = baud_rate;		
	m_stream_format = stream_format;

	if (m_connectionId != -1)
	{
		TotalEndThinkgear();
	}

	string	errmsg;
	int		errCode = 0;
	// 获取 dll 版本号
	m_dllVersion = TG_GetDriverVersion();

	// 获取到 ThinkGear 的链接 ID 
	m_connectionId = TG_GetNewConnectionId();
	if( m_connectionId < 0 ) 
	{
		// 非法的 connection id
		m_connectionId = -1;
		errmsg =format("TG_GetNewConnectionId() returned %d.", m_connectionId);
		cout << errmsg << endl;
		return false;
	}

	// 设置并打开流 log file 
	errCode = TG_SetStreamLog( m_connectionId, "streamLog.txt" );
	if( errCode < 0 ) 
	{
		// 流 log file error
		m_connectionId = -1;
		errmsg =format("TG_SetStreamLog() returned %d.", errCode);
		cout << errmsg << endl;
		return false;
	}

	// 打开并设置 data (ThinkGear values) log file for connection 
	errCode = TG_SetDataLog( m_connectionId, "dataLog.txt" );
	if( errCode < 0 ) 
	{
		// 数据 log file error
		m_connectionId = -1;
		errmsg =format("TG_SetDataLog() returned %d.", errCode);
		cout << errmsg << endl;
		return false;
	}

	// Attempt to connect the connection ID handle to serial port "COM5" 
	if (m_comPortName.length() < 0)
	{
		m_comPortName = "\\\\.\\COM1";
	}

	return true;
}


// 结束Think grea 读取状态
bool TGManager::EndThinkgear()
{
	m_beTGRun = false;
	if (m_connectionId != -1)
	{
		TG_Disconnect( m_connectionId );
	}

	return true;
}

// 彻底结束链接
bool TGManager::TotalEndThinkgear()
{
	if ( m_connectionId != -1 )
	{
		EndThinkgear();
		TG_FreeConnection( m_connectionId );
	}
	
	m_connectionId = -1;
	return true;
}


// 获取已经读取的数据
// -1.0f 数据没有更新，没有获取到数据
// -2.0f 设备出现故障，无法恢复
float TGManager::GetThinkgearData(int Tg_Data)
{
	if ( !m_hThread )
	{
		if ( Begin() == -1 )
		{
			return	-2.0f; // 设备不可用
		}
		
	}

	if ( !m_beSuc )
	{
		return -2.0f;
	}

	float TG_value = -1.0f;
	// 读取当前链接的所有数据
	if ( Tg_Data == TG_DATA_BATTERY )
	{
		TG_value = m_bufData.TG_Battery;
	}
	else if ( Tg_Data == TG_DATA_POOR_SIGNAL )
	{
		TG_value = m_bufData.TG_PoorSignal;
	}
	else if ( Tg_Data == TG_DATA_ATTENTION )
	{
		TG_value = m_bufData.TG_Attention;
	}
	else if ( Tg_Data == TG_DATA_MEDITATION )
	{
		TG_value = m_bufData.TG_Meditation;
	}
	else if ( Tg_Data == TG_DATA_RAW )
	{
		TG_value = m_bufData.TG_Raw;
	}
	else if ( Tg_Data == TG_DATA_DELTA )
	{
		TG_value = m_bufData.TG_Delta;
	}
	else if ( Tg_Data == TG_DATA_THETA )
	{
		TG_value = m_bufData.TG_Theta;
	}
	else if ( Tg_Data == TG_DATA_ALPHA1 )
	{
		TG_value = m_bufData.TG_Alpha1;
	}
	else if ( Tg_Data == TG_DATA_ALPHA2 )
	{
		TG_value = m_bufData.TG_Alpha2;
	}
	else if ( Tg_Data == TG_DATA_BETA1 )
	{
		TG_value = m_bufData.TG_Beta1;
	}
	else if ( Tg_Data == TG_DATA_BETA2 )
	{
		TG_value = m_bufData.TG_Beta2;
	}
	else if ( Tg_Data == TG_DATA_GAMMA1 )
	{
		TG_value = m_bufData.TG_Gamma1;
	}
	else if ( Tg_Data == TG_DATA_GAMMA2 )
	{
		TG_value = m_bufData.TG_Gamma2;
	}

	return TG_value;
}

/*

-1 if connectionId does not refer to a valid ThinkGear Connection ID handle.
-2 if serialBaudrate is not a valid TG_BAUD_* value.
-3 if an error occurs attempting to set the baud rate.
-4 if connectionId is connected to an input file stream instead of an actual ThinkGear COM stream.
0  on success. 

*/
int TGManager::SetThinkgearBaudRate(const int Rate)
{
	return TG_SetBaudrate(m_connectionId, Rate);
}


/*

-1 if connectionId does not refer to a valid ThinkGear Connection ID handle.
-2 if serialDataFormat is not a valid TG_STREAM_* type.
0  on success.

*/
int TGManager::SetThinkgearDataFormat(const int DFormat)
{
	return TG_SetDataFormat(m_connectionId, DFormat);
}


int TGManager::Begin()
{
	m_ConBeginEnd.Lock();

	if( !m_beTGRun )
	{
		m_beSuc = true;
		sqr::CreateThread( &m_hThread, 512, &TGManager::EntryProc, this );
	}

	m_ConBeginEnd.Unlock();

	if (!m_hThread)
		return -1;
	return 0;
}

void TGManager::End()
{
	m_ConBeginEnd.Lock();

	if( m_hThread )
	{
		TerminateThread(&m_hThread, 0);
		m_hThread =0;
	}

	m_ConBeginEnd.Unlock();
}

// thread used
void TGManager::EntryProc( void* pArg)
{
	SetThreadName("ThinkGearThread");

	return static_cast<TGManager*>( pArg )->Execute();
}

// thread 
void TGManager::Execute()
{	
	// 开启TG
	m_beSuc = false;
	if ( StartThinkgear(TG_BAUD_9600, TG_STREAM_PACKETS ) )
	{
		// 更新
		m_beSuc = true;
		TGDataRefresh();
	}
	// 结束TG
	m_beSuc = false;
	TotalEndThinkgear();
	m_hThread = 0;
}

// 更新数据
int TGManager::TGDataRefresh()
{
	if ( m_connectionId == -1 )
	{
		if ( !StartThinkgear() )
		{
			m_beSuc = false;
			return -2;
		}
	}

	float TG_value = -1.0f;
	int errCode = 0;

	// 读取当前链接的所有数据
	while (true)
	{
		errCode = TG_ReadPackets( m_connectionId, 1 );
		if( errCode >= 1 ) 
		{
			m_LastNoDataTime = 0;
			TGDataToRefResh();
		}
		else if ( errCode == -1 || errCode == -3 )		// connect id 是不合法的
		{
			m_LastNoDataTime = 0;
			TotalEndThinkgear();
			if ( !StartThinkgear(m_baud_rate, m_stream_format ) )
			{
				m_beSuc = false;
				return -2;		// 无法恢复链接，设备出现故障
			}
			else
			{
				// 读取当前链接的所有数据
				errCode = TG_ReadPackets( m_connectionId, 1 );
				if( errCode >= 1 ) 
				{
					TGDataToRefResh();
				}
				// end of reread
			}
		}
		else if ( errCode == -2 )	// 如果设备断开了链接或者没有戴正确
		{
			if (m_LastNoDataTime == 0)
			{
				m_LastNoDataTime = GetProcessTime();
			}
			else
			{
				uint64 uCurTime = GetProcessTime();
				if (uCurTime - m_LastNoDataTime > 1600)
				{
					// 数据清0
					m_bufData.InitData();
					m_LastNoDataTime = 0;
				}
			}
		}
	}
	
	return 0;
}

void TGManager::TGDataToRefResh()
{
	if( TG_GetValueStatus(m_connectionId, TG_DATA_BATTERY) != 0 ) 
	{
		m_bufData.TG_Battery = TG_GetValue(m_connectionId, TG_DATA_BATTERY);
	} 

	if( TG_GetValueStatus(m_connectionId, TG_DATA_POOR_SIGNAL) != 0 ) 
	{
		m_bufData.TG_PoorSignal = TG_GetValue(m_connectionId, TG_DATA_POOR_SIGNAL);
	} 

	if( TG_GetValueStatus(m_connectionId, TG_DATA_ATTENTION) != 0 ) 
	{
		m_bufData.TG_Attention = TG_GetValue(m_connectionId, TG_DATA_ATTENTION);
	} 

	if( TG_GetValueStatus(m_connectionId, TG_DATA_MEDITATION) != 0 ) 
	{
		m_bufData.TG_Meditation = TG_GetValue(m_connectionId, TG_DATA_MEDITATION);
	} 

	if( TG_GetValueStatus(m_connectionId, TG_DATA_RAW) != 0 ) 
	{
		m_bufData.TG_Raw = TG_GetValue(m_connectionId, TG_DATA_RAW);
	} 

	if( TG_GetValueStatus(m_connectionId, TG_DATA_DELTA) != 0 ) 
	{
		m_bufData.TG_Delta = TG_GetValue(m_connectionId, TG_DATA_DELTA);
	} 

	if( TG_GetValueStatus(m_connectionId, TG_DATA_THETA) != 0 ) 
	{
		m_bufData.TG_Theta = TG_GetValue(m_connectionId, TG_DATA_THETA);
	} 

	if( TG_GetValueStatus(m_connectionId, TG_DATA_ALPHA1) != 0 ) 
	{
		m_bufData.TG_Alpha1 = TG_GetValue(m_connectionId, TG_DATA_ALPHA1);
	} 

	if( TG_GetValueStatus(m_connectionId, TG_DATA_ALPHA2) != 0 ) 
	{
		m_bufData.TG_Alpha2 = TG_GetValue(m_connectionId, TG_DATA_ALPHA2);
	} 

	if( TG_GetValueStatus(m_connectionId, TG_DATA_BETA1) != 0 ) 
	{
		m_bufData.TG_Beta1 = TG_GetValue(m_connectionId, TG_DATA_BETA1);
	} 

	if( TG_GetValueStatus(m_connectionId, TG_DATA_BETA2) != 0 ) 
	{
		m_bufData.TG_Beta2 = TG_GetValue(m_connectionId, TG_DATA_BETA2);
	} 

	if( TG_GetValueStatus(m_connectionId, TG_DATA_GAMMA1) != 0 ) 
	{
		m_bufData.TG_Gamma1 = TG_GetValue(m_connectionId, TG_DATA_GAMMA1);
	} 

	if( TG_GetValueStatus(m_connectionId, TG_DATA_GAMMA2) != 0 ) 
	{
		m_bufData.TG_Gamma2 = TG_GetValue(m_connectionId, TG_DATA_GAMMA2);
	} 

}
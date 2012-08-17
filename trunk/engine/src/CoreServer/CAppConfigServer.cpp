#include "stdafx.h"
#include "CAppConfigServer.h"
#include "TAppConfig.inl"
#include "CMultiLangTxtTableFile.h"

template class TAppConfig<CSyncTraitsServer>;

CAppConfigServer::CAppConfigServer(istream& strmConfig)
:ParentConfig_t(strmConfig)
{
	m_uFollowerDelay= DftGet<uint32>( uint32(0) , "follower_delay" );

	m_uAppSleepTime = DftGet<uint32>( 33 , "app_sleep_time" );

	m_uConnTimeOut = Get<uint32>( "Conn_Time_Out" );
	m_uTripleBufferDataNum = Get<uint32>( "TripleBuffer_Init_DataNum" );
	m_uClientTimeStampErrorThreshold = Get<uint32>( "ClientTimeStampErrorThreshold" );

	m_fMaxStepCommandRate = Get<float>( "MaxStepCommandRate" );
	m_szLanFolder = Get<const char*>( "language_folder" );
	m_szOriLanFolder = Get<const char*>( "original_language_folder" );
	m_bReload = Get<bool>( "reload" );
	m_nMegaLogInterval = DftGet<int32>(-1, "mega_log_interval");
	m_uMegaLogMaxSize = DftGet<uint32>(1024*1024*20, "mega_log_maxsize");
	m_bTestingServer = Get<bool>("TestingServer");

	SetCurLangPath(m_szLanFolder);
	SetOriLangPath(m_szOriLanFolder);
}

CAppConfigServer::~CAppConfigServer(void)
{
}

uint32 CAppConfigServer::GetFollowerDelay()const
{
	return m_uFollowerDelay;
}

uint32 CAppConfigServer::GetConnServerTimeOut()const
{
	return m_uConnTimeOut;
}

uint32 CAppConfigServer::GetLogMemInterval()const
{
	return DftGet<uint32>(10*1000,"log_mem_interval");
}

uint32 CAppConfigServer::GetTripleBufferInitDataNum() const
{
	return m_uTripleBufferDataNum;
}

void CAppConfigServer::SetClientTimeStampErrorThreshold(uint32 uClientTimeStampErrorThreshold)
{
	m_uClientTimeStampErrorThreshold = uClientTimeStampErrorThreshold;
}

void CAppConfigServer::SetMaxStepCommandRate(float fMaxStepCommandRate)
{
	m_fMaxStepCommandRate = fMaxStepCommandRate;
}

uint32 CAppConfigServer::GetClientTimeStampErrorThreshold() const
{
	return m_uClientTimeStampErrorThreshold;
}

float CAppConfigServer::GetMaxStepCommandRate() const
{
	return m_fMaxStepCommandRate;
}

bool CAppConfigServer::InfiniteLoopDetectionEnabled()const
{
	bool bCheckInfiniteLoop;

#ifdef _WIN32
	bCheckInfiniteLoop = false;
#else
	bCheckInfiniteLoop = Get<bool>("check_infinite_loop" );
#endif

	return bCheckInfiniteLoop;
}

string sqr::CAppConfigServer::GetOriLanFolder() const
{
	return m_szOriLanFolder;
}

string sqr::CAppConfigServer::GetLanFolder() const
{
	return m_szLanFolder;
}

uint32 CAppConfigServer::GetAppSleepTime()const
{
	return m_uAppSleepTime;
}

//bool CAppConfigServer::GetCheckSkillValueSafeState()const
//{
// return m_bCheckSkillValueSafe;
//}

bool CAppConfigServer::GetReload() const
{
	return m_bReload;
}

int32 CAppConfigServer::MegaLogInterval() const
{
	return m_nMegaLogInterval;
}

uint32 CAppConfigServer::MegaLogMaxFileSize() const
{
	return m_uMegaLogMaxSize;
}

bool CAppConfigServer::TestingServerEnabled() const
{
	return m_bTestingServer;
}

uint32 CAppConfigServer::GetChildVMGCStep()const
{
	return DftGet<uint32>( 1, "collectgarbage" , "child_vm" , "step" );
}

uint32 CAppConfigServer::GetChildVMGCStepInterval()const
{
	return DftGet<uint32>( 1, "collectgarbage" , "child_vm" , "interval" );
}

float CAppConfigServer::GetChildVMGCCallbackRatio()const
{
	return DftGet<float>( 1, "collectgarbage" , "child_vm" , "callback_ratio" );
}

float CAppConfigServer::GetChildVMGCNormalUnpackRatio()const
{
	return DftGet<float>( 5, "collectgarbage" , "child_vm" ,"normal_unpack_ratio" );
}

float CAppConfigServer::GetChildVMGCSerialUnpackRatio()const
{
	return DftGet<float>( 30, "collectgarbage" , "child_vm" ,"serial_unpack_ratio" );
}



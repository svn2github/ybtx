#include "stdafx.h"
#include "LogImp/CLogImp.h"
#include <io.h>
#include <fcntl.h>
#include "../CEditToolApp.h"
#include "CEditorConfig.h"

DEF_UI_IMP(CLogCtrlImp);
CLogCtrlImp::CLogCtrlImp(void* param)
{
}

CLogCtrlImp::~CLogCtrlImp(void)
{

}

void CLogCtrlImp::ShowLog(const string& log)
{
	m_Log += log;
	Update();
}

const string& CLogCtrlImp::GetLog(void)
{
	return m_Log;
}

void CLogCtrlImp::Clear(void)
{
	m_Log.clear();
}

const int kBufferSize = 1024;
FILE* fpout;
FILE* fpin;

DEF_SINGLE_UI_IMP(CSystemLogImp);
CSystemLogImp::CSystemLogImp(void)
: CLogCtrlImp(NULL)
{
	char buf[64];

	time_t T;
	struct tm * timenow;
	time ( &T );
	timenow = localtime ( &T ); 

	sprintf(buf, "/%d-%02d-%02d_%02d-%02d-%02d.log", 
		timenow->tm_year + 1900, timenow->tm_mon + 1, 
		timenow->tm_mday, timenow->tm_hour,
		timenow->tm_min, timenow->tm_sec);

	m_logFile = CEditorConfig::GetInst()->GetEditorAppPath() + buf;

	fpout	= fopen(m_logFile.c_str(), "wt");
	fpin	= fopen(m_logFile.c_str(), "rb");
	
	*stdout = *fpout;
	std::ios_base::sync_with_stdio();
	puts("========系统日志========");

	CEditToolApp::GetInst()->RegisterTick( this, 60 );
}
	
CSystemLogImp::~CSystemLogImp(void)
{
	fclose(fpout);
	fclose(fpin);
	CEditToolApp::GetInst()->UnRegisterTick(this);
}

void	CSystemLogImp::OnTick(void)
{
	char buf[kBufferSize + 1];
	int bytes_read;

	bool isUpdate = false;
	fflush(stdout);	
	while(1)
	{
		bytes_read = fread(buf, 1,kBufferSize, fpin);
		if(bytes_read<=0)
		{ 
			if(isUpdate)Update();
			return;
		}
		buf[bytes_read] = 0;
		m_Log += buf;
		isUpdate = true;
	}
}
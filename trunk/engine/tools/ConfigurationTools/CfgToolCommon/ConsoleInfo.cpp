#include "stdafx.h"
#include "ConsoleInfo.h"

CConsoleInfo::CConsoleInfo()
{
	if (BeXpSystem())
	{
		m_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		COORD cord;
		cord.X = 80;
		cord.Y = 2500;
		SetConsoleScreenBufferSize(m_hOut,cord);
	}
}

CConsoleInfo::~CConsoleInfo()
{
	CloseHandle(m_hOut);
}

void CConsoleInfo::BeginToPrint(const char* szFileName)
{
	m_uStartTime = GetCurrentTime();
	printf("%-40s", szFileName);
}

void CConsoleInfo::EndToPrint()
{
	uint64 uTotalTime = GetCurrentTime() - m_uStartTime;
	printf("\t%d\n", uTotalTime);
}

void CConsoleInfo::PrintFunction(const string& strFun)
{
	if (!BeXpSystem())
	{
		printf("%-40s\n", strFun.c_str());
	}
	else
	{
		CONSOLE_SCREEN_BUFFER_INFO bInfo;
		GetConsoleScreenBufferInfo(m_hOut,&bInfo);
		int chNum = strFun.length();
		int textCursionY = bInfo.dwCursorPosition.Y+3;
		int x1 = 0;
		int y1 = textCursionY-2;
		int x2 = bInfo.dwSize.X-1;
		int y2 = y1 + 5;
		int xText = (bInfo.dwSize.X - chNum)/2;
		WORD att0 = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE |
			FOREGROUND_INTENSITY |  BACKGROUND_BLUE;
		COORD posShadow = {x1+1,y1+1} ,posTextBack = {x1,y1};

		for (int i=0;i<5;++i)
		{
			FillConsoleOutputAttribute(m_hOut,att0,bInfo.dwSize.X,posTextBack,NULL);
			posTextBack.Y++;
		}
		SMALL_RECT RC = {x1,y1,x2,y2-1};
		DrawBox(RC);
		COORD posText = {xText,y1+2};
		WriteConsoleOutputCharacter(m_hOut,strFun.c_str(),chNum,posText,NULL);

		COORD curPos;
		curPos.X = 0;
		curPos.Y = bInfo.dwCursorPosition.Y + 3;
		SetConsoleCursorPosition(m_hOut,curPos);
		cout<<endl<<endl<<endl;

	}
}

void CConsoleInfo::DrawBox(SMALL_RECT &rc)
{
	char chBox[6];
	chBox[0] = '|';
	chBox[1] = '|';
	chBox[2] =	'|';
	chBox[3] =	'|';
	chBox[4] =	'-';
	chBox[5] =	'|';
	COORD pos = {rc.Left,rc.Top};
	WriteConsoleOutputCharacter(m_hOut,&chBox[0],1,pos,NULL);
	for (pos.X = rc.Left+1;pos.X < rc.Right;++pos.X)
	{
		WriteConsoleOutputCharacter(m_hOut,&chBox[4],1,pos,NULL);
	}
	WriteConsoleOutputCharacter(m_hOut,&chBox[1],1,pos,NULL);
	for (pos.Y = rc.Top+1;pos.Y<rc.Bottom;++pos.Y)
	{
		pos.X = rc.Left;
		WriteConsoleOutputCharacter(m_hOut,&chBox[5],1,pos,NULL);
		pos.X = rc.Right;
		WriteConsoleOutputCharacter(m_hOut,&chBox[5],1,pos,NULL);
	}
	pos.X = rc.Left;
	pos.Y = rc.Bottom;
	WriteConsoleOutputCharacter(m_hOut,&chBox[2],1,pos,NULL);
	for (pos.X = rc.Left+1;pos.X < rc.Right;++pos.X)
	{
		WriteConsoleOutputCharacter(m_hOut,&chBox[4],1,pos,NULL);
	}
	pos.X = rc.Right;
	WriteConsoleOutputCharacter(m_hOut,&chBox[3],1,pos,NULL);
}

bool CConsoleInfo::BeXpSystem()
{
	DWORD winVer;
	winVer = GetVersion();
	OSVERSIONINFO osvi;
	memset(&osvi,0,sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (winVer < 0x80000000)
	{
		GetVersionEx(&osvi);
		if (osvi.dwMajorVersion == 5L && osvi.dwMinorVersion == 1L)
			return true;
	}
	return false;
}

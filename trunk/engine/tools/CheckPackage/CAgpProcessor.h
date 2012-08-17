#pragma once
#include <string>
#include "iresprocessormgr.h"
#include "GraphicBase.h"

class CAniGroup
{
	string						m_IndexSkeName;
	vector<IDNAME>				m_AnimationList;		//所有动画列表
public:
	int BinLoad( string FileName, CBaseResProcessor* pProcessor );
};

class CAgpProcessor : public CBaseResProcessor
{
public:
	CAgpProcessor(void);
	virtual ~CAgpProcessor(void);

	virtual bool			RunProcess( const TCHAR* szFilename );
	virtual size_t			GetCountCanBeProcess(){ return 1; };
	virtual unsigned long	GetTypeCanBeProcess( size_t nNum ){ return 'agp'; }
};
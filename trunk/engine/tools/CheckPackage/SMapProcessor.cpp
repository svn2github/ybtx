#include "StdAfx.h"
#include "SMapProcessor.h"


bool SMapProcessor::RunProcess( const char* szFilename )
{
	CBaseResProcessor::RunProcess(szFilename);

	// вснд╪Ч
	string sChild = szFilename;
	size_t npos = sChild.find("common/");
	if( npos != -1 )
	{
		npos = sChild.find("/");
		if( npos != -1 )
		{
			sChild = sChild.substr(npos+1, sChild.length());
			sChild = "server/" + sChild;
		}
	}
	sChild.erase( sChild.size() - 4, 4 );

	m_pMgr->ProcessWith(( sChild + "_s.arg" ).c_str(), szFilename);

	return true;
}

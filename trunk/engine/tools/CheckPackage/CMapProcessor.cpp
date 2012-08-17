#include "StdAfx.h"
#include "CMapProcessor.h"
#include "GraphicCommon.h"

bool CMapProcessor::RunProcess( const char* szFilename )
{
	FILE* fp;
	ChangeToSrcDir();
	fp = SureOpenFile( szFilename, "rb" );

	fseek(fp,0x0,SEEK_SET);

	int nVersion;
	fread(&nVersion, sizeof(int),1,fp); // magic number
	fread(&nVersion, sizeof(int),1,fp);

	if ( nVersion > CLIENT_AMP_VERSION )
	{
		ostringstream strm;
		strm<< szFilename << "ÎÄ¼þ°æ±¾´íÎó\n";
		GenErr(strm.str());
	}

	fseek(fp,0x8f,SEEK_SET);

	for ( int i = 0; i < 4; ++i )
	{
		fseek(fp,0x38,SEEK_CUR);
		char bufFileName[256];
		memset(bufFileName,0,256);
		fread(bufFileName, 64,1,fp);

		m_pMgr->ProcessWith(bufFileName,szFilename); // sky texture
	}

	fseek(fp,0x26f,SEEK_SET);

	int nFileNameLen;
	char bufFileName[256];
	memset(bufFileName,0,256);
	fread(bufFileName, 64,1,fp);

	m_pMgr->ProcessWith(bufFileName,szFilename); // sky back texture

	fseek(fp,0x2c3,SEEK_SET);
	memset(bufFileName,0,256);
	fread(&nFileNameLen, sizeof(int),1 ,fp); // wave
	fread(bufFileName, nFileNameLen,1,fp);
	m_pMgr->ProcessWith(bufFileName,szFilename);

	fclose(fp);

	string sChild = szFilename;
	size_t npos = sChild.find("common/");
	if( npos != -1 )
	{
		npos = sChild.find("/");
		if( npos != -1 )
			sChild = sChild.substr(npos+1, sChild.length());
	}
	
	sChild.erase( sChild.size() - 4, 4 );
	m_pMgr->ProcessWith( ( sChild + "_c.arg" ).c_str(), szFilename );
	m_pMgr->ProcessWith( ( sChild + "_c.ars" ).c_str(), szFilename );

	m_pMgr->PostProcessed(szFilename);

	return true;
}
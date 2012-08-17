#include "StdAfx.h"
#include "CArsProcessor.h"
#include "GraphicCommon.h"

bool CArsProcessor::RunProcess( const char* szFilename )
{
	FILE* fp;
	ChangeToSrcDir();
	fp = SureOpenFile( szFilename, "rb" );

	fseek(fp,0,SEEK_SET);

	int nVersion;
	fread(&nVersion, sizeof(int),1,fp);

	if ( nVersion > CLIENT_RES_VERSION )
	{
		ostringstream strm;
		strm<< szFilename << "文件版本错误\n";
		GenErr(strm.str());
	}

	//读取每张地图中所有纹理名
	std::vector<std::string> vecNames;
	int nSurfaceNum;
	fread(&nSurfaceNum, sizeof(int),1,fp);
	for( int i = 0; i < nSurfaceNum; i++ )
	{
		int nTextLen;
		char Buf[1024];
		memset( Buf, 0, 1024 );
		fread( &nTextLen, sizeof(int),1,fp);
		fread( Buf, nTextLen,1,fp);

		vecNames.push_back(Buf);
	}

	//////////////////////////////////////////////////////////////////////////

	//读取每张地图中所有的模型，动作及特效名
	int nSetUnitNum;
	fread( &nSetUnitNum, sizeof(int),1,fp);
	for( int i = 0; i < nSetUnitNum; i++ )
	{
		TBYTE ucFlag;
		TBYTE ucUnitPartNum;
		fread( &ucFlag, sizeof(TBYTE), 1, fp);
		fread( &ucUnitPartNum, sizeof(TBYTE),1,fp);

		for ( size_t j = 0; j < ucUnitPartNum; j++ )
		{
			char szBuf[1024];
			memset( szBuf, 0, 1024 );
			unsigned int uResType;
			int nLen;
			fread( &uResType, sizeof(int),1,fp);
			fread( &nLen, sizeof(int),1,fp);
			fread( szBuf, nLen, 1, fp);

			vecNames.push_back(szBuf);

			switch( uResType )
			{
			case 'ERA':
				fread( &nLen, sizeof(int), 1, fp);
				memset(szBuf, 0, 1024 );
				fread(szBuf, nLen, 1, fp);
				break;
			default:
				break;
			}
		}
	}

	for ( size_t i = 0; i < vecNames.size(); ++i )
	{
		m_pMgr->ProcessWith(vecNames[i].c_str(),szFilename);
	}

	fclose(fp);

	m_pMgr->PostProcessed(szFilename);

	return true;
}


#include "stdafx.h"
#include "CAreProcessor.h"
#include "Algorithm.h"
#include "../../src/Effect/CEffectClassDesc.h"
#include "../../include/IEffectMgrModule.h"
#include "BaseHelper.h"
#include "../../src/Effect/CEffectUnit.h"

CPakEffect::~CPakEffect(void)
{
	size_t n = m_Tracks.size();
	for( size_t Num = 0; Num < n; Num++ )
	{
		for( map< int, EFFECTKEY >::iterator it = m_Tracks[Num].begin(); it != m_Tracks[Num].end(); it++ )
		{
			SafeRelease( it->second.pEffectUnit );
		}
	}
}

int CPakEffect::LoadBin( CEffectReadFile& File, DWORD dwVer, string szFxName )
{
	int TrackSize;
	File.read( (char*)&TrackSize, sizeof(int) );
	for( int i = 0; i < TrackSize; i++ )
	{
		map< int, EFFECTKEY > CurTrack;
		int KeySize;
		File.read( (char*)&KeySize, sizeof(int) );
		for( int i = 0; i < KeySize; i++ )
		{
			int Len;
			GUID ClassID;
			UINT ID;
			int Pos;
			EFFECTKEY Key;
			char Desc[1024];
			File.read( (char*)&ClassID, sizeof(GUID) );
			File.read( (char*)&Len, sizeof(int) );
			File.read( (char*)Desc, Len );
			File.read( (char*)&ID, sizeof(UINT) );
			File.read( (char*)&Pos, sizeof(int) );

			IEffectUnitProp* pEffectUnit = m_pEffectMgr->CreateEffectUnitProp( ClassID, ID );
			if( pEffectUnit == NULL )
			{
				Desc[Len] = 0;
				string str = "无法找到相应的DLL，DLL描述：";
				str += Desc;
				return GERROR;
			}

			//加入特效跟随
			if( dwVer >= 10005 )
			{
				File.read( (char*)&Key.nParentTrack, sizeof(int) );
				File.read( (char*)&Key.nParentStartTime, sizeof(int) );
			}

			pEffectUnit->LoadBin( File );
			Key.pEffectUnit = pEffectUnit;
			CurTrack[Pos] = Key;

			/*ADVANCE_PROP			Offset[256];
			ADVANCE_PROP			Texture[256];
			ADVANCE_PROP			Color[256];
			ADVANCE_PROP			Scale[256];
			ADVANCE_PROP			XYZcale[256];
			ADVANCE_PROP			UVRotateCenter[256];
			ADVANCE_PROP			EmissionMinRadius[256];
			ADVANCE_PROP			EmissionMaxRadius[256];
			int OffsetLen = 256, TextLen = 256, ColorLen = 256, ScaleLen = 256, XYZcaleLen = 256, UVRotateCenterLen = 256, EmissionMinRadiusLen = 256, EmissionMaxRadiusLen = 256;
			EFFECT_PROP Prop[256];
			int nLen = pEffectUnit->GetProp( Prop, 256 );
			pEffectUnit->SetProp( Prop, nLen );
			for( int i = 0; i < nLen; i++ )
			{
				if( Prop[i].Get().GetType() == EFFECT_DATA::STRING )
				{
					if( Prop[i].Get().IsFile() )
						m_pEffectMgr->GetProcessor()->GetMgr()->ProcessWith( Prop[i].Get().Str(), ( File.m_strFileName + "( " + szFxName + " )" ).c_str() );
				}
			}
			pEffectUnit->GetAdvancedProp( Offset, OffsetLen, Texture, TextLen, Color, ColorLen, Scale, ScaleLen, XYZcale, XYZcaleLen, UVRotateCenter, UVRotateCenterLen,
				EmissionMinRadius, EmissionMinRadiusLen, EmissionMaxRadius, EmissionMaxRadiusLen);
			for( int i = 0; i < TextLen; i++ )
			{
				if( !_stricmp( Texture[i].Prop.CStr(), "none" ) )
					continue;
				m_pEffectMgr->GetProcessor()->GetMgr()->ProcessWith( Texture[i].Prop.CStr(), ( File.m_strFileName + "( " + szFxName + " )" ).c_str() );
			}
			pEffectUnit->SetAdvancedProp( Offset, OffsetLen, Texture, TextLen, Color, ColorLen, Scale, ScaleLen, XYZcale, XYZcaleLen, UVRotateCenter, UVRotateCenterLen,
				EmissionMinRadius, EmissionMinRadiusLen, EmissionMaxRadius, EmissionMaxRadiusLen);*/
			EMap< float, EString >& TextName =  ((CEffectUnitProp*)pEffectUnit)->m_TextName;
			for(EMap< float, EString >::iterator iter = TextName.begin();iter!=TextName.end();++iter)
			{
				if( !_stricmp( iter->second.c_str(), "none" ) )
					continue;
				m_pEffectMgr->GetProcessor()->GetMgr()->ProcessWith(iter->second.c_str(), ( File.m_strFileName + "( " + szFxName + " )" ).c_str() );
			}
		}
		m_Tracks.push_back( CurTrack );
	}

	if( dwVer >= 10003 )
		File.read( (char*)&m_TimeRange, sizeof(int) );
	m_AttachAniName.clear();
	if ( dwVer >= 10004 )
	{
		int Num;
		File.read( (char*)&Num, sizeof(int) );
		for( int i = 0; i < Num; i++ )
		{
			int Len;
			char Buf[256];
			memset( Buf, 0, 256 );
			File.read( (char*)&Len, sizeof(int) );
			File.read( Buf, Len );
			m_AttachAniName.insert( Buf );
		}
	}

	m_UserDesc.clear();
	if( dwVer >= 10006 )
		LoadStringT( File, m_UserDesc );

	//读取特效帧同步信息
	if( dwVer >= 10007 )
		File.read((TCHAR*)&m_bFrameSynch,sizeof(bool));

	if( dwVer >= 10008 )
		File.read((TCHAR*)&m_bForbidScale,sizeof(bool));

	return 0;
}

// ----------------------------------------------------------------------------

CAreManager::CAreManager(CAreProcessor* pProcessor): m_pProcessor( pProcessor )
{
}

CAreManager::~CAreManager()
{
	for( map< string, map< string, CPakEffect* > >::iterator itGroup = m_pEffects.begin(); 
		itGroup != m_pEffects.end(); itGroup++ )
	{
		for( map< string, CPakEffect* >::iterator itEffect = itGroup->second.begin();
			itEffect != itGroup->second.end(); itEffect++ )
			SafeDelete( itEffect->second );
	}
	m_pEffects.clear();
}

IEffectUnitProp* CAreManager::CreateEffectUnitProp( const GUID Guid, int Num )
{
	return m_pProcessor->GetClassDesc()->CreateEffectUnitProp( Num );
}

int CAreManager::LoadEffectGroups( string LoadName )
{
	CEffectReadFile File( LoadName.data(), true );
	if( File.fail() )
	{
		ostringstream strm;
		strm<<"无法打开文件："<<LoadName<<endl;
		GenErr(strm.str());

		return GERROR;
	}

	DWORD Res;
	DWORD Version;
	File.read( (char*)&Res, sizeof(DWORD) );
	File.read( (char*)&Version, sizeof(DWORD) );

	if( Res != MAKEFOURCC( 'E', 'F', 'X', 0 ) || Version < ARE_VERSION - 3 || Version > ARE_VERSION )
	{
		ostringstream strm;
		strm<<"版本不正确，当前程序版本："<<ARE_VERSION/10000.0f<<"\t"<<LoadName<<"文件版本："<<Version/10000.0f<<endl;
		GenErr(strm.str());

		return GERROR;
	}

	vector<char> Temp;
	int Size;
	int GroupSize;
	File.read( (char*)&GroupSize, sizeof(int) );
	for( int i = 0; i < GroupSize; i++ )
	{
		File.read( (char*)&Size, sizeof(int) );
		Temp.resize( Size + 1 );
		memset( &Temp[0], 0, Size + 1 );
		File.read( &Temp[0], Size );
		string str = &Temp[0];
		if( m_pEffects.find( str ) != m_pEffects.end() )
			return GERROR;

		m_pEffects[str] = map< string, CPakEffect* >();
		map< string, map< string, CPakEffect* > >::iterator itGroup = m_pEffects.find( str );

		int EffectSize;
		File.read( (char*)&EffectSize, sizeof(int) );
		for( int j = 0; j < EffectSize; j++ )
		{
			string str;
			LoadStringT( File, str );
			if( itGroup->second.find( str ) != itGroup->second.end() )
				return GERROR;

			itGroup->second[str] = new CPakEffect(this);
			map< string, CPakEffect* >::iterator itEffect = itGroup->second.find( str );

			if( FAILED( itEffect->second->LoadBin( File, Version, itGroup->first + "\\" + str ) ) )
				return GERROR;
		}
	}
	return 0;
}

// ----------------------------------------------------------------------------

CAreProcessor::CAreProcessor(void)
{
	m_pEffectDesc = NULL;
	m_pEffectDesc = CreateEffectClassDesc();

	GetIEffectMgrModule()->CreateEffectMgr();

	if (!m_pEffectDesc)
	{
		GenErr("CreateEffectClassDesc is fail");
	}
}

CAreProcessor::~CAreProcessor(void)
{
	SafeRelease( m_pEffectDesc );
}

bool CAreProcessor::RunProcess( const char* szFilename )
{
	CAreManager Mgr( this );
	ChangeToSrcDir();
	if( FAILED( Mgr.LoadEffectGroups( szFilename ) ) )
	{
		MessageBox( 0, szFilename, "加载失败", 0);
		return false;
	}
	m_pMgr->PostProcessed(szFilename);
	
	return true;
}

#include "stdafx.h"
#include "CEffectGroup.h"
#include "CEffectManager.h"
#include "Algorithm.h"
#include "SyncSystem.h"
#include "ErrLogHelper.h"
#include "TSqrAllocator.inl"
#include "CEffectUnit.h"

int CEffectProp::LoadBin( CEffectFile& File, uint32 dwVer )
{
	int TrackSize = 0;
	File.read( (char*)&TrackSize, sizeof(int) );
	for( int i = 0; i < TrackSize; ++i )//每个特效里面有多少个轨道
	{
		EMap< int, IEffectUnitProp* > CurTrack;
		int KeySize = 0;
		File.read( (char*)&KeySize, sizeof(int) );
		for( int i = 0; i < KeySize; ++i )//每个轨道里面有多少个特效单元
		{
			int Len = 0, Pos = 0;
			GUID ClassID;
			UINT ID;
			char Desc[1024];

			File.read( (char*)&ClassID, sizeof(GUID) );
			File.read( (char*)&Len, sizeof(int) );
			File.read( (char*)Desc, Len );
			File.read( (char*)&ID, sizeof(UINT) );
			File.read( (char*)&Pos, sizeof(int) );
			Pos = max(0,Pos);
			IEffectUnitProp* pEffectUnitProp = ( (CEffectManager*)m_pEffectMgr )->CreateEffectUnitProp( ClassID, ID );
			if( pEffectUnitProp == NULL )
			{
				Desc[Len] = 0;
				EString str = "无法找到相应的DLL，DLL描述：";
				str += Desc;
				printf( ( str + "错误提示" ).c_str() );
				return GERROR;
			}
			static_cast<CEffectUnitProp*>(pEffectUnitProp)->m_pEffectProp = this;
			if( dwVer >= 10005 )
			{
				File.read( (char*)&m_AttachKey[pEffectUnitProp].nTrack, sizeof(int) );
				File.read( (char*)&m_AttachKey[pEffectUnitProp].nKeyStartTime, sizeof(int) );
			}
			
			pEffectUnitProp->LoadBin( File );
			CurTrack[Pos] = pEffectUnitProp;
			
			CSyncSystem::TryToStop();
		}
		m_Tracks.push_back( CurTrack );
	}

	if( dwVer >= 10003 )
		File.read( (char*)&m_TimeRange, sizeof(int) );
	
	m_AttachAniName.clear();
	
	if ( dwVer >= 10004 )
	{
		int Num = 0;
		File.read( (char*)&Num, sizeof(int) );
		for( int i = 0; i < Num; ++i )
		{
			int Len;
			File.read( (char*)&Len, sizeof(int) );
			string Buf(Len, 0);
			File.read( &Buf[0], Buf.size() );
			m_AttachAniName.push_back( Buf.c_str() );
		}
	}

	if( dwVer >= 10006 )
		LoadStringT( File, m_UserDesc );
	//读取特效帧同步信息
	if( dwVer >= 10007 )
		File.read((TCHAR*)&m_bFrameSynch,sizeof(bool));

	//读取特效是否禁止缩放
	if( dwVer >= 10008 )
		File.read((TCHAR*)&m_bForbidScale,sizeof(bool));

	return 0;
}

int CEffectProp::LoadFullRes()
{
	if( m_bLoadRes )
		return 0;
	for( size_t i = 0; i < m_Tracks.size(); ++i )
	{
		EMap< int, IEffectUnitProp* >& Track = m_Tracks[i];
		for( EMap< int, IEffectUnitProp* >::iterator it = Track.begin(); it != Track.end(); ++it )
			it->second->LoadFullRes();
	}
	m_bLoadRes = true;

	return 0;
}

void	CEffectProp::_FreeRes(void)
{
	EVector< EMap< int, IEffectUnitProp* > >::iterator vit,vite = m_Tracks.end();
	for( vit = m_Tracks.begin(); vit!=vite; ++vit )
	{
		EMap< int, IEffectUnitProp* >::iterator it,eit = (*vit).end();
		for( it = (*vit).begin(); it!=eit; ++it )
			it->second->_FreeRes();
	}
}

void	CEffectProp::_Reclaim(void)
{
	EVector< EMap< int, IEffectUnitProp* > >::iterator vit,vite = m_Tracks.end();
	for( vit = m_Tracks.begin(); vit!=vite; ++vit )
	{
		EMap< int, IEffectUnitProp* >::iterator it,eit = (*vit).end();
		for( it = (*vit).begin(); it!=eit; ++it )
			it->second->_Reclaim();
	}
}

const char* CEffectProp::GetEffectFileName()
{
	if(m_pEffectGroup)
		return m_pEffectGroup->GetName();
	return NULL;
}
//////////////////////////////////////////////////////////////////////////

CEffectGroup::CEffectGroup( IEffectManager* pEffectMgr, const char* szFileName ) 
: m_pEffectMgr( pEffectMgr ), m_szName( szFileName ), m_bValid(false)
{
	m_isOnRead = false;
}

CEffectGroup::~CEffectGroup(void)
{
	if(	m_isOnRead )
	{
		CSyncSystem::SafeDelCoder((SyncCoder*)0x1); //等Coder出现就好了
		//GfkLogErr("有读取终止","CEffectGroup");
	}

	for( EMap< EString, CEffectProp* >::iterator it = m_EffectProps.begin(); it != m_EffectProps.end(); ++it )
		GfkSafeDelete(it->second);

	m_EffectProps.clear();
}

int CEffectGroup::BinLoad( CEffectFile& File )
{
	uint32 Res = 0, Version = 0;
	File.read( (char*)&Res, sizeof(uint32) );
	File.read( (char*)&Version, sizeof(uint32) );

	if( Res != MAKEFOURCC( 'E', 'F', 'X', 0 ) || Version < ARE_VERSION - 3 || Version > ARE_VERSION )
	{
		char Buf[256];
		sprintf( Buf, "版本不匹配: 当前程序版本：%f\n%s文件版本：%f", ARE_VERSION/10000.0f, m_szName.c_str(), Version/10000.0f );
		printf( Buf );
		return GERROR;
	}
	SetRefInfo(m_szName.c_str());
	m_isOnRead = true;
	SQR_TRY
	{
		int Size = 0 , GroupSize = 0;
		File.read( (char*)&GroupSize, sizeof(int) );
		for( int i = 0; i < GroupSize; ++i )//每个efx文件有多少组特效
		{
			EString strGroup = "";

			File.read(&Size, sizeof(int));
			EVector<char> temp(Size + 1, 0);
			File.read(&temp.front(), Size);
			strGroup = &temp.front();

			int EffectSize = 0;
			File.read( (char*)&EffectSize, sizeof(int) );
			for( int j = 0; j < EffectSize; ++j )//每组有多少个特效
			{
				EString strEffect = "";
				File.read(&Size, sizeof(int));
				EVector<char> temp(Size + 1, 0);
				File.read(&temp.front(), Size);
				strEffect = &temp.front();

				strEffect = strGroup + "\\" + strEffect;

				EMap< EString, CEffectProp* >::iterator iter = m_EffectProps.find(strEffect);
				if( iter == m_EffectProps.end() )
					m_EffectProps[strEffect] = CreateEffectPropFun(strEffect.c_str());
				else
				{
					EString Err = "特效文件:" + m_szName + "存在同名轨道";
					GfkLogErr(Err.c_str());
				}

				if( FAILED( m_EffectProps[strEffect]->LoadBin( File, Version ) ) )
					return GERROR;
				CSyncSystem::TryToStop();

				m_EffectProps[strEffect]->LoadFullRes();
				CSyncSystem::TryToStop();
			}
		}

		m_bValid = true;
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("特效创建失败");
		exp.AppendType(m_szName.c_str());
		GfkLogExp(exp);
	}
	SQR_TRY_END;


	m_isOnRead = false;
	return 0;
}

CEffectProp* CEffectGroup::CreateEffectPropFun(const char* szEffect)
{
	return new CEffectProp( this, m_pEffectMgr, szEffect );
}

CEffectProp* CEffectGroup::GetEffectProp( const char* szEffectName )
{
	EMap< EString, CEffectProp* >::iterator it = m_EffectProps.find( szEffectName );
	if( it == m_EffectProps.end() )
		return NULL;
	return it->second;
}

UINT CEffectGroup::GetEffectNum()
{
	return (UINT)m_EffectProps.size();
}

const char* CEffectGroup::GetEffectName( UINT Num )
{
	EMap< EString, CEffectProp* >::iterator it = m_EffectProps.begin();
	for( UINT i = 0; it != m_EffectProps.end(); ++it,++i )
	{
		if( i == Num )
			return it->first.c_str();
	}
	return NULL;
}

void CEffectGroup::DeleteThis()
{
	CEffectManager* pMgr = (CEffectManager*)m_pEffectMgr;
	pMgr->DeleteEffectGroup( this );
};


void CEffectGroup::_FreeRes(void)
{
	EMap< EString, CEffectProp* >::iterator it,eit = m_EffectProps.end();
	for(it = m_EffectProps.begin(); it!=eit; ++it)
		it->second->_FreeRes();
}

void CEffectGroup::_Reclaim(void)
{
	EMap< EString, CEffectProp* >::iterator it,eit = m_EffectProps.end();
	for(it = m_EffectProps.begin(); it!=eit; ++it)
		it->second->_Reclaim();
}

void	CBaseEffectGroup::Release()
{
	if(GetRefCount() == 0 && m_EffectRef>0)
	{
		GfkLogErr("特效释放错误:子特效未释放",GetName());
		return;
	}
	IEffectGroup::Release();
}

void	CBaseEffectGroup::Free()
{
	if(m_EffectRef == 0)
		DeleteThis(); 
}

const char* CBaseEffectGroup::GetName() const
{
	return "";
}
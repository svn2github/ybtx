#include "stdafx.h"
#include "CEffectCoder.h"
#include "CEffectGroup.h"
#include "CEffectUnit.h"
#include "CCameraFilter.h"
#include "CCameraShake.h"
#include "CEffectManager.h"
#include "CDynamicLight.h"
#include "CFlareSprite.h"
#include "CLeading.h"
#include "CLightning.h"
#include "CModelConsole.h"
#include "CModelRenderStyle.h"
#include "CNull.h"
#include "CParticleSystem.h"
#include "CPolyTrail.h"
#include "CShake.h"
#include "CSprite.h"
#include "CTDimAudio.h"
//----------------------------------------------------------


CEffectFormatMgr::CEffectFormatMgr()
:CFileFormatMgr('EGCK',1,NULL)
{
	RegisterCoder(new CEffectPropCoder);
}

BEGIN_VERSION_MAP(CEffectPropCoder)
	ON_VERSION( 1 , DeCode_EffectProp )
END_VERSION_MAP()

CEffectPropCoder::CEffectPropCoder()
{
	RegisterCoder(new CCameraFilterPropCoder);
	RegisterCoder(new CDynamicLightPropCoder);
	RegisterCoder(new CFlareSpritePropCoder);
	RegisterCoder(new CLeadingPropCoder);
	RegisterCoder(new CLightningPropCoder);
	RegisterCoder(new CModelConsolePropCoder);
	RegisterCoder(new CModelRenderStylePropCoder);
	RegisterCoder(new CNullPropCoder);
	RegisterCoder(new CParticleSystemPropCoder);
	RegisterCoder(new CPolyTrailPropCoder);
	RegisterCoder(new CShakePropCoder);
	RegisterCoder(new CSpritePropCoder);
	RegisterCoder(new CTDimAudioPropCoder);
}

bool CEffectPropCoder::Code(WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CEffectGroup* pGroup = reinterpret_cast<CEffectGroup*>(handle);

	for(EMap< EString, CEffectProp* >::iterator iter = pGroup->m_EffectProps.begin();iter != pGroup->m_EffectProps.end();++iter)
	{
		int Size = iter->first.length();
		wdi.Write(&Size,sizeof(int));
		const EString& strRef = iter->first;
		wdi.Write(strRef.c_str(),Size);
		int TrackSize = iter->second->m_Tracks.size();
		wdi.Write(&TrackSize,sizeof(int));
		for (int i=0;i<TrackSize;++i)
		{
			int KeySize = iter->second->m_Tracks[i].size();
			wdi.Write(&KeySize,sizeof(int));
			for (EMap< int, IEffectUnitProp* >::iterator iterSecnd = iter->second->m_Tracks[i].begin();iterSecnd!=iter->second->m_Tracks[i].end();++iterSecnd)
			{
				int Pos = iterSecnd->first;
				wdi.Write(&Pos,sizeof(int));
				string Desc = iterSecnd->second->GetClassDescription();
				int Len = Desc.size();
				const GUID ClassID = iterSecnd->second->GetGUID();
				UINT ID = iterSecnd->second->GetID();
				Pos = max(Pos,0);
				wdi.Write( (TCHAR*)&ClassID, sizeof(GUID) );
				wdi.Write( (TCHAR*)&Len, sizeof(int) );
				wdi.Write( (TCHAR*)Desc.c_str(), Len );
				wdi.Write( (TCHAR*)&ID, sizeof(UINT) );
				WriteEffectUnitProp(wdi,iterSecnd->second);
				CEffectProp* pEffectProp = iter->second;
				wdi.Write(&pEffectProp->m_AttachKey[iterSecnd->second].nTrack, sizeof(int) );
				wdi.Write(&pEffectProp->m_AttachKey[iterSecnd->second].nKeyStartTime, sizeof(int) );
			}
		}
		int timeRange = iter->second->GetTimeRange();
		wdi.Write(&timeRange, sizeof(int) );

		int Num = iter->second->GetAttachAniNum();
		wdi.Write(&Num, sizeof(int) );
		for (int i=0;i<Num;++i)
		{
			string strAniName = iter->second->GetAttachAniName(i);
			int Len = strAniName.size();
			wdi.Write(&Len, sizeof(int) );
			wdi.Write(strAniName.c_str(), Len );
		}

		SaveStringWDIT( wdi, iter->second->GetUserDesc() );
		//保存特效帧同步信息
		bool bFrameSynch = iter->second->GetFrameSynch();
		wdi.Write(&bFrameSynch,sizeof(bool));
	}
	return true;
}

#define EFFECTSELECTCASE(enumType,classType,wdi,handle); \
	case enumType: \
	{\
		 classType propCoder; \
		propCoder.Encode(wdi,handle); \
	}\
	break;

void	CEffectPropCoder::WriteEffectUnitProp(WriteDataInf& wdi , IEffectUnitProp* handle)
{
	switch(handle->GetID())
	{
		EFFECTSELECTCASE(eParticleSystem,CParticleSystemPropCoder,wdi,handle);
		EFFECTSELECTCASE(eSprite,CSpritePropCoder,wdi,handle);
		EFFECTSELECTCASE(ePolyTrail,CPolyTrailPropCoder,wdi,handle);
		EFFECTSELECTCASE(eCameraShake,CCameraShakePropCoder,wdi,handle);
		EFFECTSELECTCASE(eFlareSprite,CFlareSpritePropCoder,wdi,handle);
		EFFECTSELECTCASE(eCameraFilter,CCameraFilterPropCoder,wdi,handle);
		EFFECTSELECTCASE(eModelConsole,CModelConsolePropCoder,wdi,handle);
		EFFECTSELECTCASE(eDynamicLight,CDynamicLightPropCoder,wdi,handle);
		EFFECTSELECTCASE(eNull,CNullPropCoder,wdi,handle);
		EFFECTSELECTCASE(eTDimAudio,CTDimAudioPropCoder,wdi,handle);
		EFFECTSELECTCASE(eModelRenderStyle,CModelRenderStylePropCoder,wdi,handle);
		EFFECTSELECTCASE(eLightning,CLightningPropCoder,wdi,handle);
		EFFECTSELECTCASE(eLeading,CLeadingPropCoder,wdi,handle);
		EFFECTSELECTCASE(eShake,CShakePropCoder,wdi,handle);
	}
}

bool CEffectPropCoder::DeCode_EffectProp( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	CEffectGroup* pGroup = reinterpret_cast<CEffectGroup*>(handle);
	string strEffect;
	int Size;
	tData.SafeRead(&Size, sizeof(int));
	EVector<char> temp(Size + 1, 0);
	tData.SafeRead(&temp.front(), Size);
	strEffect = &temp.front();
	EMap< EString, CEffectProp* >::iterator iter = pGroup->m_EffectProps.find(strEffect.c_str());
	if( iter == pGroup->m_EffectProps.end() )
		pGroup->m_EffectProps[strEffect.c_str()] = pGroup->CreateEffectPropFun(strEffect.c_str());
	else
	{
		EString Err = "特效文件:" + pGroup->m_szName + "存在同名轨道";
		GfkLogErr(Err.c_str());
	}
	CEffectProp* pEffectProp = pGroup->m_EffectProps[strEffect.c_str()];
	int TrackSize = 0;
	tData.SafeRead( (char*)&TrackSize, sizeof(int) );
	for( int i = 0; i < TrackSize; ++i )//每个特效里面有多少个轨道
	{
		EMap< int, IEffectUnitProp* > CurTrack;
		int KeySize = 0;
		tData.SafeRead( (char*)&KeySize, sizeof(int) );
		for( int j = 0; j < KeySize; ++j )//每个轨道里面有多少个特效单元
		{
			int Pos = 0;
			tData.SafeRead( (char*)&Pos, sizeof(int) );
			int Len = 0;
			GUID ClassID;
			UINT ID;
			char Desc[1024];
			tData.SafeRead( (char*)&ClassID, sizeof(GUID) );
			tData.SafeRead( (char*)&Len, sizeof(int) );
			tData.SafeRead( (char*)Desc, Len );
			tData.SafeRead( (char*)&ID, sizeof(UINT) );
			Pos = max(0,Pos);
			IEffectUnitProp* pEffectUnitProp = ( (CEffectManager*)(pEffectProp->m_pEffectMgr) )->CreateEffectUnitProp( ClassID, ID );
			if( pEffectUnitProp == NULL )
			{
				Desc[Len] = 0;
				EString str = "无法找到相应的DLL，DLL描述：";
				str += Desc;
				printf( ( str + "错误提示" ).c_str() );
				return false;
			}
			static_cast<CEffectUnitProp*>(pEffectUnitProp)->m_pEffectProp = pEffectProp;
			CDataChunk tChunk;
			tData.SafeRead(&tChunk.GetHdr(),sizeof(CDataChunk::ChunkHdr));
			ReadChunk(tData,tChunk);
			_DoDecode(tChunk,pEffectUnitProp);
			CurTrack[Pos] = pEffectUnitProp;
			tData.SafeRead( (char*)&pEffectProp->m_AttachKey[pEffectUnitProp].nTrack, sizeof(int) );
			tData.SafeRead( (char*)&pEffectProp->m_AttachKey[pEffectUnitProp].nKeyStartTime, sizeof(int) );
		}
		pEffectProp->m_Tracks.push_back(CurTrack);
	}

	tData.SafeRead(&pEffectProp->m_TimeRange, sizeof(int));
	pEffectProp->m_AttachAniName.clear();
	int Num = 0;
	tData.SafeRead(&Num, sizeof(int) );
	for( int i = 0; i < Num; ++i )
	{
		EString strTemp;
		LoadStringSafeT(tData,strTemp);
		pEffectProp->m_AttachAniName.push_back( strTemp );
	}
	LoadStringSafeT( tData, pEffectProp->m_UserDesc );
	tData.SafeRead(&pEffectProp->m_bFrameSynch,sizeof(bool));
	pEffectProp->LoadFullRes();
	return true;
}


BEGIN_VERSION_MAP(CEffectUnitPropCoder)
	ON_VERSION( 1 , DeCode_EffectUnitProp )
END_VERSION_MAP()


bool CEffectUnitPropCoder::Code(WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CEffectUnitProp* pEffectUnitProp = reinterpret_cast<CEffectUnitProp*>(handle);
	//wdi.BeginWrite(GetChunkName(),GetNewVer());
	SaveStringWDIT( wdi, pEffectUnitProp->m_szAttachName.c_str() );
	SaveStringWDIT( wdi, pEffectUnitProp->m_CustomName.c_str() );
	SaveStringWDIT( wdi, pEffectUnitProp->m_EffectClassName.c_str() );
	int Size = pEffectUnitProp->m_szAniName.size();
	wdi.Write( (char*)&Size,sizeof(int32) );
	for( ESet<EString>::iterator it = pEffectUnitProp->m_szAniName.begin(); it != pEffectUnitProp->m_szAniName.end(); it++ )
		SaveStringWDIT( wdi, (*it).c_str() );
	wdi.Write(&pEffectUnitProp->m_bSlownDisappear,	sizeof(bool)  );
	wdi.Write(&pEffectUnitProp->m_isUseVel,				sizeof(bool)  );
	wdi.Write(&pEffectUnitProp->m_EffectLod,			sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_TimeRange,			sizeof(int32) );
	wdi.Write(&pEffectUnitProp->m_eUpdatePos,			sizeof(eUpdatePosType) );
	wdi.Write(&pEffectUnitProp->m_nTextureCircle,		sizeof(int32) );
	SaveMapValueWDI( wdi, pEffectUnitProp->m_vecOffset );
	SaveMapValueWDI( wdi, pEffectUnitProp->m_Color );
	SaveMapValueWDI( wdi, pEffectUnitProp->m_Scale );
	Size = (int32)pEffectUnitProp->m_TextName.size();
	wdi.Write( (char*)&Size, sizeof( int32 ) );
	for( EMap< float, EString >::const_iterator it = pEffectUnitProp->m_TextName.begin(); it != pEffectUnitProp->m_TextName.end(); it++ )
	{
		wdi.Write( (char*)&it->first, sizeof( float ) );
		SaveStringWDIT( wdi, it->second.c_str() );
	}
	return true;
}

bool CEffectUnitPropCoder::DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	CEffectUnitProp* pEffectUnitProp = reinterpret_cast<CEffectUnitProp*>(handle);
	LoadStringSafeT( tData, pEffectUnitProp->m_szAttachName );
	LoadStringSafeT( tData, pEffectUnitProp->m_CustomName );
	LoadStringSafeT( tData, pEffectUnitProp->m_EffectClassName );
	int32 Size = 0;
	pEffectUnitProp->m_szAniName.clear();
	tData.SafeRead(&Size,sizeof(int32));
	for( int32 i = 0; i < Size; ++i )
	{
		EString szStr = "";
		LoadStringSafeT( tData, szStr );
		pEffectUnitProp->m_szAniName.insert( szStr );
	}
	tData.SafeRead(&pEffectUnitProp->m_bSlownDisappear,sizeof(bool));
	tData.SafeRead(&pEffectUnitProp->m_isUseVel,sizeof(bool));
	tData.SafeRead(&pEffectUnitProp->m_EffectLod,sizeof(float));
	tData.SafeRead(&pEffectUnitProp->m_TimeRange,sizeof(int32));
	tData.SafeRead(&pEffectUnitProp->m_eUpdatePos,sizeof(eUpdatePosType));
	tData.SafeRead(&pEffectUnitProp->m_nTextureCircle,	sizeof(int32));

	LoadMapValueSafe( tData, pEffectUnitProp->m_vecOffset );
	LoadMapValueSafe( tData, pEffectUnitProp->m_Color );
	LoadMapValueSafe( tData, pEffectUnitProp->m_Scale );

	tData.SafeRead(&Size, sizeof( int32 ) );
	for( int32 i = 0; i < Size; ++i )
	{
		float fPos = 0.0f;
		EString szStr = "";
		tData.SafeRead(&fPos, sizeof(float));
		LoadStringSafeT( tData, szStr );
		pEffectUnitProp->m_TextName[fPos] = szStr;
	}
	return true;
}

BEGIN_VERSION_MAP(CCameraFilterPropCoder)
	ON_VERSION( 1 , DeCode_EffectUnitProp )
END_VERSION_MAP()

CCameraFilterPropCoder::CCameraFilterPropCoder()
{
	RegisterCoder(new CEffectUnitPropCoder);
}

bool CCameraFilterPropCoder::Code(WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CCameraFilterProp* pEffectUnitProp = (CCameraFilterProp*)(handle);
	wdi.Write(&pEffectUnitProp->m_bFilterParent,sizeof(bool) );
	wdi.Write(&pEffectUnitProp->m_eBlendModel,sizeof(SPR_BLENDMODE) );
	wdi.Write(&pEffectUnitProp->m_eZTest,sizeof(Z_TEST) );
	wdi.Write(&pEffectUnitProp->m_xScale,sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_yScale,sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_zScale,sizeof(float) );
	_AutoEncode(wdi,handle);
	return true;
}

bool CCameraFilterPropCoder::DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	CCameraFilterProp* pEffectUnitProp = (CCameraFilterProp*)(handle);
	tData.SafeRead(&pEffectUnitProp->m_bFilterParent,sizeof(bool));
	tData.SafeRead(&pEffectUnitProp->m_eBlendModel,sizeof(SPR_BLENDMODE));
	tData.SafeRead(&pEffectUnitProp->m_eZTest,sizeof(Z_TEST));
	tData.SafeRead(&pEffectUnitProp->m_xScale,sizeof(float));
	tData.SafeRead(&pEffectUnitProp->m_yScale,sizeof(float));
	tData.SafeRead(&pEffectUnitProp->m_zScale,sizeof(float));
	_DoDecode(inData,pEffectUnitProp);
	return true;
}

BEGIN_VERSION_MAP(CCameraShakePropCoder)
	ON_VERSION( 1 , DeCode_EffectUnitProp )
END_VERSION_MAP()

CCameraShakePropCoder::CCameraShakePropCoder()
{
	RegisterCoder(new CEffectUnitPropCoder);
}

bool CCameraShakePropCoder::Code(WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CCameraShakeProp* pEffectUnitProp = (CCameraShakeProp*)(handle);
	
	wdi.Write(&pEffectUnitProp->m_fFrequence,			sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fStrength,			sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fMinRadius,			sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fMaxRadius,			sizeof(float) );
	_AutoEncode(wdi,handle);
	return true;
}

bool CCameraShakePropCoder::DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	CCameraShakeProp* pEffectUnitProp = (CCameraShakeProp*)(handle);

	tData.SafeRead(&pEffectUnitProp->m_fFrequence,			sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fStrength,				sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fMinRadius,			sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fMaxRadius,			sizeof(float) );
	CDataChunk tChunk;
	tData.SafeRead(&tChunk.GetHdr(),sizeof(CDataChunk::ChunkHdr));
	ReadChunk(tData,tChunk);
	_DoDecode(tChunk,pEffectUnitProp);
	return true;
}

BEGIN_VERSION_MAP(CDynamicLightPropCoder)
	ON_VERSION( 1 , DeCode_EffectUnitProp )
END_VERSION_MAP()

CDynamicLightPropCoder::CDynamicLightPropCoder()
{
	RegisterCoder(new CEffectUnitPropCoder);
}

bool CDynamicLightPropCoder::Code(WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CDynamicLightProp* pEffectUnitProp = (CDynamicLightProp*)(handle);
	wdi.Write(&pEffectUnitProp->m_fRange,				sizeof(float));
	wdi.Write(&pEffectUnitProp->m_fMinStrong,			sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fMaxStrong,			sizeof(float) );
	_AutoEncode(wdi,handle);
	return true;
}

bool CDynamicLightPropCoder::DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CDynamicLightProp* pEffectUnitProp = (CDynamicLightProp*)(handle);
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	tData.SafeRead(&pEffectUnitProp->m_fRange,				sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fMinStrong,			sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fMaxStrong,			sizeof(float) );
	CDataChunk tChunk;
	tData.SafeRead(&tChunk.GetHdr(),sizeof(CDataChunk::ChunkHdr));
	ReadChunk(tData,tChunk);
	_DoDecode(tChunk,pEffectUnitProp);
	return true;
}

BEGIN_VERSION_MAP(CFlareSpritePropCoder)
	ON_VERSION( 1 , DeCode_EffectUnitProp )
END_VERSION_MAP()

CFlareSpritePropCoder::CFlareSpritePropCoder()
{
	RegisterCoder(new CEffectUnitPropCoder);
}

bool CFlareSpritePropCoder::Code(WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CFlareSpriteProp* pEffectUnitProp = (CFlareSpriteProp*)(handle);
	wdi.Write( &pEffectUnitProp->m_vecRotate,			sizeof(CVector3f) );
	wdi.Write( &pEffectUnitProp->m_vecNormal,			sizeof(CVector3f) );
	wdi.Write( &pEffectUnitProp->m_fMinSize,				sizeof(float) );
	wdi.Write( &pEffectUnitProp->m_fMaxSize,				sizeof(float) );
	wdi.Write( &pEffectUnitProp->m_eFaceType,			sizeof(SPR_FACE) );
	wdi.Write( &pEffectUnitProp->m_eBlendModel,			sizeof(SPR_BLENDMODE) );
	wdi.Write( &pEffectUnitProp->m_eZTest,				sizeof(Z_TEST) );
	wdi.Write( &pEffectUnitProp->m_bLightEnable,			sizeof(BOOL) );
	_AutoEncode(wdi,handle);
	return true;
}

bool CFlareSpritePropCoder::DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CFlareSpriteProp* pEffectUnitProp = (CFlareSpriteProp*)(handle);
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	tData.SafeRead( &pEffectUnitProp->m_vecRotate,				sizeof(CVector3f) );
	tData.SafeRead( &pEffectUnitProp->m_vecNormal,				sizeof(CVector3f) );
	tData.SafeRead( &pEffectUnitProp->m_fMinSize,				sizeof(float) );
	tData.SafeRead( &pEffectUnitProp->m_fMaxSize,				sizeof(float) );
	tData.SafeRead( &pEffectUnitProp->m_eFaceType,				sizeof(SPR_FACE) );
	tData.SafeRead( &pEffectUnitProp->m_eBlendModel,			sizeof(SPR_BLENDMODE) );
	tData.SafeRead( &pEffectUnitProp->m_eZTest,				sizeof(Z_TEST) );
	tData.SafeRead( &pEffectUnitProp->m_bLightEnable,			sizeof(BOOL) ); 
	CDataChunk tChunk;
	tData.SafeRead(&tChunk.GetHdr(),sizeof(CDataChunk::ChunkHdr));
	ReadChunk(tData,tChunk);
	_DoDecode(tChunk,pEffectUnitProp);
	return true;
}

BEGIN_VERSION_MAP(CLeadingPropCoder)
	ON_VERSION( 1 , DeCode_EffectUnitProp )
END_VERSION_MAP()

CLeadingPropCoder::CLeadingPropCoder()
{
	RegisterCoder(new CEffectUnitPropCoder);
}

bool CLeadingPropCoder::Code(WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CLeadingProp* pEffectUnitProp = (CLeadingProp*)(handle);
	wdi.Write(&pEffectUnitProp->m_eTextOp,				sizeof(SPR_BLENDOP) );

	wdi.Write(&pEffectUnitProp->m_JumpSpeed,			sizeof(int32) );
	wdi.Write(&pEffectUnitProp->m_JumpNum,				sizeof(int32) );
	wdi.Write(&pEffectUnitProp->m_eLType,				sizeof(CLeadingProp::eLP_LKType) );
	wdi.Write(&pEffectUnitProp->m_eSSType,				sizeof(CLeadingProp::eLP_StartScr) );

	wdi.Write(&pEffectUnitProp->m_fStartAngle,			sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fRotSpeed,			sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fUVSpeed,				sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_nPieceCount,			sizeof(uint32) );
	wdi.Write(&pEffectUnitProp->m_nDouCount,			sizeof(uint32) );
	wdi.Write(&pEffectUnitProp->m_fDouPercent,			sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fDouInterval,			sizeof(float) );

	wdi.Write(&pEffectUnitProp->m_eBlendModel,			sizeof(SPR_BLENDMODE) );
	wdi.Write(&pEffectUnitProp->m_eZTest,				sizeof(Z_TEST) );
	wdi.Write(&pEffectUnitProp->m_bLightEnable,			sizeof(BOOL) );
	wdi.Write(&pEffectUnitProp->m_fZBias,				sizeof(float) );

	_AutoEncode(wdi,handle);
	return true;
}

bool CLeadingPropCoder::DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CLeadingProp* pEffectUnitProp = (CLeadingProp*)(handle);
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	tData.SafeRead( (char*)&pEffectUnitProp->m_eTextOp,				sizeof(SPR_BLENDOP) );
	tData.SafeRead( (char*)&pEffectUnitProp->m_JumpSpeed,				sizeof(int32) );
	tData.SafeRead( (char*)&pEffectUnitProp->m_JumpNum,				sizeof(int32) );
	tData.SafeRead( (char*)&pEffectUnitProp->m_eLType,				sizeof(CLeadingProp::eLP_LKType) );
	tData.SafeRead( (char*)&pEffectUnitProp->m_eSSType,				sizeof(CLeadingProp::eLP_StartScr) );

	tData.SafeRead( (char*)&pEffectUnitProp->m_fStartAngle,			sizeof(float) );
	tData.SafeRead( (char*)&pEffectUnitProp->m_fRotSpeed,				sizeof(float) );
	tData.SafeRead( (char*)&pEffectUnitProp->m_fUVSpeed,				sizeof(float) );
	tData.SafeRead( (char*)&pEffectUnitProp->m_nPieceCount,			sizeof(uint32) );
	tData.SafeRead( (char*)&pEffectUnitProp->m_nDouCount,				sizeof(uint32) );
	tData.SafeRead( (char*)&pEffectUnitProp->m_fDouPercent,			sizeof(float) );
	tData.SafeRead( (char*)&pEffectUnitProp->m_fDouInterval,			sizeof(float) );

	tData.SafeRead( (char*)&pEffectUnitProp->m_eBlendModel,			sizeof(SPR_BLENDMODE) );
	tData.SafeRead( (char*)&pEffectUnitProp->m_eZTest,				sizeof(Z_TEST) );
	tData.SafeRead( (char*)&pEffectUnitProp->m_bLightEnable,			sizeof(BOOL) );
	tData.SafeRead( (char*)&pEffectUnitProp->m_fZBias,				sizeof(float) );
	CDataChunk tChunk;
	tData.SafeRead(&tChunk.GetHdr(),sizeof(CDataChunk::ChunkHdr));
	ReadChunk(tData,tChunk);
	_DoDecode(tChunk,pEffectUnitProp);
	return true;
}

BEGIN_VERSION_MAP(CLightningPropCoder)
	ON_VERSION( 1 , DeCode_EffectUnitProp )
END_VERSION_MAP()

CLightningPropCoder::CLightningPropCoder()
{
	RegisterCoder(new CEffectUnitPropCoder);
}

bool CLightningPropCoder::Code(WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CLightningProp* pEffectUnitProp = (CLightningProp*)(handle);
	wdi.Write(&pEffectUnitProp->m_vecRotate,			sizeof(CVector3f) );
	wdi.Write(&pEffectUnitProp->m_vecNormal,			sizeof(CVector3f) );
	wdi.Write(&pEffectUnitProp->m_fStartAngle,			sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fWidthRatio,			sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fMinSize,				sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fMaxSize,				sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_vecSrc,				sizeof(CVector3f) );
	wdi.Write(&pEffectUnitProp->m_fDitherMax,			sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fDitherMin,			sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_nSubNum,				sizeof(int) );
	wdi.Write(&pEffectUnitProp->m_eFaceType,			sizeof(SPR_FACE) );
	wdi.Write(&pEffectUnitProp->m_eBlendModel,			sizeof(SPR_BLENDMODE) );
	wdi.Write(&pEffectUnitProp->m_eZTest,				sizeof(Z_TEST) );
	wdi.Write(&pEffectUnitProp->m_bLightEnable,			sizeof(BOOL) );
	wdi.Write(&pEffectUnitProp->m_fZBias,				sizeof(float) );
	_AutoEncode(wdi,handle);
	return true;
}

bool CLightningPropCoder::DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CLightningProp* pEffectUnitProp = (CLightningProp*)(handle);
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	tData.SafeRead(&pEffectUnitProp->m_vecRotate,				sizeof(CVector3f) );
	tData.SafeRead(&pEffectUnitProp->m_vecNormal,				sizeof(CVector3f) );
	tData.SafeRead(&pEffectUnitProp->m_fStartAngle,			sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fWidthRatio,			sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fMinSize,				sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fMaxSize,				sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_vecSrc,				sizeof(CVector3f) );
	tData.SafeRead(&pEffectUnitProp->m_fDitherMax,			sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fDitherMin,			sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_nSubNum,				sizeof(int) );
	tData.SafeRead(&pEffectUnitProp->m_eFaceType,				sizeof(SPR_FACE) );
	tData.SafeRead(&pEffectUnitProp->m_eBlendModel,			sizeof(SPR_BLENDMODE) );
	tData.SafeRead(&pEffectUnitProp->m_eZTest,				sizeof(Z_TEST) );
	tData.SafeRead(&pEffectUnitProp->m_bLightEnable,			sizeof(BOOL) );
	tData.SafeRead(&pEffectUnitProp->m_fZBias,				sizeof(float) );
	CDataChunk tChunk;
	tData.SafeRead(&tChunk.GetHdr(),sizeof(CDataChunk::ChunkHdr));
	ReadChunk(tData,tChunk);
	_DoDecode(tChunk,pEffectUnitProp);
	return true;
}


BEGIN_VERSION_MAP(CModelConsolePropCoder)
	ON_VERSION( 1 , DeCode_EffectUnitProp )
END_VERSION_MAP()

CModelConsolePropCoder::CModelConsolePropCoder()
{
	RegisterCoder(new CEffectUnitPropCoder);
}

bool CModelConsolePropCoder::Code(WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CModelConsoleProp* pEffectUnitProp = (CModelConsoleProp*)(handle);
	SaveStringWDIT( wdi, pEffectUnitProp->m_SkeName.c_str() );
	wdi.Write( &pEffectUnitProp->m_BirthTime,			sizeof(int) );
	wdi.Write( &pEffectUnitProp->m_DeleteTime,			sizeof(int) );

	SaveMapValueWDI(wdi, pEffectUnitProp->m_ClipDistMap);
	wdi.Write( &pEffectUnitProp->m_bEnableShadow, sizeof(pEffectUnitProp->m_bEnableShadow) );
	SaveMapValueWDI(wdi, pEffectUnitProp->m_mapRefract);
	wdi.Write(&pEffectUnitProp->m_ClipPlane.originPlane, sizeof(pEffectUnitProp->m_ClipPlane.originPlane));
	SaveMapValueWDI( wdi, pEffectUnitProp->m_XYZcale );
	pEffectUnitProp->m_fMaxXSize = pEffectUnitProp->m_XYZcale[0.0f].x;
	pEffectUnitProp->m_fMaxYSize = pEffectUnitProp->m_XYZcale[0.0f].y;
	pEffectUnitProp->m_fMaxZSize = pEffectUnitProp->m_XYZcale[0.0f].z;
	wdi.Write( (char*)&pEffectUnitProp->m_eFaceType,			sizeof(SPR_FACE) );

	SaveStringWDIT( wdi, pEffectUnitProp->m_szParentSk.c_str() );
	wdi.Write( &pEffectUnitProp->m_nSkIdx,				sizeof(int32) );

	wdi.Write( &pEffectUnitProp->m_fMinYSize,			sizeof(float) );
	wdi.Write( &pEffectUnitProp->m_fMaxYSize,			sizeof(float) );
	wdi.Write( &pEffectUnitProp->m_fMinZSize,			sizeof(float) );
	wdi.Write( &pEffectUnitProp->m_fMaxZSize,			sizeof(float) );

	wdi.Write( &pEffectUnitProp->m_vecTextMoveVel,		sizeof(CVector2f) );
	wdi.Write( &pEffectUnitProp->m_fTextAngularVel,		sizeof(float) );
	wdi.Write( &pEffectUnitProp->m_fTextMaxScale,		sizeof(float) );
	wdi.Write( &pEffectUnitProp->m_fTextMinScale,		sizeof(float) );

	if(!pEffectUnitProp->m_szARPName.empty())
	{
		size_t npos = pEffectUnitProp->m_szARPName.rfind(".arp");
		if(npos != -1)
		{
			pEffectUnitProp->m_szARPName = pEffectUnitProp->m_szARPName.substr(0, npos);
			pEffectUnitProp->m_szARPName += ".mod";
		}
	}
	SaveStringWDIT( wdi, pEffectUnitProp->m_szARPName.c_str() );

	if(!pEffectUnitProp->m_szARAName.empty())
	{
		size_t npos = pEffectUnitProp->m_szARAName.rfind(".ara");
		if(npos != -1)
		{
			pEffectUnitProp->m_szARAName = pEffectUnitProp->m_szARAName.substr(0, npos);
			pEffectUnitProp->m_szARAName += ".ske";
		}
	}
	SaveStringWDIT( wdi, pEffectUnitProp->m_szARAName.c_str() );

	wdi.Write( &pEffectUnitProp->m_vecRotate,			sizeof(CVector3f) );
	wdi.Write( &pEffectUnitProp->m_nPiece0,				sizeof(int32) );
	wdi.Write( &pEffectUnitProp->m_nRS0,					sizeof(int32) );
	wdi.Write( &pEffectUnitProp->m_nPiece1,				sizeof(int32) );
	wdi.Write( &pEffectUnitProp->m_nRS1,					sizeof(int32) );
	wdi.Write( &pEffectUnitProp->m_nPiece2,				sizeof(int32) );
	wdi.Write( &pEffectUnitProp->m_nRS2,					sizeof(int32) );
	wdi.Write( &pEffectUnitProp->m_nPiece3,				sizeof(int32) );
	wdi.Write( &pEffectUnitProp->m_nRS3,					sizeof(int32) );
	wdi.Write( &pEffectUnitProp->m_nPiece4,				sizeof(int32) );
	wdi.Write( &pEffectUnitProp->m_nRS4,					sizeof(int32) );
	wdi.Write( &pEffectUnitProp->m_nPiece5,				sizeof(int32) );
	wdi.Write( &pEffectUnitProp->m_nRS5,					sizeof(int32) );
	wdi.Write( &pEffectUnitProp->m_Animation,			sizeof(int32) );
	wdi.Write( &pEffectUnitProp->m_bSynch,				sizeof(BOOL) );
	wdi.Write( &pEffectUnitProp->m_fMinXSize,			sizeof(float) );
	wdi.Write( &pEffectUnitProp->m_fMaxXSize,			sizeof(float) );
	_AutoEncode(wdi,handle);
	return true;
}

bool CModelConsolePropCoder::DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CModelConsoleProp* pEffectUnitProp = (CModelConsoleProp*)(handle);
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	LoadStringSafeT( tData, pEffectUnitProp->m_SkeName );
	tData.SafeRead(&pEffectUnitProp->m_BirthTime,				sizeof(int) );
	tData.SafeRead(&pEffectUnitProp->m_DeleteTime,			sizeof(int) );
	LoadMapValueSafe(tData, pEffectUnitProp->m_ClipDistMap);
	tData.SafeRead(&pEffectUnitProp->m_bEnableShadow,		sizeof(bool) );
	if (pEffectUnitProp->m_ClipPlane.originPlane.IsValid() && pEffectUnitProp->m_ClipDistMap.empty())
	{
		pEffectUnitProp->m_ClipDistMap[0.0f] = -pEffectUnitProp->m_ClipPlane.originPlane.m_Dist;
		pEffectUnitProp->m_ClipDistMap[1.0f] = -pEffectUnitProp->m_ClipPlane.originPlane.m_Dist;
	}
	
	LoadMapValue(tData, pEffectUnitProp->m_mapRefract);
	tData.SafeRead(&pEffectUnitProp->m_ClipPlane.originPlane, sizeof(CPlane));
	LoadMapValue( tData, pEffectUnitProp->m_XYZcale );
	tData.SafeRead(&pEffectUnitProp->m_eFaceType,			sizeof(SPR_FACE) );
	LoadStringSafeT( tData, pEffectUnitProp->m_szParentSk );
	tData.SafeRead(&pEffectUnitProp->m_nSkIdx,			sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_fMinYSize,			sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fMaxYSize,			sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fMinZSize,			sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fMaxZSize,			sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_vecTextMoveVel,		sizeof(CVector2f) );
	tData.SafeRead(&pEffectUnitProp->m_fTextAngularVel,		sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fTextMaxScale,		sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fTextMinScale,		sizeof(float) );
	LoadStringSafeT( tData, pEffectUnitProp->m_szARPName );
	LoadStringSafeT( tData, pEffectUnitProp->m_szARAName );
	
	tData.SafeRead(&pEffectUnitProp->m_vecRotate,			sizeof(CVector3f) );
	tData.SafeRead(&pEffectUnitProp->m_nPiece0,			sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_nRS0,				sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_nPiece1,			sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_nRS1,				sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_nPiece2,			sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_nRS2,				sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_nPiece3,			sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_nRS3,				sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_nPiece4,			sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_nRS4,				sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_nPiece5,			sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_nRS5,				sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_Animation,			sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_bSynch,			sizeof(BOOL) ); 
	tData.SafeRead(&pEffectUnitProp->m_fMinXSize,			sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fMaxXSize,			sizeof(float) );

	if(pEffectUnitProp->CaculateProp())
		return false;
	CDataChunk tChunk;
	tData.SafeRead(&tChunk.GetHdr(),sizeof(CDataChunk::ChunkHdr));
	ReadChunk(tData,tChunk);
	_DoDecode(tChunk,pEffectUnitProp);
	return true;
}


BEGIN_VERSION_MAP(CModelRenderStylePropCoder)
	ON_VERSION( 1 , DeCode_EffectUnitProp )
END_VERSION_MAP()

CModelRenderStylePropCoder::CModelRenderStylePropCoder()
{
	RegisterCoder(new CEffectUnitPropCoder);
}

bool CModelRenderStylePropCoder::Code(WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CModelRenderStyleProp* pEffectUnitProp = (CModelRenderStyleProp*)(handle);
	SaveMapValueWDI(wdi, pEffectUnitProp->m_ClipDistMap);
	wdi.Write(&pEffectUnitProp->m_ClipPlane.originPlane, sizeof(CPlane));
	int32 n = pEffectUnitProp->m_ApplyClass.size();
	wdi.Write( (char*)&n,						sizeof(int32) );
	for( int32 i = 0; i < n; ++i )
		SaveStringWDIT( wdi, pEffectUnitProp->m_ApplyClass[i].c_str() );
	wdi.Write(&pEffectUnitProp->m_SpecularColor,		sizeof(CColor) );
	wdi.Write(&pEffectUnitProp->m_vecTextMoveVel,		sizeof(CVector2f) );
	wdi.Write(&pEffectUnitProp->m_fTextAngularVel,		sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_bDiffuseAmbient,		sizeof(BOOL) );
	wdi.Write(&pEffectUnitProp->m_bSpecular,			sizeof(BOOL) );
	wdi.Write(&pEffectUnitProp->m_bSetTextureMatrix,	sizeof(BOOL) );
	_AutoEncode(wdi,handle);
	return true;
}

bool CModelRenderStylePropCoder::DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CModelRenderStyleProp* pEffectUnitProp = (CModelRenderStyleProp*)(handle);
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	LoadMapValueSafe(tData, pEffectUnitProp->m_ClipDistMap);
	tData.SafeRead(&pEffectUnitProp->m_ClipPlane.originPlane, sizeof(CPlane));
	pEffectUnitProp->CaculateProp();
	int32 n = 0;
	tData.SafeRead( (char*)&n,						sizeof(int32) );
	pEffectUnitProp->m_ApplyClass.resize(n);
	for( int32 i = 0; i < n; ++i )
		LoadStringSafeT( tData, pEffectUnitProp->m_ApplyClass[i] );
	tData.SafeRead(&pEffectUnitProp->m_SpecularColor,			sizeof(CColor) );
	tData.SafeRead(&pEffectUnitProp->m_vecTextMoveVel,		sizeof(CVector2f) );
	tData.SafeRead(&pEffectUnitProp->m_fTextAngularVel,		sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_bDiffuseAmbient,		sizeof(BOOL) );
	tData.SafeRead(&pEffectUnitProp->m_bSpecular,				sizeof(BOOL) );
	tData.SafeRead(&pEffectUnitProp->m_bSetTextureMatrix,		sizeof(BOOL) );
	
	CDataChunk tChunk;
	tData.SafeRead(&tChunk.GetHdr(),sizeof(CDataChunk::ChunkHdr));
	ReadChunk(tData,tChunk);
	_DoDecode(tChunk,pEffectUnitProp);
	return true;
}

BEGIN_VERSION_MAP(CNullPropCoder)
	ON_VERSION( 1 , DeCode_EffectUnitProp )
END_VERSION_MAP()

CNullPropCoder::CNullPropCoder()
{
	RegisterCoder(new CEffectUnitPropCoder);
}

bool CNullPropCoder::Code(WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CNullProp* pEffectUnitProp = (CNullProp*)(handle);
	
	wdi.Write(&pEffectUnitProp->m_bDelay,				sizeof(bool) );
	wdi.Write(&pEffectUnitProp->m_vecRotate,			sizeof(CVector3f) );
	wdi.Write(&pEffectUnitProp->m_bFolowSpeed,			sizeof(BOOL) );
	_AutoEncode(wdi,handle);
	return true;
}

bool CNullPropCoder::DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CNullProp* pEffectUnitProp = (CNullProp*)(handle);
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	
	tData.SafeRead(&pEffectUnitProp->m_bDelay,			sizeof(bool) ); 
	tData.SafeRead(&pEffectUnitProp->m_vecRotate,		sizeof(CVector3f) );
	tData.SafeRead(&pEffectUnitProp->m_bFolowSpeed,		sizeof(BOOL) );
	CDataChunk tChunk;
	tData.SafeRead(&tChunk.GetHdr(),sizeof(CDataChunk::ChunkHdr));
	ReadChunk(tData,tChunk);
	_DoDecode(tChunk,pEffectUnitProp);
	return true;
}

BEGIN_VERSION_MAP(CParticleSystemPropCoder)
	ON_VERSION( 1 , DeCode_EffectUnitProp )
END_VERSION_MAP()

CParticleSystemPropCoder::CParticleSystemPropCoder()
{
	RegisterCoder(new CEffectUnitPropCoder);
}

bool CParticleSystemPropCoder::Code(WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CParticleSystemProp* pEffectUnitProp = (CParticleSystemProp*)(handle);

	wdi.Write(&pEffectUnitProp->m_eTextOp,					sizeof(SPR_BLENDOP) );

	SaveMapValueWDI(wdi, pEffectUnitProp->m_EmissionMinRadius);
	SaveMapValueWDI(wdi, pEffectUnitProp->m_EmissionMaxRadius);

	SaveMapValueWDI(wdi, pEffectUnitProp->m_ClipDistMap);
	wdi.Write(&pEffectUnitProp->m_ClipPlane.originPlane, sizeof(CPlane));
	SaveMapValueWDI( wdi, pEffectUnitProp->m_XYZcale );
	wdi.Write(&pEffectUnitProp->m_fLongShortRadius,			sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fCornerDivision,			sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fLongShortDRadius,		sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fZBias,					sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_sRefSkelatal,				sizeof(IDNAME) );
	wdi.Write(&pEffectUnitProp->m_fWidthRatio,				sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fVelocityPercent,			sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_eTextDir,					sizeof(CParticleSystemProp::ePS_TextureDir) );
	wdi.Write(&pEffectUnitProp->m_nEmissionInterval,		sizeof(int32) );
	wdi.Write(&pEffectUnitProp->m_nParticlesPerEmission,	sizeof(int32) );
	wdi.Write(&pEffectUnitProp->m_fMinSize,					sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fMaxSize,					sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_vEmissionPlan,			sizeof(CVector3f) );
	wdi.Write(&pEffectUnitProp->m_vMinVelocity,				sizeof(CVector3f) );
	wdi.Write(&pEffectUnitProp->m_vMaxVelocity,				sizeof(CVector3f) );
	wdi.Write(&pEffectUnitProp->m_vAcceleration,			sizeof(CVector3f) );
	wdi.Write(&pEffectUnitProp->m_fMinAngularVelocity,		sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fMaxAngularVelocity,		sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_nMinLifeSpan,				sizeof(int32) );
	wdi.Write(&pEffectUnitProp->m_nMaxLifeSpan,				sizeof(int32) );
	wdi.Write(&pEffectUnitProp->m_fMinRadius,				sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fMaxRadius,				sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_eFollowType,				sizeof(CParticleSystemProp::ePS_ParticlCoord) );
	wdi.Write(&pEffectUnitProp->m_eEmissionType,			sizeof(CParticleSystemProp::ePS_EMSType) );
	wdi.Write(&pEffectUnitProp->m_eVelType,					sizeof(CParticleSystemProp::ePS_VELType) );
	wdi.Write(&pEffectUnitProp->m_eBlendModel,				sizeof(SPR_BLENDMODE) );
	wdi.Write(&pEffectUnitProp->m_eZTest,					sizeof(Z_TEST) );
	wdi.Write(&pEffectUnitProp->m_bLightEnable,				sizeof(BOOL) );
	_AutoEncode(wdi,handle);
	return true;
}

bool CParticleSystemPropCoder::DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CParticleSystemProp* pEffectUnitProp = (CParticleSystemProp*)(handle);
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	tData.SafeRead(&pEffectUnitProp->m_eTextOp,			sizeof(SPR_BLENDOP) );
	LoadMapValueSafe( tData, pEffectUnitProp->m_EmissionMinRadius );
	LoadMapValueSafe( tData, pEffectUnitProp->m_EmissionMaxRadius );
	LoadMapValueSafe(tData, pEffectUnitProp->m_ClipDistMap);
	tData.SafeRead(&pEffectUnitProp->m_ClipPlane.originPlane, sizeof(CPlane));
	LoadMapValueSafe( tData, pEffectUnitProp->m_XYZcale );
	tData.SafeRead(&pEffectUnitProp->m_fLongShortRadius,			sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fCornerDivision,			sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fLongShortDRadius,			sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fZBias,					sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_sRefSkelatal,				sizeof(IDNAME) );
	tData.SafeRead(&pEffectUnitProp->m_fWidthRatio,				sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fVelocityPercent,			sizeof(float) );		
	tData.SafeRead(&pEffectUnitProp->m_eTextDir,					sizeof(CParticleSystemProp::ePS_TextureDir) );

	tData.SafeRead(&pEffectUnitProp->m_nEmissionInterval,		sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_nParticlesPerEmission,	sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_fMinSize,					sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fMaxSize,					sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_vEmissionPlan,			sizeof(CVector3f) );
	tData.SafeRead(&pEffectUnitProp->m_vMinVelocity,				sizeof(CVector3f) );
	tData.SafeRead(&pEffectUnitProp->m_vMaxVelocity,				sizeof(CVector3f) );
	tData.SafeRead(&pEffectUnitProp->m_vAcceleration,			sizeof(CVector3f) );
	tData.SafeRead(&pEffectUnitProp->m_fMinAngularVelocity,		sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fMaxAngularVelocity,		sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_nMinLifeSpan,				sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_nMaxLifeSpan,				sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_fMinRadius,				sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fMaxRadius,				sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_eFollowType,				sizeof(CParticleSystemProp::ePS_ParticlCoord) );
	tData.SafeRead(&pEffectUnitProp->m_eEmissionType,			sizeof(CParticleSystemProp::ePS_EMSType) );
	tData.SafeRead(&pEffectUnitProp->m_eVelType,					sizeof(CParticleSystemProp::ePS_VELType) );
	tData.SafeRead(&pEffectUnitProp->m_eBlendModel,				sizeof(SPR_BLENDMODE) );
	tData.SafeRead(&pEffectUnitProp->m_eZTest,					sizeof(Z_TEST) );
	tData.SafeRead(&pEffectUnitProp->m_bLightEnable,				sizeof(BOOL) );
	CDataChunk tChunk;
	tData.SafeRead(&tChunk.GetHdr(),sizeof(CDataChunk::ChunkHdr));
	ReadChunk(tData,tChunk);
	_DoDecode(tChunk,pEffectUnitProp);
	return true;
}


BEGIN_VERSION_MAP(CPolyTrailPropCoder)
	ON_VERSION( 1 , DeCode_EffectUnitProp )
END_VERSION_MAP()

CPolyTrailPropCoder::CPolyTrailPropCoder()
{
	RegisterCoder(new CEffectUnitPropCoder);
}

bool CPolyTrailPropCoder::Code(WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CPolyTrailProp* pEffectUnitProp = (CPolyTrailProp*)(handle);

	wdi.Write(&pEffectUnitProp->m_eTextOp,				sizeof(SPR_BLENDOP));
	wdi.Write(&pEffectUnitProp->m_fRefractIndex,		sizeof(float));
	wdi.Write(&pEffectUnitProp->m_DeclTime,			sizeof(int32) );
	wdi.Write(&pEffectUnitProp->m_Smooth,				sizeof(int32) );
	wdi.Write(&pEffectUnitProp->m_sRefSkelatal,		sizeof(IDNAME) );
	wdi.Write(&pEffectUnitProp->m_bSingleSide,			sizeof(BOOL) );
	wdi.Write(&pEffectUnitProp->m_eStrip,				sizeof(POLY_TRAIL_TYPE) );
	wdi.Write(&pEffectUnitProp->m_vecRotate,			sizeof(CVector3f) );
	wdi.Write(&pEffectUnitProp->m_MinTrailWidth,		sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_MaxTrailWidth,		sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_UnitLifeSpan,			sizeof(int32) );
	wdi.Write(&pEffectUnitProp->m_eBlendModel,			sizeof(SPR_BLENDMODE) );
	wdi.Write(&pEffectUnitProp->m_eZTest,				sizeof(Z_TEST) );
	wdi.Write(&pEffectUnitProp->m_bLightEnable,			sizeof(BOOL) );
	_AutoEncode(wdi,handle);
	return true;
}

bool CPolyTrailPropCoder::DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CPolyTrailProp* pEffectUnitProp = (CPolyTrailProp*)(handle);
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	tData.SafeRead(&pEffectUnitProp->m_eTextOp,				sizeof(SPR_BLENDOP));
	tData.SafeRead(&pEffectUnitProp->m_fRefractIndex, sizeof(float));
	tData.SafeRead(&pEffectUnitProp->m_DeclTime,				sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_Smooth,				sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_sRefSkelatal,			sizeof(IDNAME) );
	tData.SafeRead(&pEffectUnitProp->m_bSingleSide,			sizeof(BOOL) ); 
	tData.SafeRead(&pEffectUnitProp->m_eStrip,				sizeof(POLY_TRAIL_TYPE) );
	tData.SafeRead(&pEffectUnitProp->m_vecRotate,				sizeof(CVector3f) );
	tData.SafeRead(&pEffectUnitProp->m_MinTrailWidth,			sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_MaxTrailWidth,			sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_UnitLifeSpan,			sizeof(int32) );
	tData.SafeRead(&pEffectUnitProp->m_eBlendModel,			sizeof(SPR_BLENDMODE) );
	tData.SafeRead(&pEffectUnitProp->m_eZTest,				sizeof(Z_TEST) );
	tData.SafeRead(&pEffectUnitProp->m_bLightEnable,			sizeof(BOOL) );
	CDataChunk tChunk;
	tData.SafeRead(&tChunk.GetHdr(),sizeof(CDataChunk::ChunkHdr));
	ReadChunk(tData,tChunk);
	_DoDecode(tChunk,pEffectUnitProp);
	return true;
}


BEGIN_VERSION_MAP(CShakePropCoder)
	ON_VERSION( 1 , DeCode_EffectUnitProp )
END_VERSION_MAP()

CShakePropCoder::CShakePropCoder()
{
	RegisterCoder(new CEffectUnitPropCoder);
}

bool CShakePropCoder::Code(WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CShakeProp* pEffectUnitProp = (CShakeProp*)(handle);

	wdi.Write(&pEffectUnitProp->m_vecDirCtrl,			sizeof(CVector3f) );
	wdi.Write(&pEffectUnitProp->m_vecFrequence,			sizeof(CVector3f) );
	wdi.Write(&pEffectUnitProp->m_fStrength,			sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_isCtrlParent,			sizeof(bool) );
	_AutoEncode(wdi,handle);
	return true;
}

bool CShakePropCoder::DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CShakeProp* pEffectUnitProp = (CShakeProp*)(handle);
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	tData.SafeRead(&pEffectUnitProp->m_vecDirCtrl,			sizeof(CVector3f) );
	tData.SafeRead(&pEffectUnitProp->m_vecFrequence,			sizeof(CVector3f) );
	tData.SafeRead(&pEffectUnitProp->m_fStrength,				sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_isCtrlParent,			sizeof(bool) );
	CDataChunk tChunk;
	tData.SafeRead(&tChunk.GetHdr(),sizeof(CDataChunk::ChunkHdr));
	ReadChunk(tData,tChunk);
	_DoDecode(tChunk,pEffectUnitProp);
	return true;
}


BEGIN_VERSION_MAP(CSpritePropCoder)
	ON_VERSION( 1 , DeCode_EffectUnitProp )
END_VERSION_MAP()

CSpritePropCoder::CSpritePropCoder()
{
	RegisterCoder(new CEffectUnitPropCoder);
}

bool CSpritePropCoder::Code(WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CSpriteProp* pEffectUnitProp = (CSpriteProp*)(handle);

	wdi.Write(&pEffectUnitProp->m_BirthTime,			sizeof(int) );
	wdi.Write(&pEffectUnitProp->m_DeleteTime,			sizeof(int) );
	wdi.Write(&pEffectUnitProp->m_eTextOp,				sizeof(SPR_BLENDOP) );


	SaveMapValueWDI( wdi, pEffectUnitProp->m_UVRCenter );
	SaveMapValueWDI( wdi, pEffectUnitProp->m_XYZcale );
	pEffectUnitProp->m_fWidthRatio = pEffectUnitProp->m_XYZcale[0.0f].x;
	pEffectUnitProp->m_fMaxSize = pEffectUnitProp->m_XYZcale[0.0f].y;

	wdi.Write(&pEffectUnitProp->m_fStartAngle,			sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fWidthRatio,			sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fZBias,				sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_vecRotate,			sizeof(CVector3f) );
	wdi.Write(&pEffectUnitProp->m_vecNormal,			sizeof(CVector3f) );
	wdi.Write(&pEffectUnitProp->m_fMinSize,				sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_fMaxSize,				sizeof(float) );
	wdi.Write(&pEffectUnitProp->m_eFaceType,			sizeof(SPR_FACE) );
	wdi.Write(&pEffectUnitProp->m_eBlendModel,			sizeof(SPR_BLENDMODE) );
	wdi.Write(&pEffectUnitProp->m_eZTest,				sizeof(Z_TEST) );
	wdi.Write(&pEffectUnitProp->m_bLightEnable,			sizeof(BOOL) );
	_AutoEncode(wdi,handle);
	return true;
}

bool CSpritePropCoder::DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CSpriteProp* pEffectUnitProp = (CSpriteProp*)(handle);
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	tData.SafeRead(&pEffectUnitProp->m_BirthTime,				sizeof(int) );
	tData.SafeRead(&pEffectUnitProp->m_DeleteTime,			sizeof(int) );
	tData.SafeRead(&pEffectUnitProp->m_eTextOp,				sizeof(SPR_BLENDOP) );
	LoadMapValueSafe( tData, pEffectUnitProp->m_UVRCenter );
	LoadMapValueSafe( tData, pEffectUnitProp->m_XYZcale );
	tData.SafeRead(&pEffectUnitProp->m_fStartAngle,			sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fWidthRatio,			sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fZBias,				sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_vecRotate,				sizeof(CVector3f) );
	tData.SafeRead(&pEffectUnitProp->m_vecNormal,				sizeof(CVector3f) );
	tData.SafeRead(&pEffectUnitProp->m_fMinSize,				sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_fMaxSize,				sizeof(float) );
	tData.SafeRead(&pEffectUnitProp->m_eFaceType,				sizeof(SPR_FACE) );
	tData.SafeRead(&pEffectUnitProp->m_eBlendModel,			sizeof(SPR_BLENDMODE) );
	tData.SafeRead(&pEffectUnitProp->m_eZTest,				sizeof(Z_TEST) );
	tData.SafeRead(&pEffectUnitProp->m_bLightEnable,			sizeof(BOOL) );

	pEffectUnitProp->CaculateProp();
	CDataChunk tChunk;
	tData.SafeRead(&tChunk.GetHdr(),sizeof(CDataChunk::ChunkHdr));
	ReadChunk(tData,tChunk);
	_DoDecode(tChunk,pEffectUnitProp);
	return true;
}


BEGIN_VERSION_MAP(CTDimAudioPropCoder)
	ON_VERSION( 1 , DeCode_EffectUnitProp )
END_VERSION_MAP()

CTDimAudioPropCoder::CTDimAudioPropCoder()
{
	RegisterCoder(new CEffectUnitPropCoder);
}

bool CTDimAudioPropCoder::Code(WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTDimAudioProp* pEffectUnitProp = (CTDimAudioProp*)(handle);

	wdi.Write(&pEffectUnitProp->m_bDelayStop, sizeof(bool));
	wdi.Write(&pEffectUnitProp->m_fPlayProbability,sizeof(float));

	SaveStringWDIT(wdi, pEffectUnitProp->m_szAudioName.c_str());
	_AutoEncode(wdi,handle);
	return true;
}

bool CTDimAudioPropCoder::DeCode_EffectUnitProp( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CTDimAudioProp* pEffectUnitProp = (CTDimAudioProp*)(handle);
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );

	tData.SafeRead( &pEffectUnitProp->m_bDelayStop,	sizeof( bool ) );
	tData.SafeRead( &pEffectUnitProp->m_fPlayProbability,	sizeof( float ) );
	LoadStringSafeT( tData, pEffectUnitProp->m_szAudioName );
	CDataChunk tChunk;
	tData.SafeRead(&tChunk.GetHdr(),sizeof(CDataChunk::ChunkHdr));
	ReadChunk(tData,tChunk);
	_DoDecode(tChunk,pEffectUnitProp);
	return true;
}
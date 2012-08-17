#pragma once
#include "CoreCommon.h"
#include "TBaseMetaSceneServer.h"
#include "TMetaScene.inl"
#include "TWhole.inl"
#include "TPart.inl"
#include "ErrLogHelper.h"

template<typename CTraits>
TBaseMetaSceneServer<CTraits>::TBaseMetaSceneServer(  const char* szSceneName, const uint32 sceneId, ImpMetaSceneMgr_t* pMgr )
: ParentMetaScene_t( szSceneName,sceneId, pMgr )
{
}

template<typename CTraits>
void TBaseMetaSceneServer<CTraits>::PreLoadRegionInfo()
{
	//m_vecAbsoluteHeight.Alloc( m_uWidthInGrid, m_uHeightInGrid );
	//m_vecBlock.Alloc( m_uWidthInGrid, m_uHeightInGrid );
	ParentMetaScene_t::PreLoadRegionInfo();
	/*m_pTrapMgr = new CTrapMgr( ParentMetaScene_t::m_uWidthInGrid, ParentMetaScene_t::m_uHeightInGrid );
	Ast( m_pTrapMgr );

	m_pLogicObjMgr = new CLogicObjMgr();
	Ast( m_pLogicObjMgr );*/

}

template<typename CTraits>
TBaseMetaSceneServer<CTraits>::~TBaseMetaSceneServer(void)
{
	if( HasObject() )
		GenErr("It is not allow to destroy metascene before all CoreObject in the CoreScene classify by which is destroyed!");
}

template<typename CTraits>
const char* TBaseMetaSceneServer<CTraits>::GetRegionFileName( string& str )	
{
	str = ParentMetaScene_t::m_sSceneName + "_s.arg";
	return str.c_str();
}

template<typename CTraits>
uint32 TBaseMetaSceneServer<CTraits>::GetRegionVersionID() const
{
	return ParentMetaScene_t::m_nVersionID;
}


template<typename CTraits>
void TBaseMetaSceneServer<CTraits>::LoadOneChunk( CPkgFile& File, uint32 uRegionId, uint32 uMagicID )
{
	switch(uMagicID)
	{
	//case RGFG:
	//	LoadFlag(File);
	//	break;
	case TRAP:
		LoadRegionTrap( File, uRegionId );
		break;
	case OBJT:
		LoadRegionObject( File, uRegionId );
		break;
	case GHGT:
		LoadRegionAbsoluteHeight( File, uRegionId );
		break;
	default:
		ParentMetaScene_t::LoadOneChunk( File, uRegionId, uMagicID);
	}
}

template<typename CTraits>
void TBaseMetaSceneServer<CTraits>::LoadRegionTrap( CPkgFile& File, uint32 uRegionId )
{
	uint16 uTrapNum;
	File.read(&uTrapNum, sizeof(uint16));
	for( uint16 i = 0; i < uTrapNum; i++ )
	{
		uint8 uIdx;
		uint8 uVal;
		File.read(&uIdx, sizeof(uint8));
		File.read(&uVal, sizeof(uint8));
	}
}

template<typename CTraits>
void TBaseMetaSceneServer<CTraits>::LoadRegionObject( CPkgFile& File, uint32 uRegionId )
{
	uint16 uObjectNum;
	File.read( &(uObjectNum) , sizeof(int16) );

	for( uint16 i = 0; i < uObjectNum; ++i )
	{
		uint32 GridIndex;
		uint16 GroupId;
		uint16 IndexInGroup;
		uint8  Dir;
		uint16 LogicObjId;
		uint16 ExtraInfoSize;
		// string ExtraInfo;

		File.read( &GridIndex,    sizeof(uint32) );
		File.read( &GroupId,      sizeof(uint16) );
		File.read( &IndexInGroup, sizeof(uint16) );
		File.read( &Dir,			sizeof(uint8) );
		File.read( &LogicObjId,		sizeof(uint16) );
		File.read( &ExtraInfoSize, sizeof(uint16) );
		char * ExtraInfo = new char[ ExtraInfoSize + 1 ];

		File.read( ExtraInfo, sizeof(char) * ExtraInfoSize );
		ExtraInfo[ ExtraInfoSize ] = '\0';
		// m_ExtraInfo = string(t);
		//CLogicObj obj = CLogicObj( GridIndex, GroupId, IndexInGroup, Dir, LogicObjId, ExtraInfo );
		//m_pLogicObjMgr->AddLogicObj(obj);

		//CLogicObj const * p = m_pLogicObjMgr->GetLogicObj( GroupId, IndexInGroup );
		//cout<<"Load Object: GridIndex " << p->m_GridIndex <<
		//	"  GroupID "				<< p->m_GroupId	<<
		//	"  IndexInGroup "			<< p->m_IndexInGroup <<
		//	"  Direction "			<< (uint16)p->m_Dir <<
		//	"  ObjectId "				<< p->m_LogicObjId	<<
		//	"  ExtraInfo ["			<< p->m_ExtraInfo << "]" << endl;

	}
}

template<typename CTraits>
void TBaseMetaSceneServer<CTraits>::LoadRegionAbsoluteHeight( CPkgFile& File, uint32 uRegionId )
{
	for(uint32 i = 0; i < eRegionSpan; i++)
	{
		for(uint32 j = 0; j < eRegionSpan; j++)
		{
			int16 nVal;
			File.read( &(nVal) , sizeof(int16) );
			//SetAbsoluteHeight( GetRegionLeftX(uRegionId) + j,
			//	GetRegionDownY(uRegionId) + i, nVal );
		}
	}
}

template<typename CTraits>
bool TBaseMetaSceneServer<CTraits>::HasObject()const
{
	typedef list<typename CTraits::CoreScene_t*> ListType;
	const ListType& lstCoreScene = ParentMetaScene_t::GetParts();

	for( typename ListType::const_iterator it=lstCoreScene.begin() ; it != lstCoreScene.end() ; ++it )
	{
		if( (*it)->HasObject() )
			return true;
	}
	return false;
}

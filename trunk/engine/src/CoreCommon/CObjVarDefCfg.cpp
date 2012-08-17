#include "stdafx.h"
#include "CObjVarDef.h"
#include "CObjVarDefCfg.h"
#include "ExpHelper.h"

CObjVarDefCfg::CObjVarDefCfg(void)
{
}

CObjVarDefCfg::~CObjVarDefCfg(void)
{
	for( map<uint32,CObjVarDef*>::iterator it = m_mapId2Def.begin(); it != m_mapId2Def.end(); ++it )
		delete it->second;
}

uint32& CObjVarDefCfg::GetCurDefID()
{
	static uint32 ls_uCurID=0;
	return ls_uCurID;
}

uint32 CObjVarDefCfg::Get(const char* szFileName)
{
	uint32 uCurDefID = GetCurDefID();

	pair<map<string,uint32>::iterator,bool> p=
		m_mapName2Id.insert(   make_pair(szFileName,uCurDefID)   );
	
	if( !p.second )
		return p.first->second;



	CObjVarDef* pDef=new CObjVarDef( szFileName );

	Ver( m_mapId2Def.insert(   make_pair( uCurDefID, pDef )    ).second );
		
	GetCurDefID() = uCurDefID + 1;

	return uCurDefID;
}

CObjVarDef* CObjVarDefCfg::Get(uint32 uDefId)const
{
	map<uint32,CObjVarDef*>::const_iterator it=m_mapId2Def.find( uDefId );
	
	if( it == m_mapId2Def.end() )
		return NULL;

	return it->second;
}

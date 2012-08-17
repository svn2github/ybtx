/*
 * =====================================================================================
 *
 *       Filename:  CGlobalObjVarDefCfg.cpp
 *
 *    Description:  全局Variant配置索引
 *
 *        Version:  1.0
 *        Created:  2008年09月20日 17时11分10秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  廖鑫炜 (Comet), liaoxinwei@linekong.com
 *        Company:  ThreeOGCMan
 *
 * =====================================================================================
 */

#include "stdafx.h"
#include "CGlobalObjVarDefCfg.h"
#include "ExpHelper.h"

CGlobalObjVarDefCfg::CGlobalObjVarDefCfg()
{
	m_pCfg = NULL;
}

bool CGlobalObjVarDefCfg::AddName2ID(const TCHAR* szName, uint32 uID)
{
	return mapName2ID.insert(make_pair(string(szName), uID)).second;
}

uint32 CGlobalObjVarDefCfg::GetIDByName(const TCHAR* szName)
{
	std::map<string, uint32>::iterator it = mapName2ID.find(szName);
	if (it == mapName2ID.end()) 
	{
		ostringstream oss;
		oss << "Can't find Variant Def:" << szName;
		GenErr(oss.str());
	}
	return it->second;
}

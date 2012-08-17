#pragma once

namespace UpdateCfg
{

	template<typename CfgStringMapType, typename CfgType, typename CfgSharedPtrType>
	bool UpdateCfg(CfgStringMapType& mapCfg, const std::string& strName)
	{
		typename CfgStringMapType::iterator it = mapCfg.find(strName);
		if(it != mapCfg.end())
		{
			CfgSharedPtrType* pOldCfg = it->second;
			mapCfg.erase(it);
			CfgSharedPtrType* pNewCfg = new CfgSharedPtrType(new CfgType(*(pOldCfg->get())));
			CfgChk::InsertStringSharedPtrMap(mapCfg, pNewCfg);
			delete pOldCfg;
			return true;
		}
		return false;
	}

	template<typename CfgStringMapType, typename CfgType, typename CfgSharedPtrType>
	bool UpdateCfgAndNotifyStringRefer(CfgStringMapType& mapCfg, const std::string& strName)
	{
		typename CfgStringMapType::iterator it = mapCfg.find(strName);
		if(it != mapCfg.end())
		{
			CfgSharedPtrType* pOldCfg = it->second;
			mapCfg.erase(it);
			CfgSharedPtrType* pNewCfg = new CfgSharedPtrType(new CfgType(*(pOldCfg->get())));
			CfgChk::InsertStringSharedPtrMap(mapCfg, pNewCfg);
			pOldCfg->get()->NotifyRefer();
			delete pOldCfg;
			return true;
		}
		return false;
	}
}


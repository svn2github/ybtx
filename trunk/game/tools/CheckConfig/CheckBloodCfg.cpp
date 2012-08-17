#include "stdafx.h"
#include "CheckBloodCfg.h"
#include "CCfgBloodCheck.h"
#include "CCfgColChecker.inl"
#include "CScriptAppServer.h"

bool CheckBloodCfg()
{
	string strCfgFilePath = CScriptAppServer::Inst()->GetCfgFilePath("");

	string strPlayerPath;
	strPlayerPath = strCfgFilePath + "/player/";

	string strFileName = strPlayerPath + "BloodType_Client.txt";
	if(	!CCfgBloodCheck::Check(strFileName.c_str()) )
	{
		ostringstream strm;
		strm<<"¼ÓÔØ£º"<< strFileName <<" Ê§°Ü";
		CfgChk::GenExpInfo(strm.str());
		return false;
	}
	return true;
}

void EndCheckBloodCfg()
{
	CCfgBloodCheck::EndCheck();
}

#include "stdafx.h"
#include "CCfgColChecker.inl"
#include "CAppServer.h"
#include "TimeHelper.h"
#include "ErrLogHelper.h"
#include "StringHelper.h"

namespace CfgChk
{

	bool g_bIsCheckingCfg = false;
	CTxtTableFile* g_pTabFile = NULL;
	string g_sTabName = "";
	string g_sSrcTabLineName = "";
	string g_sMagicEffName = "";
	int32 g_iLine = 0;					//行号
	string g_sTitle = "";				//列标题
	EErrorReactType g_uErrReact = eERT_AtEnd;			//错误反应 0:立即异常 1:结束异常 2:不异常
	bool g_bExistError = false;
	uint64 g_uTimeForTest = 0;
	bool g_bCheckInReading = true;		//默认读表时不检测

	bool	GT(const float x, const float a) {return x > a;}
	bool	GE(const float x, const float a) {return x >= a;}
	bool	LT(const float x, const float a) {return x < a;}
	bool	LE(const float x, const float a) {return x <= a;}
	bool	EQ(const float x, const float a) {return x == a;}
	bool	NE(const float x, const float a) {return x != a;}
	bool	IsRate(const float x, const float a) {return x >= 0.0f && x <= 1.0f;}

	bool	ExistError() {return g_bExistError;}
	bool	ExpIsForbidden() {return g_uErrReact == eERT_Never;}
	bool	ExpAtOnce() {return g_uErrReact == eERT_AtOnce;}
	bool	ExpAtEnd() {return g_uErrReact == eERT_AtEnd;}

void GenExpInfo(const string& sExp)
{
	g_bExistError = true;
	if(g_bIsCheckingCfg)
	{
		cout << sExp << "\n";
		if(ExpAtOnce())
			CfgLogErr(sExp.c_str());
	}
	else
	{
		CfgLogErr(sExp.c_str());
	}
}

void SetTabFile(CTxtTableFile& TabFile, const string& TabName) 
{
	uint64 uCurTime = GetProcessTime();
	cout << "\t耗时" << uCurTime - g_uTimeForTest << "毫秒\n[" << uCurTime << "]读取并检测技能配置表[" << TabName << "]开始";
	g_uTimeForTest = uCurTime;
	g_bIsCheckingCfg = true;
	g_pTabFile = &TabFile;
	g_sTabName = TabName;
	g_sSrcTabLineName.clear();
	g_sMagicEffName.clear();
	g_iLine = 0;
	//g_bCheckingInReading = true;
	g_sTitle.clear();

}

void ResetTab(CTxtTableFile& TabFile, const string& TabName)
{
	g_bIsCheckingCfg = true;
	g_pTabFile = &TabFile;
	g_sTabName.clear();
	g_sSrcTabLineName.clear();
	g_sMagicEffName.clear();
	g_sTabName = TabName;
	g_iLine = 0;
	//g_bCheckingInReading = true;
	g_sTitle.clear();
}

void SetTabName(const string& TabName)
{
	uint64 uCurTime = GetProcessTime();
	cout << "\t耗时" << uCurTime - g_uTimeForTest << "毫秒\n[" << uCurTime << "]关系检测技能配置表[" << TabName << "]开始";
	g_uTimeForTest = uCurTime;
	g_bIsCheckingCfg = true;
	g_pTabFile = NULL;
	g_sSrcTabLineName.clear();
	g_sMagicEffName.clear();
	g_sTabName = TabName;
	g_iLine = 0;
	//g_bCheckingInReading = false;
	g_sTitle.clear();

}

void SetTabNull()
{
	g_bIsCheckingCfg = false;
	g_pTabFile = NULL;
	g_sSrcTabLineName.clear();
	g_sMagicEffName.clear();
	g_sTabName.clear();
	g_iLine = 0;
	g_sTitle.clear();
}

void SetLineNo(int32 iLine)
{
	g_iLine = iLine;
}

void SetItemTitle(const TCHAR* sTitle)
{
	g_sTitle = sTitle;
}



//SetValue()
void SetValue(int32& tReader, const string& sValue)
{
	stringstream sTempValue;
	tReader = atoi(sValue.c_str());
	sTempValue << tReader;
	if(sTempValue.str() != sValue)
	{
		GenExpInfo("违反列约束：整型列不能为浮点数或非数值", sValue);
		//return false;
	}
}

void SetValue(float& tReader, const string& sValue)
{
	//stringstream sTempValue;
	tReader = float(atof(sValue.c_str()));
	//sTempValue << tReader;

	if(g_bCheckInReading)
	{
		if(tReader == 0.0f)
		{
			if(/*sTempValue.str()*/ "0" != sValue)
			{
				GenExpInfo("违反列约束：浮点型列不能为非数值", sValue);
				//return false;
			}
		}
	}
}

void SetValue(bool& tReader, const string& sValue)
{
	if(sValue == "是" || sValue == "被")
	{
		tReader = true;
	}
	else if(sValue == "否" || sValue == "主")
	{
		tReader = false;
	}
	else
	{
		GenExpInfo("违反列约束：布尔型列不能取是和否以外的值", sValue);
	}
}

void SetValue(string& tReader, const string& sValue)
{
	tReader = sValue;
}

void SetValue(CCfgCalc*& pCalc, const string& sValue, bool bIsSingle)
{
	//这里考虑要不要判断NULL，如果这样则需要在各配置表构造函数里把CCfgCalc*成员置为NULL
	pCalc = new CCfgCalc;
	pCalc->InputString(sValue);
	if(bIsSingle) pCalc->SetTypeExpression();
}

//CheckType()
void CheckUIntType(int32& tReader)
{
	if(g_bCheckInReading)
	{
		if(tReader < 0)
		{
			GenExpInfo("违反列约束：非负整型列不能取负数", tReader);
			//return false;
		}
	}
}

//CheckField()
void CheckField(string& sValue, FIELD_CHECKER_STRING pFunFieldChecker)
{
	if(g_bCheckInReading)
	{
		if(pFunFieldChecker)
		{
			if(!pFunFieldChecker(sValue))
			{
				GenExpInfo("违反列约束：字符串超出取值范围", sValue);
			}
		}
	}
}

//ReadItem()
void ReadItem(uint32& tReader, const TCHAR* sTitle,
			  FIELD_CHECKER pFunFieldChecker, uint32 fFieldCheckerParam)
{
	ReadItem(tReader, sTitle, false, 0, pFunFieldChecker, fFieldCheckerParam);
}

void ReadItem(uint32& tReader, const TCHAR* sTitle, bool bCanEmpty, 
							  uint32 tDefault, FIELD_CHECKER pFunFieldChecker, uint32 fFieldCheckerParam)
{
	SetItemTitle(sTitle);
	string sValue = g_pTabFile->GetString(g_iLine, sTitle);
	CheckEmpty(tReader, sValue, bCanEmpty, tDefault);
	if(!sValue.empty())
	{
		int32 tTempReader;
		SetValue(tTempReader, sValue);
		CheckUIntType(tTempReader);
		tReader = tTempReader;
	}
	CheckField(tReader, pFunFieldChecker, fFieldCheckerParam);
}

void ReadItem(string& tReader, const TCHAR* sTitle, 
			  FIELD_CHECKER_STRING pFunFieldChecker)
{
	SetItemTitle(sTitle);
	ReadItem(tReader, sTitle, false, "", pFunFieldChecker);
}

void ReadItem(string& tReader, const TCHAR* sTitle, bool bCanEmpty,
			  const string& tDefault, FIELD_CHECKER_STRING pFunFieldChecker)
{
	SetItemTitle(sTitle);
	string sValue = g_pTabFile->GetString(g_iLine, sTitle);
	size_t l = sValue.size();
	if(l > 1 && sValue[0] == '"' && sValue[l - 1] == '"') sValue = sValue.substr(1, l - 2);
	trimend(sValue);
	CheckEmpty(tReader, sValue, bCanEmpty, tDefault);
	if(!sValue.empty())
	{
		SetValue(tReader, sValue);
	}
	CheckField(tReader, pFunFieldChecker);
}

//typedef CCfgCalc* CalcType;
void ReadItem(CCfgCalc*& tReader, const TCHAR* sTitle, FIELD_CHECKER pFunFieldChecker,
			  float fFieldCheckerParam)
{
	ReadItem(tReader, sTitle, false, 0.0f, pFunFieldChecker, fFieldCheckerParam);
}

void ReadItem(CCfgCalc*& tReader, const TCHAR* sTitle, bool bCanEmpty, 
			  float tDefault, FIELD_CHECKER pFunFieldChecker, float fFieldCheckerParam)
{
	SetItemTitle(sTitle);
	string sValue = g_pTabFile->GetString(g_iLine, sTitle);
	trimend(sValue);
	CheckEmpty(tReader, sValue, bCanEmpty, tDefault);
	if(!sValue.empty())
	{
		SetValue(tReader, sValue);
	}
	if(pFunFieldChecker && tReader->IsSingleNumber())
	{
		float fTemp = float(tReader->GetDblValue());
		CheckField(fTemp, pFunFieldChecker, fFieldCheckerParam);
	}
}

void ReadMixedItem(CCfgCalc*& tReader, const TCHAR* sTitle, bool bCanEmpty, 
			  const string& tDefault)
{
	SetItemTitle(sTitle);
	string sValue = g_pTabFile->GetString(g_iLine, sTitle);
	trimend(sValue);
	CheckEmpty(tReader, sValue, bCanEmpty, tDefault);
	if(!sValue.empty())
	{
		SetValue(tReader, sValue, false);
	}
}


}//end namespace spaceCfgColChecke


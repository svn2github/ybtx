#include "stdafx.h"
#include "CCfgCalc.inl"
#include <cmath>
#include "CCfgColChecker.inl"
#include "BaseHelper.h"
#include "CTempVarMgr.h"
#include "TSqrAllocator.inl"
#include "StringHelper.h"

#ifdef _WIN32
#pragma warning (disable:4996)
#endif

void CExprUnit::Set(double value)
{
	m_Value.flt = value;
}

void CExprUnit::Set(uint32 value)
{
	m_Value.lng = value;
}

void CExprUnit::Set(string* value)
{
	m_Value.pStr = value;
}

void CExprUnit::Set(double value, EUnitType type)
{
	m_Value.flt = value;
	m_eType = type;
}

void CExprUnit::Set(uint32 value, EUnitType type)
{
	m_Value.lng = value;
	m_eType = type;
}

void CExprUnit::Set(string* value, EUnitType type)
{
	m_Value.pStr = value;
	m_eType = type;
}

void CExprUnit::SetType(EUnitType type)
{
	m_eType = type;
}

void CExprUnit::SetFunType(EFunType type)
{
	m_eFunType = type;
}

void CExprUnit::SetTargetType(ECalcTargetType type)
{
	m_eTargetType = type;
}





CExpression::CExpression(const string& str, const CCfgCalc* pCfgCalc)
: //m_pTalentHolder(NULL), 
  //m_pSkillInst(NULL),
  m_pVecArgs(&pCfgCalc->m_vecArgs),
  m_pCfgCalc(pCfgCalc)
{
	Input(str, m_pVecArgs);
}

CExpression::CExpression(double dblValue, const CCfgCalc* pCfgCalc)
: //m_pTalentHolder(NULL), 
  //m_pSkillInst(NULL),
  m_pVecArgs(NULL),
  m_pCfgCalc(pCfgCalc)
{
	Input(dblValue);
}

CExpression::CExpression(const CExpression& srcExpr, const CCfgCalc* pCfgCalc)
: m_sExprStr(srcExpr.m_sExprStr),
  m_vecExpr(srcExpr.m_vecExpr),
  m_pVecArgs(&pCfgCalc->m_vecArgs),
  m_pCfgCalc(pCfgCalc)
{
	size_t l =  srcExpr.m_vecPreExpr.size();
	m_vecPreExpr.assign(l, NULL);
	for(size_t i = 0; i < l; ++i)
	{
		m_vecPreExpr[i] = new CPreUnit(*srcExpr.m_vecPreExpr[i]);
	}
	l = srcExpr.m_vecOpndArray.size();
	m_vecOpndArray.assign(l, NULL);
	for(size_t i = 0; i < l; ++i)
	{
		m_vecOpndArray[i] = new VecOpnd(*srcExpr.m_vecOpndArray[i]);
	}
}

CExpression::~CExpression()
{
	ClearVector(m_vecPreExpr);
	ClearVector(m_vecOpndArray);
}

	

void CExpression::Input(const string& str, VecArgs* pVecArgs)
{
	//m_vecPreExpr.clear();
	//m_vecExpr.clear();
	m_sExprStr = str;
	m_pVecArgs = pVecArgs;
	Separate();		
	Convert();
}

void CExpression::Input(double dblValue)
{
	//m_sExprStr.clear();
	//m_vecPreExpr.clear();
	m_vecExpr.clear();
	CExprUnit cUnit(dblValue);
	m_vecExpr.push_back(cUnit);
	CExprUnit cUnit2(uint32(eOI_Shell), eUT_Symbol);				//设置结束符
	m_vecExpr.push_back(cUnit2);
}

void CExpression::GetCharType(TCHAR c, EUnitType &type) {
	static string symbolchars = ",=<>()[]+-*/%!&|^~:#$";
	//以下为保留字可字母跟数字组合（但必须以字母开始）的脚本解释
	if (type == eUT_String)
	{
		if (c == '"') type = eUT_Other;
	}
	else if (c < 0 || c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_' || c == '.' && type == eUT_Ltword
		|| type ==eUT_Ltword && c >= '0' && c <= '9')
	{
			type = eUT_Ltword;
	}
	else if (c >= '0' && c <= '9' || c == '.')
	{
		type = eUT_Number;
	}
	else if (symbolchars.find(c) != string::npos)
	{
		type = eUT_Symbol;
	}
	else if (c == '"')
	{
		type = eUT_String;
	}
	else
	{
		type = eUT_Other;
	}
}








//以下为CExpression操作
uint32 CExpression::Separate()
{
	static string symboldbchars = "== != <= >= && || ** ^^ := //";
	size_t i = 0, j = 0, k = 0;
	const string& s = m_sExprStr;
	uint32 l = (uint32)s.size();
	EUnitType type=eUT_Other, oldtype=eUT_Other;
	ClearVector(m_vecPreExpr);		//m_vecPreExpr.clear();
	CPreUnit* exprunit;
	while (i < l)
	{
		GetCharType(s[i], type);
		if(type != oldtype ||
			(oldtype == eUT_Symbol && symboldbchars.find(s.substr(i - 1, 2)) == (size_t)-1))
		{
				if(oldtype != eUT_Other)
				{
					exprunit = new CPreUnit;
					if(oldtype == eUT_String)
					{
						exprunit->str = s.substr(j + 1, i - j - 1).c_str();		//去掉两边的双引号
					}
					else
					{
						exprunit->str = s.substr(j, i - j);
						//auto_ptr<TCHAR> pBuffer(new TCHAR[exprunit->str.size() + 1]);
						//memcpy(pBuffer.get(), exprunit->str.c_str(), exprunit->str.size() + 1);
						//strlwr(pBuffer.get());
						//exprunit->str = pBuffer.get();
						tolower(exprunit->str);
					}
					exprunit->type = oldtype;
					m_vecPreExpr.push_back(exprunit);
					++k;
				}
				j = i;
		}
		oldtype = type;
		++i;
	}
	if(oldtype != eUT_Other)
	{
		exprunit = new CPreUnit;
		if(oldtype == eUT_String)
		{
			exprunit->str = s.substr(j + 1);		//去掉两边的双引号
		}
		else
		{
			exprunit->str = s.substr(j);
			//auto_ptr<TCHAR> pBuffer(new TCHAR[exprunit->str.size() + 1]);
			//memcpy(pBuffer.get(), exprunit->str.c_str(), exprunit->str.size() + 1);
			//strlwr(pBuffer.get());
			//exprunit->str = pBuffer.get();
			tolower(exprunit->str);
		}
		exprunit->type = type;
		m_vecPreExpr.push_back(exprunit);
	}
	//把这个置为空能节省对象所占空间，但不方便调试打印
	//m_sExprStr.clear();
	return (uint32)k;
}

void CExpression::Combine(stringstream& str)
{
	if(m_sExprStr.empty())
	{
		for(size_t i = 0, l = m_vecPreExpr.size(), k = m_vecExpr.size(); i < l; ++i)
		{
			if(i < k)
			{
				if (m_vecExpr[i].m_eTargetType > eCTT_Self)
				{
					str << g_sCalcTargetTypeStr[m_vecExpr[i].m_eTargetType].c_str() << ".";
				}
				if (m_vecExpr[i].m_eFunType > eVT_Arg)
				{
					str << g_sVarTypeStr[m_vecExpr[i].m_eFunType].c_str() << ".";
				}
			}
			str << m_vecPreExpr[i]->str.c_str();
		}
	}
	else
	{
		str << m_sExprStr;
	}
}

void CExpression::Convert()
{
	CExprUnit exprunit;
	VecExprUnit& expr = m_vecExpr;
	expr.clear();
	size_t l = m_vecPreExpr.size();
	for(size_t i = 0; i < l; ++i)
	{
		string& sPreUnit = m_vecPreExpr[i]->str;
		switch(m_vecPreExpr[i]->type)
		{
		case eUT_Number:
			exprunit.Set(atof(sPreUnit.c_str()), eUT_Number);
			expr.push_back(exprunit);
			break;
		case eUT_String:
			exprunit.Set(&sPreUnit, eUT_String);
			expr.push_back(exprunit);
			break;
		case eUT_Symbol:
			{
				MapStr2Optr::iterator itr = COptrTbl::m_mapOptrNo.find(sPreUnit);
				if(itr == COptrTbl::m_mapOptrNo.end())
				{
					stringstream ExpStr, ExprStr;
					ExpStr << "表达式含有错误的符号" << sPreUnit << "\n";
					Combine(ExprStr);
					CfgChk::GenExpInfo(ExpStr.str(), ExprStr.str());
					return;
				}
				exprunit.Set(uint32(itr->second), eUT_Symbol);
				exprunit.SetFunType(eVT_Operator);
				exprunit.SetTargetType(eCTT_Self);			//这句其实没用
				expr.push_back(exprunit);
			}
			break;
		case eUT_Ltword:
			m_pCfgCalc->m_bIsConstValue = false;
			//这里加入函数和变量映射
			{
				ECalcTargetType eCalcTargetType = GetTargetType(sPreUnit);
				if(eCalcTargetType == eCTT_Target)
				{
					m_pCfgCalc->m_bCalcNeedTarget = true;
				}
				exprunit.SetTargetType(eCalcTargetType);
			}
			if(sPreUnit == "x")
			{
				//自变量参数
				exprunit.SetFunType(eVT_Arg);
				exprunit.Set(&sPreUnit, eUT_Ltword);
			}
			else
			{
				exprunit.SetFunType(GetVarType(sPreUnit));
				if(exprunit.m_eFunType == eVT_Operator)
				{
					//函数
					MapStr2Optr::iterator itr = COptrTbl::m_mapOptrNo.find(sPreUnit);
					if(itr == COptrTbl::m_mapOptrNo.end())
					{
						stringstream ExpStr, ExprStr;
						ExpStr << "表达式函数名[" << sPreUnit << "]不存在";
						Combine(ExprStr);
						CfgChk::GenExpInfo(ExpStr.str(), ExprStr.str().c_str());
						return;
					}
					if(itr->second == eOI_Distance)
					{
						m_pCfgCalc->m_bCalcNeedTarget = true;
					}
					exprunit.Set(uint32(itr->second), eUT_Ltword);
				}
				else
				{
					//变量
					exprunit.SetType(eUT_Ltword);
					SetVarHandle(sPreUnit, exprunit);
				}
			}
			expr.push_back(exprunit);
			break;
		case eUT_Other:
			break;
		default:
			{
				stringstream ExpStr, ExprStr;
				ExpStr << "表达式含有错误的元素类型" << m_vecPreExpr[i]->type << "\n";
				Combine(ExprStr);
				CfgChk::GenExpInfo(ExpStr.str(), ExprStr.str());
			}
			return;
		}
	}
	m_sExprStr.clear();
	CExprUnit cUnit(uint32(eOI_Shell), eUT_Symbol);				//设置结束符
	expr.push_back(cUnit);
}

//string& CExpression::GetVarName(uint32 index)
//{
//	return m_vecPreExpr[index]->str;
//}

ECalcTargetType	CExpression::GetTargetType(string& varstr)
{
	size_t j = varstr.find('.');
	if(j != string::npos)
	{
		MapStr2CalcTargetType::iterator itr = COptrTbl::m_mapCalcTargetTypeNo.find(varstr.substr(0, j));
		if(itr == COptrTbl::m_mapCalcTargetTypeNo.end())
		{
			return eCTT_Self;
		}
		else
		{
			varstr = varstr.substr(j + 1);
			return itr->second;
		}
	}
	else
	{
		//默认目标为自身
		return eCTT_Self;
	}
}

EFunType CExpression::GetVarType(string& varstr)
{
	//string& varstr = m_vecPreExpr[index]->str;
	MapStr2Optr::iterator itrOptr = COptrTbl::m_mapOptrNo.find(varstr);
	if(itrOptr != COptrTbl::m_mapOptrNo.end())
	{
		return eVT_Operator;
	}

	size_t j = varstr.find('.');
	if(j != string::npos)
	{
		MapStr2VarType::iterator itr = COptrTbl::m_mapVarTypeNo.find(varstr.substr(0, j));
		if(itr == COptrTbl::m_mapVarTypeNo.end())
		{
			stringstream ExpStr, ExprStr;
			ExpStr << "表达式含有错误的变量域[" << varstr.substr(0, j) << "]\n";
			Combine(ExprStr);
			CfgChk::GenExpInfo(ExpStr.str(), ExprStr.str());
			return eVT_Unknow;
		}
		if(j + 1 == varstr.size())
		{
			stringstream ExpStr, ExprStr;
			ExpStr << "表达式的变量名不完整：[" << varstr << "]\n";
			Combine(ExprStr);
			CfgChk::GenExpInfo(ExpStr.str(), ExprStr.str());
			return eVT_Unknow;
		}
		varstr = varstr.substr(j + 1);
		return itr->second;
	}
	else
	{
		stringstream ExpStr, ExprStr;
		ExpStr << "表达式的变量域为空[" << varstr << "]\n";
		Combine(ExprStr);
		CfgChk::GenExpInfo(ExpStr.str(), ExprStr.str());
		return eVT_Unknow;
	}
}

bool CExpression::SetVarHandle(string& sName, CExprUnit& exprunit)
{
	switch(exprunit.m_eFunType)
	{
	case eVT_Talent:
	case eVT_MasterTalent:
		exprunit.Set(&sName);
		if(CTalentHolder::ExistName(sName)) return true;
		break;
	case eVT_Attr:
	case eVT_MasterAttr:
	case eVT_ServantAttr:
		{
			pair<bool, uint32> pr = CAttrVarMap::GetVarId(sName);
			exprunit.Set(pr.second);
			if(pr.first) return true;
		}
		break;
	case eVT_Temp:
	case eVT_MasterTemp:
		CTempVarMgr::SetVarPtr(&exprunit.m_Value, sName);
		if(CTempVarMgr::ExistVar(sName)) return true;
		break;
	case eVT_Cfg:
		{
			exprunit.Set(CCfgGlobal::GetVarId(sName));
			if(exprunit.m_Value.lng) return true;
		}
		break;
	default:
		{
			stringstream ExpStr, ExprStr;
			ExpStr << "表达式含有错误的变量名[" << sName << "]";
			Combine(ExprStr);
			CfgChk::GenExpInfo(ExpStr.str(), ExprStr.str());
			return false;
		}
	} 
	stringstream ExpStr, ExprStr;
	ExpStr << "表达式" << g_sVarTypeStr[exprunit.m_eFunType] << "变量名[" << sName << "]不存在";
	Combine(ExprStr);
	CfgChk::GenExpInfo(ExpStr.str(), ExprStr.str());
	return false;
}





pair<bool, double> CExpression::GetArgValue(uint32 index)
{
	return make_pair(true, (*m_pVecArgs)[index]);
}

class CFighterNull;

template<>
pair<bool, double> CExpression::GetTalentValue<CFighterNull>(const string& varName, const CFighterNull* pFighter)
{
	return make_pair(false, 0.0);
}

template<>
pair<bool, double> CExpression::GetMasterTalentValue<CFighterNull>(const string& varName,const CFighterNull* pFighter)
{
	return make_pair(false, 0.0);
}

template<>
pair<bool, double> CExpression::GetAttrValue<CFighterNull>(uint32 uVarID,const CFighterNull* pFighter)
{
	return make_pair(false, 0.0);
}

template<>
pair<bool, double> CExpression::GetMasterAttrValue<CFighterNull>(uint32 uVarID,const CFighterNull* pFighter)
{
	return make_pair(false, 0.0);
}

template<>
pair<bool, double> CExpression::GetServantAttrValue<CFighterNull>(uint32 uVarID,const CFighterNull* pFighter)
{
	return make_pair(false, 0.0);
}


class CSkillInstNull;

template<>
pair<bool, double> CExpression::GetTempValue<CFighterNull>(UUnitValue pVarStr,const CFighterNull* pFighter)
{
	return make_pair(false, 0.0);
}

template<>
pair<bool, double> CExpression::GetMasterTempValue<CFighterNull>(UUnitValue pVarStr,const CFighterNull* pFighter)
{
	return make_pair(false, 0.0);
}

template<>
const CFighterNull* CExpression::GetCalcFighter<CFighterNull>(const CFighterNull* pFighter[], ECalcTargetType eTargetType)
{
	if(eTargetType == eCTT_Master)
	{
		return NULL;
	}
	else
	{
		return pFighter[eTargetType];
	}
}






COpndUnit CExpression::Operate2Eyes(const COpndUnit& opnd_a, uint32 theta, const COpndUnit& opnd_b)
{
	switch(theta)
	{
	case eOI_Add:
		Ast(opnd_a.type == eUT_Number && opnd_b.type == eUT_Number);
		return COpndUnit(opnd_a.value.flt + opnd_b.value.flt, eUT_Number);
	case eOI_Sub:
		Ast(opnd_a.type == eUT_Number && opnd_b.type == eUT_Number);
		return COpndUnit(opnd_a.value.flt - opnd_b.value.flt, eUT_Number);
	case eOI_Mul:
		Ast(opnd_a.type == eUT_Number && opnd_b.type == eUT_Number);
		return COpndUnit(opnd_a.value.flt * opnd_b.value.flt, eUT_Number);
	case eOI_Div:
		{
			Ast(opnd_a.type == eUT_Number && opnd_b.type == eUT_Number);
			if(opnd_b.value.flt == 0.0)
			{
				stringstream ExpStr;
				Combine(ExpStr);
				ExpStr << "表达式计算错误：除数为0\n";
				GenErr(ExpStr.str());
				//return COpndUnit(0.0, eUT_Number);;
			}
			else
			{
				return COpndUnit(opnd_a.value.flt / opnd_b.value.flt, eUT_Number);
			}
		}
	case eOI_DivSptZero:
		{
			Ast(opnd_a.type == eUT_Number && opnd_b.type == eUT_Number);
			if(opnd_b.value.flt == 0.0)
			{
				return COpndUnit(0.0, eUT_Number);;
			}
			else
			{
				return COpndUnit(opnd_a.value.flt / opnd_b.value.flt, eUT_Number);
			}
		}

	case eOI_Pow:
		Ast(opnd_a.type == eUT_Number && opnd_b.type == eUT_Number);
		return COpndUnit(pow(opnd_a.value.flt, opnd_b.value.flt), eUT_Number);
	case eOI_EQ:
		Ast(opnd_a.type == eUT_Number && opnd_b.type == eUT_Number);
		return COpndUnit(opnd_a.value.flt == opnd_b.value.flt, eUT_Number);
	case eOI_UE:
		Ast(opnd_a.type == eUT_Number && opnd_b.type == eUT_Number);
		return COpndUnit(opnd_a.value.flt != opnd_b.value.flt, eUT_Number);
	case eOI_GT:
		Ast(opnd_a.type == eUT_Number && opnd_b.type == eUT_Number);
		return COpndUnit(opnd_a.value.flt > opnd_b.value.flt, eUT_Number);
	case eOI_GE:
		Ast(opnd_a.type == eUT_Number && opnd_b.type == eUT_Number);
		return COpndUnit(opnd_a.value.flt >= opnd_b.value.flt, eUT_Number);
	case eOI_LT:
		Ast(opnd_a.type == eUT_Number && opnd_b.type == eUT_Number);
		return COpndUnit(opnd_a.value.flt < opnd_b.value.flt, eUT_Number);
	case eOI_LE:
		Ast(opnd_a.type == eUT_Number && opnd_b.type == eUT_Number);
		return COpndUnit(opnd_a.value.flt <= opnd_b.value.flt, eUT_Number);
	case eOI_And:
		Ast(opnd_a.type == eUT_Number && opnd_b.type == eUT_Number);
		return COpndUnit(opnd_a.value.flt && opnd_b.value.flt, eUT_Number);
	case eOI_Or:
		Ast(opnd_a.type == eUT_Number && opnd_b.type == eUT_Number);
		return COpndUnit(opnd_a.value.flt || opnd_b.value.flt, eUT_Number);
	case eOI_Comma:
		{
			if (opnd_a.type == eUT_Array)
			{
				opnd_a.value.pArr->push_back(opnd_b);
				return COpndUnit(opnd_a.value.pArr, eUT_Array);
			}
			else
			{
				VecOpnd* pVecOpnd = new VecOpnd;
				pVecOpnd->push_back(opnd_a);
				pVecOpnd->push_back(opnd_b);
				m_vecOpndArray.push_back(pVecOpnd);
				return COpndUnit(pVecOpnd, eUT_Array);
			}
		}
	default:
		{
			stringstream ExpStr;
			Combine(ExpStr);
			ExpStr << "表达式含有不支持的双目运算符：" << g_sOptrStr[theta].c_str() << "\n";
			GenErr(ExpStr.str());
			return COpndUnit();
		}
	}
}




//以下为个人测试用

void CExpression::Test()
{
	string s;
	s="1+1";
	s="73*2+-1.0/3";
	s="1";
	s="7/2.0";
	s="7.5/3.5";
	s="(e+(2-3.0)*4)/((5+6)*7-8)/9";
	s="破甲冷却*0.1";

	
	
	CTalentHolder talentHolder;
	talentHolder.InsertTalent("破甲冷却", 2);
	Input(s, NULL);
	//double f = Eval(&talentHolder, (CSkillInstNull*)NULL);
}
		










//以下为CCfgCalc操作：
CCfgCalc::CCfgCalc()
: m_eExprType(eET_Empty),
  m_eStrType(eST_Null),
  m_bCalcNeedTarget(false),
  m_bIsConstValue(true)
{
	SetStringSplitter();
}

CCfgCalc::CCfgCalc(int32 intValue)
: m_eExprType(eET_Single),
  m_eStrType(eST_Expression),
  m_bCalcNeedTarget(false),
  m_bIsConstValue(true)
{
	static stringstream str;
	str.str("");
	str << intValue;
	m_sString = str.str();
	m_vecExprs.push_back(new CExpression(double(intValue), this));
}

CCfgCalc::CCfgCalc(const CCfgCalc& srcCfgCalc)
: m_eExprType(srcCfgCalc.m_eExprType),
  m_eStrType(srcCfgCalc.m_eStrType),
  m_sString(srcCfgCalc.m_sString),
  m_cStringSplitter(srcCfgCalc.m_cStringSplitter),
  m_bCalcNeedTarget(srcCfgCalc.m_bCalcNeedTarget),
  m_bIsConstValue(srcCfgCalc.m_bIsConstValue)
{
	//复制STL
	m_vecArgs = srcCfgCalc.m_vecArgs;
	size_t l =  srcCfgCalc.m_vecExprs.size();
	m_vecExprs.assign(l, NULL);
	for(size_t i = 0; i < l; ++i)
	{
		m_vecExprs[i] = new CExpression(*srcCfgCalc.m_vecExprs[i], this);
	}
	l = srcCfgCalc.m_vecString.size();
	m_vecString.assign(l, NULL);
	for(size_t i = 0; i < l; ++i)
	{
		m_vecString[i] = new string(*srcCfgCalc.m_vecString[i]);
	}
}

CCfgCalc::~CCfgCalc()
{
	ClearVector(m_vecExprs);
	ClearVector(m_vecString);
}

CCfgCalc::CCfgCalc(double dblValue)
: m_eExprType(eET_Single),
m_eStrType(eST_Expression),
m_bCalcNeedTarget(false),
m_bIsConstValue(true)
{
	static stringstream str;
	str.str("");
	str << dblValue;
	m_sString = str.str();
	m_vecExprs.push_back(new CExpression(dblValue, this));
}

CCfgCalc::CCfgCalc(const string& str)
: m_eExprType(eET_Single),
m_eStrType(eST_Null),
m_bCalcNeedTarget(false),
m_bIsConstValue(true)
{
	SetStringSplitter();
	Exuviate(str);
}

void CCfgCalc::InputString(const string& str)
{
	ClearVector(m_vecExprs);		//m_vecExprs.clear();
	ClearVector(m_vecString);
	m_vecArgs.clear();
	Exuviate(str);
}

void CCfgCalc::Exuviate(const string& str)
{
	if(str[0] == '"' && str[str.size() - 1] == '"')
	{
		m_sString = str.substr(1, str.size() - 2);
		size_t pos=0;
		while( (pos = m_sString.find("\"\"", pos)) != string::npos){
			m_sString.erase(pos, 1);
			++ pos;
		}
	}
	else
	{
		m_sString = str;
	}
}

void CCfgCalc::Parse() const
{
	string& str = m_sString;

	if(str == "")
	{
		m_eExprType = eET_Empty;
		strstream ExpStr;
		ExpStr << "表达式不能为空";
		CfgChk::GenExpInfo(ExpStr.str());
		return;
	}

	bool bInString = false;
	int32 iInBracket = 0;
	bool bArgMode = false;
	vector<string> vecStr;
	size_t pos = 0;
	m_eExprType = eET_Single;
	for(size_t i = 0, l = str.size(); i < l; ++i)
	{
		switch(str[i])
		{
		case '"':
			bInString = !bInString;
			break;
		case '(':
		case '[':
			if(!bInString)
			{
				++iInBracket;
				break;
			}
		case ')':
		case ']':
			if(!bInString)
			{
				--iInBracket;
				if(iInBracket < 0)
				{
					strstream ExpStr;
					ExpStr << "表达式括号配对错误：右括号比左括号多";
					CfgChk::GenExpInfo(ExpStr.str(), str);
					return;
				}
			}
			break;
		case '|':
			if(!bInString)
			{
				if(i < l - 1 && str[i + 1] == '|')
				{
					++i;
					continue;
				}
				if(bArgMode == true)
				{
					strstream ExpStr;
					ExpStr << "表达式不该有两个'|'";
					CfgChk::GenExpInfo(ExpStr.str(), str);
					return;
				}
				vecStr.push_back(str.substr(pos, i - pos));
				pos = i + 1;
				bArgMode = true;
				m_eExprType = eET_Args;
				if(!vecStr.back().empty())
				{
					for(size_t j = 0, m = vecStr.size(); j < m; ++j)
					{
						m_vecArgs.push_back(atof(vecStr[j].c_str()));
					}
				}
				vecStr.clear();
			}
			break;
		case ',':
			if(!bInString && iInBracket == 0)
			{
				if(bArgMode)
				{
					stringstream ExpStr;
					ExpStr << "参数模式表达式的算式部分不能有逗号\n";
					CfgChk::GenExpInfo(ExpStr.str(), m_sString);
					return;
				}
				m_eExprType = eET_Multiple;
				vecStr.push_back(str.substr(pos, i - pos));
				pos = i + 1;
			}
		}
	}
	if(bInString)
	{
		strstream ExpStr;
		ExpStr << "表达式双引号不配对";
		CfgChk::GenExpInfo(ExpStr.str(), str);
		return;
	}
	if(iInBracket>0)
	{
		strstream ExpStr;
		ExpStr << "表达式括号配对错误：左括号比右括号多";
		CfgChk::GenExpInfo(ExpStr.str(), str);
		return;
	}
	vecStr.push_back(str.substr(pos));
	for(size_t j = 0, m = vecStr.size(); j < m; ++j)
	{
		m_vecExprs.push_back(new CExpression(vecStr[j], /*&m_vecArgs,*/ this));
	}
	vecStr.clear();


	//size_t iStart = 0, iEnd, iSearch;
	//iEnd = str.find('|');
	//if(iEnd!= string::npos)
	//{
	//	m_eExprType = eET_Args;
	//	m_vecArgs.clear();
	//	while(1)
	//	{
	//		iSearch = str.find(',', iStart);
	//		if(iSearch != string::npos)
	//		{
	//			if(iSearch > iEnd)
	//			{
	//				stringstream ExpStr;
	//				ExpStr << "参数模式表达式" << m_sString << "的表达式部分不能有逗号\n";
	//				GenErr(ExpStr.str());
	//				return;
	//			}
	//			m_vecArgs.push_back(atof(str.substr(iStart, iSearch - iStart).c_str()));
	//			iStart = iSearch + 1;
	//			m_eExprType = eET_Multiple;
	//		}
	//		else
	//		{
	//			m_vecArgs.push_back(atof(str.substr(iStart, iEnd - iStart).c_str()));
	//			break;
	//		}
	//	}
	//	CExpression tempExpr(str.substr(iEnd + 1), &m_vecArgs);
	//	m_vecExprs.push_back(tempExpr);
	//}
	//else if(str.find(',') != string::npos)
	//{
	//	m_eExprType = eET_Multiple;
	//	while(1)
	//	{
	//		iEnd = str.find(',', iStart);
	//		if(iEnd == string::npos)
	//		{
	//			CExpression	tempExpr(str.substr(iStart));
	//			m_vecExprs.push_back(tempExpr);
	//			break;
	//		}
	//		else
	//		{
	//			CExpression	tempExpr(str.substr(iStart, iEnd - iStart));
	//			m_vecExprs.push_back(tempExpr);
	//			iStart = iEnd + 1;
	//		}
	//	}
	//}
	//else if(str == "")
	//{
	//	m_eExprType = eET_Empty;
	//}
	//else
	//{
	//	m_eExprType = eET_Single;
	//	CExpression	tempExpr(str);
	//	m_vecExprs.push_back(tempExpr);
	//}
	//str.clear();
}

bool CCfgCalc::IsSingleNumber() const
{
	if(m_eExprType != eET_Single)
		return false;
	CExpression::VecExprUnit& vecExprUnit = m_vecExprs[0]->m_vecExpr;
	return (vecExprUnit.size() == 2 && vecExprUnit[0].m_eType == eUT_Number) || 
		(vecExprUnit.size() == 3 && vecExprUnit[0].m_eType == eUT_Symbol && 
		 vecExprUnit[0].m_Value.lng == eOI_Sub && 
		 vecExprUnit[1].m_eType == eUT_Number);
}



void CCfgCalc::GenPermitChangeExprTypeExp() const
{
	stringstream ExpStr;
	ExpStr << "CfgCalc对象类型已置为" << m_eStrType << "，不能改变类型\n";
	GenErr(ExpStr.str());
}



const string& CCfgCalc::GetString() const
{
	switch(m_eStrType)
	{
	case eST_Null:
		m_eStrType = eST_String;
	case eST_String:
		return m_sString;
	default:
		GenPermitChangeExprTypeExp();
		return m_sString;
	}
}

void CCfgCalc::ParseString() const
{
	size_t iStart = 0, iEnd;
	string& str = m_sString;
	for(;;)
	{
		iEnd = str.find(m_cStringSplitter, iStart);
		if(iEnd == string::npos)
		{
			m_vecString.push_back(new string(str.substr(iStart)));
			break;
		}
		else
		{
			m_vecString.push_back(new string(str.substr(iStart, iEnd - iStart)));
			iStart = iEnd + 1;
		}
	}
	//str.clear();
}

const string& CCfgCalc::GetString(uint32 index) const
{
	switch(m_eStrType)
	{
	case eST_Null:
		m_eStrType = eST_MutliString;
		ParseString();
	case eST_MutliString:
		if(index >= m_vecString.size())
		{
			stringstream ExpStr;
			ExpStr << "GetString下标" << index << "超出字符串数组的数量" << m_vecString.size() << "\n";
			CfgChk::GenExpInfo(ExpStr.str(), GetTestString());
			return m_sString;
		}
		return *m_vecString[index];
	default:
		GenPermitChangeExprTypeExp();
		return m_sString;
	}
}

void CCfgCalc::SetTypeExpression() const
{
	if(m_eStrType == eST_Expression) return;
	if(m_eStrType != eST_Null)
	{
		GenPermitChangeExprTypeExp();
		return;
	}
	m_eStrType = eST_Expression;
	Parse();
}

void CCfgCalc::SetTypeSingleString() const
{
	if(m_eStrType == eST_String) return;
	if(m_eStrType != eST_Null)
	{
		GenPermitChangeExprTypeExp();
		return;
	}
	m_eStrType = eST_String;
	Parse();
}

void CCfgCalc::SetTypeMultiString() const
{
	if(m_eStrType == eST_MutliString) return;
	if(m_eStrType != eST_Null)
	{
		GenPermitChangeExprTypeExp();
		return;
	}
	m_eStrType = eST_MutliString;
	ParseString();
}


const uint32 CCfgCalc::GetValueCount() const
{
	switch(m_eStrType)
	{
	case eST_Null:
		m_eStrType = eST_Expression;
		Parse();
	case eST_Expression:
		if(m_eExprType == eET_Empty)
		{
			stringstream ExpStr;
			ExpStr << "空表格不能用GetValue获取值";
			CfgChk::GenExpInfo(ExpStr.str(), GetTestString());
			//return 0;
		}
		if(m_eExprType == eET_Args)
		{
			return (uint32)m_vecArgs.size();
		}
		else
		{
			return (uint32)m_vecExprs.size();
		}
	default:
		GenPermitChangeExprTypeExp();
		return 0;
	}
}

const uint32 CCfgCalc::GetStringCount() const
{
	switch(m_eStrType)
	{
	case eST_Null:
		m_eStrType = eST_MutliString;
		ParseString();
	case eST_MutliString:
		return (uint32)m_vecString.size();
	default:
		GenPermitChangeExprTypeExp();
		return 0;
	}
}





//以下为个人测试用
void CCfgCalc::Test()
{
	string s = "空地|1+破甲冷却";
	s = "(3.14/强击暴击)";
	s = "(强击光环+(快速三连击+精准射击)*0.3,7,3.14/强击暴击)";
	InputString(s);

	CTalentHolder talentHolder;
	//talentHolder.Insert("破甲冷却", 1);
	//talentHolder.Insert("强击光环", 2);
	//talentHolder.Insert("精准射击", 3);
	//talentHolder.Insert("闪身强化", 4);
	//talentHolder.Insert("乱射", 5);
	//talentHolder.Insert("快速三连击", 6);
	//talentHolder.Insert("强击暴击", 7);
	//talentHolder.Insert("暴击上限", 8);
	//talentHolder.Insert("扩展弹簧网", 9);
	//talentHolder.Insert("扎根", 10);
	//talentHolder.Insert("近身伤害", 1);
	//talentHolder.Insert("乱射范围", 2);
	//talentHolder.Insert("召唤树人", 3);
	//talentHolder.Insert("加速光环", 4);
	//talentHolder.Insert("破甲加强", 5);
	//talentHolder.Insert("强击破甲", 6);
	//double f = GetDblValue(&talentHolder, 2);
	//int32 l = GetIntValue(&talentHolder, 0);
}



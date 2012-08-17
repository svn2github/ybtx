#pragma once
#include "stdafx.h"
#include "CCfgCalc.h"
#include "CTalent.h"
#include "CCfgCalcFunction.h"
#include "ExpHelper.h"
#include <iostream>
#include <cmath>
#include <iostream>
#include "BaseHelper.h"
//#include "TSqrAllocator.inl"

//template <typename FighterType>
//pair<bool, double> CExpression::GetAttrValue(const string& varName, FighterType* pFighter)
//{
//	pair<bool, uint32> pr = CAttrVarMap::GetVarId(varName);
//	if(pr.first)
//	{
//		return make_pair(true, double(pFighter->CppGetPropertyValue(pr.second)));
//	}
//	else
//	{
//		return make_pair(false, 0.0);
//	}
//}



template<typename FighterType>
double CExpression::GetVarValue(UUnitValue varHandle, EFunType& vartype, const FighterType* pFighter[],
	ECalcTargetType eTargetType, uint32 index)
{
	//这里取变量的值
	switch(vartype)
	{
	case eVT_Arg:
		if(m_pVecArgs == NULL)
		{
			stringstream ExpStr;
			Combine(ExpStr);
			ExpStr << "指向VecArgs的指针为空，含有代入变量X的表达式缺少代入参数\n";
			cout << ExpStr.str();
			//GenErr(ExpStr.str());
			return 0.0;
		}
		else if(index >= m_pVecArgs->size())
		{
			if(m_pVecArgs->size() == 0)
			{
				return double(index+1);
			}
			else
			{
				stringstream ExpStr;
				Combine(ExpStr);
				ExpStr << "下标" << index << "超出变量参数VecArgs的数量" << m_pVecArgs->size() << "\n";
				cout << ExpStr.str();

				//GenErr(ExpStr.str());
				return 0.0;
			}
		}
		return GetArgValue(index).second;
	case eVT_Talent:
		{
			const FighterType* pCalcFighter = GetCalcFighter(pFighter, eTargetType);
			if(pCalcFighter == NULL)
			{
				stringstream ExpStr;
				Combine(ExpStr);
				ExpStr << "指向Talent pFighter的指针为空\n";
				cout << ExpStr.str();

				//GenErr(ExpStr.str());
				return 0.0;
			}
			return GetTalentValue(*varHandle.pStr, pCalcFighter).second;

		}
	case eVT_MasterTalent:
		{
			const FighterType* pCalcFighter = GetCalcFighter(pFighter, eTargetType);
			if(pCalcFighter == NULL)
			{
				stringstream ExpStr;
				Combine(ExpStr);
				ExpStr << "指向MasterTalent pFighter的指针为空\n";
				cout << ExpStr.str();

				//GenErr(ExpStr.str());
				return 0.0;
			}
			return GetMasterTalentValue(*varHandle.pStr, pCalcFighter).second;
		}
	case eVT_Attr:
		{
			const FighterType* pCalcFighter = GetCalcFighter(pFighter, eTargetType);
			if(pCalcFighter == NULL)
			{
				stringstream ExpStr;
				Combine(ExpStr);
				ExpStr << "指向Attr pFighter的指针为空\n";
				cout << ExpStr.str();

				//GenErr(ExpStr.str());
				return 0.0;
			}
			return GetAttrValue(varHandle.lng, pCalcFighter).second;
		}
	case eVT_MasterAttr:
		{
			const FighterType* pCalcFighter = GetCalcFighter(pFighter, eTargetType);
			if(pCalcFighter == NULL)
			{
				stringstream ExpStr;
				Combine(ExpStr);
				ExpStr << "指向MasterAttr pFighter的指针为空\n";
				cout << ExpStr.str();

				//GenErr(ExpStr.str());
				return 0.0;
			}
			return GetMasterAttrValue(varHandle.lng, pCalcFighter).second;
		}
	case eVT_ServantAttr:
		{
			const FighterType* pCalcFighter = GetCalcFighter(pFighter, eTargetType);
			if(pCalcFighter == NULL)
			{
				stringstream ExpStr;
				Combine(ExpStr);
				ExpStr << "指向ServantAttr pFighter的指针为空\n";
				cout << ExpStr.str();

				//GenErr(ExpStr.str());
				return 0.0;
			}
			return GetServantAttrValue(varHandle.lng, pCalcFighter).second;
		}
	case eVT_Temp:
		{
			const FighterType* pCalcFighter = GetCalcFighter(pFighter, eTargetType);
			if(pCalcFighter == NULL)
			{
				stringstream ExpStr;
				Combine(ExpStr);
				ExpStr << "指向Temp pFighter的指针为空\n";
				cout << ExpStr.str();

				//GenErr(ExpStr.str());
				return 0.0;
			}
			return GetTempValue(varHandle, pCalcFighter).second;
		}
	case eVT_MasterTemp:
		{
			const FighterType* pCalcFighter = GetCalcFighter(pFighter, eTargetType);
			if(pCalcFighter == NULL)
			{
				stringstream ExpStr;
				Combine(ExpStr);
				ExpStr << "指向MasterTemp pFighter的指针为空\n";
				cout << ExpStr.str();

				//GenErr(ExpStr.str());
				return 0.0;
			}
			return GetMasterTempValue(varHandle, pCalcFighter).second;
		}
	case eVT_Cfg:
		{
			pair<bool, CCfgGlobal*> resultCfgGlobal = CCfgGlobal::GetCfgValue(varHandle.lng);
			if(resultCfgGlobal.first) return resultCfgGlobal.second->Get(pFighter[eCTT_Self], pFighter[eCTT_Target], index);		//全局变量跟eTargetType没关系
			else return 0.0;
		}
	case eVT_Unknow:
		{
			//pair<bool, CCfgCalc*> resultCfgCalc = CCfgGlobal::GetCfgValue(varName);
			//if(resultCfgCalc.first) return resultCfgCalc.second->GetDblValue(pFighter, index);
			//pair<bool, double> result;
			//result = GetTalentValue(varName, pFighter->GetTalentHolder());
			//if(result.first) return result.second;
			//result = GetAttrValue(varName, pFighter);
			//if(result.first) return result.second;
			//result = GetTempValue(varName, pFighter);
			//if(result.first) return result.second;
			stringstream ExpStr;
			Combine(ExpStr);
			ExpStr << "变量名错误：" << *varHandle.pStr;
			GenErr(ExpStr.str());
			return 0.0;
		}
	default:
		{
			stringstream ExpStr;
			Combine(ExpStr);
			ExpStr << "GetVarValue函数内变量类型错误：" << vartype << "\n";
			GenErr(ExpStr.str());
			return 0.0;
		}
	}
}

template<typename FighterType>
double CExpression::Operate0Eyes(uint32 theta, const FighterType* pFighter[], ECalcTargetType eTargetType)
{
	switch(theta)
	{
	case eOI_Distance:
		return (double)CCfgGlobal::Distance(pFighter[eCTT_Self], pFighter[eCTT_Target]);
	case eOI_TargetIsNPC:
		return (double)CCfgGlobal::TargetIsNPC(pFighter[eCTT_Target]);
	case eOI_CharaLevel:
		{
			const FighterType* pFighterGet = GetCalcFighter(pFighter, eTargetType);
			if(pFighterGet)
			{
				//return 0.0;
				return pFighterGet->CppGetLevel();
			}
			else
			{
				return 0.0;
			}
		}
	default:
		{
			stringstream ExpStr;
			Combine(ExpStr);
			ExpStr << "表达式含有不支持的零目运算符：" << g_sOptrStr[theta].c_str() << "\n";
			GenErr(ExpStr.str());
			return 0.0;
		}
	}
}

template<typename FighterType>
double CExpression::Operate1Eyes(uint32 theta, const COpndUnit& opnd_b, const  FighterType* pFighter[],
	ECalcTargetType eTargetType, uint32 index)
{
	switch(theta)
	{
	case eOI_Add:
		Ast(opnd_b.type == eUT_Number);
		return opnd_b.value.flt;
	case eOI_Sub:
		Ast(opnd_b.type == eUT_Number);
		return -opnd_b.value.flt;
	case eOI_Not:
		Ast(opnd_b.type == eUT_Number);
		return !opnd_b.value.flt;
	//case eOI_VarCfg:
	//	return CCfgGlobal::GetCfgValue(*opnd_b.value.pStr).second->Get(pFighter[eCTT_Self], pFighter[eCTT_Target], index);
	//case eOI_VarGift:
	//	Ast(opnd_b.type == eUT_String && GetCalcFighter(pFighter, eTargetType) != NULL);
	//	return GetTalentValue(*opnd_b.value.pStr, GetCalcFighter(pFighter, eTargetType)).second;
	//case eOI_VarMasterGift:
	//	Ast(opnd_b.type == eUT_String && GetCalcFighter(pFighter, eTargetType) != NULL);
	//	return GetMasterTalentValue(*opnd_b.value.pStr, GetCalcFighter(pFighter, eTargetType)).second;
	//case eOI_VarTemp:
	//	Ast(opnd_b.type == eUT_String && GetCalcFighter(pFighter, eTargetType) != NULL);
	//	return GetTempValue(*opnd_b.value.pStr, GetCalcFighter(pFighter, eTargetType)).second;
	//case eOI_VarMasterTemp:
	//	Ast(opnd_b.type == eUT_String && GetCalcFighter(pFighter, eTargetType) != NULL);
	//	return GetMasterTempValue(*opnd_b.value.pStr, GetCalcFighter(pFighter, eTargetType)).second;
	//case eOI_VarAttr:
	//	Ast(opnd_b.type == eUT_String && GetCalcFighter(pFighter, eTargetType) != NULL);
	//	return GetAttrValue(opnd_b.value.lng, GetCalcFighter(pFighter, eTargetType)).second;
	//case eOI_VarMasterAttr:
	//	Ast(opnd_b.type == eUT_String && GetCalcFighter(pFighter, eTargetType) != NULL);
	//	return GetMasterAttrValue(opnd_b.value.lng, GetCalcFighter(pFighter, eTargetType)).second;
	//case eOI_VarServantAttr:
	//	Ast(opnd_b.type == eUT_String && GetCalcFighter(pFighter, eTargetType) != NULL);
		return GetServantAttrValue(opnd_b.value.lng, GetCalcFighter(pFighter, eTargetType)).second;
	case eOI_Rand:
		Ast(opnd_b.type == eUT_Array);
		return CCfgGlobal::Rand((*opnd_b.value.pArr)[0].value.flt, (*opnd_b.value.pArr)[1].value.flt);
	case eOI_Randf:
		Ast(opnd_b.type == eUT_Array);
		return CCfgGlobal::Randf((*opnd_b.value.pArr)[0].value.flt, (*opnd_b.value.pArr)[1].value.flt);
	case eOI_IfElse:
		Ast(opnd_b.type == eUT_Array && opnd_b.value.pArr->size() == 3);
		return CCfgGlobal::IfElse((*opnd_b.value.pArr)[0].value.flt, (*opnd_b.value.pArr)[1].value.flt, (*opnd_b.value.pArr)[2].value.flt);
	case eOI_ExistState:
		Ast(opnd_b.type == eUT_String);
		return (double)CCfgGlobal::ExistState(*opnd_b.value.pStr, GetCalcFighter(pFighter, eTargetType));
	case eOI_ExistMagicState:
		Ast(opnd_b.type == eUT_String);
		return (double)CCfgGlobal::ExistMagicState(*opnd_b.value.pStr, GetCalcFighter(pFighter, eTargetType));
	case eOI_ExistTriggerState:
		Ast(opnd_b.type == eUT_String);
		return (double)CCfgGlobal::ExistTriggerState(*opnd_b.value.pStr, GetCalcFighter(pFighter, eTargetType));
	case eOI_ExistDamageChangeState:
		Ast(opnd_b.type == eUT_String);
		return (double)CCfgGlobal::ExistDamageChangeState(*opnd_b.value.pStr, GetCalcFighter(pFighter, eTargetType));
	case eOI_ExistSpecialState:
		Ast(opnd_b.type == eUT_String);
		return (double)CCfgGlobal::ExistSpecialState(*opnd_b.value.pStr, GetCalcFighter(pFighter, eTargetType));
	case eOI_StateCount:
		Ast(opnd_b.type == eUT_String);
		return (double)CCfgGlobal::StateCount(*opnd_b.value.pStr, GetCalcFighter(pFighter, eTargetType));
	case eOI_CurRlserStateCount:
		Ast(opnd_b.type == eUT_String);
		return (double)CCfgGlobal::CurRlserStateCount(*opnd_b.value.pStr, pFighter[eCTT_Self], GetCalcFighter(pFighter, eTargetType));
	case eOI_TriggerCount:
		Ast(opnd_b.type == eUT_String);
		return (double)CCfgGlobal::TriggerCount(*opnd_b.value.pStr, pFighter[eTargetType]);
	case eOI_StateLeftTime:
		Ast(opnd_b.type == eUT_String);
		//这里注意，如果StateLeftTime要获取别人安装的可叠加魔法状态，则第二个参数要换成安装者，且要考虑安装者在表达式里的表示方式
		return (double)CCfgGlobal::StateLeftTime(*opnd_b.value.pStr, pFighter[eCTT_Self], GetCalcFighter(pFighter, eTargetType));
	case eOI_Floor:
		Ast(opnd_b.type == eUT_Number);
		return (double)floor(opnd_b.value.flt);
	case eOI_Ceil:
		Ast(opnd_b.type == eUT_Number);
		return (double)ceil(opnd_b.value.flt);
	case eOI_Distance:
		Ast(opnd_b.type == eUT_Number);
		return (double)(CCfgGlobal::Distance(pFighter[eCTT_Self], pFighter[eCTT_Target]) <= opnd_b.value.flt);
	case eOI_IsActiveSkill:
		Ast(opnd_b.type == eUT_String);
		return (double)CCfgGlobal::IsActiveSkill(*opnd_b.value.pStr, GetCalcFighter(pFighter, eTargetType));
	default:
		{
			stringstream ExpStr;
			Combine(ExpStr);
			ExpStr << "表达式含有不支持的单目运算符：" << g_sOptrStr[theta].c_str() << "\n";
			GenErr(ExpStr.str());
			return 0.0;
		}
	}
}

template <typename FighterType>
double CExpression::Eval(const FighterType* pFighter[], uint32 index)
{
	if(m_vecExpr.size() == 2)
	{
		switch (m_vecExpr[0].m_eType)
		{
		case eUT_Number:
			return m_vecExpr[0].m_Value.flt;
		case eUT_Ltword:
			//m_pTalentHolder = pTalentHolder;
			return GetVarValue(m_vecExpr[0].m_Value, m_vecExpr[0].m_eFunType, pFighter, 
				m_vecExpr[0].m_eTargetType, index);
		default:
			{
				stringstream ExpStr;
				Combine(ExpStr);
				ExpStr << "表达式元素类型错误";
				GenErr(ExpStr.str());
				return 0.0;
			}
		}
	}

	//m_pTalentHolder = pTalentHolder;
	//m_pSkillInst = pSkillInst;

	//stack<COptrUnit> Optr;
	//stack<COpndUnit> Opnd;
	if(!Optr.empty())
		Optr.clear();
	if(!Opnd.empty())
		Opnd.clear();
	CExprUnit cUnit, prev_cUnit;
	COpndUnit a, b;
	UUnitValue c;
	COpndUnit OpndUnit;
	size_t i = 0;
	uint32 grade = 0;
	uint32 curOptrEyes;
	VecExprUnit& expr = m_vecExpr;

	COptrUnit OptrUnit(eOI_Shell, 2, grade);
	Optr.push_back(OptrUnit);


	prev_cUnit.Set(uint32(eOI_Shell), eUT_Symbol);			//设置前置符
	cUnit = expr[i++];
	c = cUnit.m_Value;

	while (!(cUnit.m_eType == eUT_Symbol && c.lng == eOI_Shell) || Optr.back().theta != eOI_Shell)
	{ 
		switch(cUnit.m_eType)
		{
		case eUT_Number:										//如果是数字
			OpndUnit.Set(c, eUT_Number);
			Opnd.push_back(OpndUnit);
			prev_cUnit = cUnit;
			//prev_cUnit.SetFunType();							//目前数值计算未区分浮点和整数
			cUnit = expr[i++];
			c = cUnit.m_Value;
			break;
		case eUT_String:										//如果是字符串
			OpndUnit.Set(c, eUT_String);
			Opnd.push_back(OpndUnit);
			prev_cUnit = cUnit;
			cUnit = expr[i++];
			c = cUnit.m_Value;
			break;
		case eUT_Ltword:										//如果是字母组合
			if(cUnit.m_eFunType != eVT_Operator)
			{													//如果是变量
				curOptrEyes = 0;

				//注1
				prev_cUnit.Set(GetVarValue(c, cUnit.m_eFunType, pFighter, cUnit.m_eTargetType, index));	//这里可以考虑在第一次计算时设置eVT_Unknow的变量类型
				OpndUnit.Set(prev_cUnit.m_Value.flt, eUT_Number);
				Opnd.push_back(OpndUnit);

				prev_cUnit.m_eType = eUT_Number;
				prev_cUnit.SetFunType();						//目前数值计算未区分浮点和小数
				cUnit = expr[i++];
				c = cUnit.m_Value;
				continue;
				break;
			}
			curOptrEyes = COptrTbl::m_uOptrEyes[c.lng];
			{
				uint32 exprValue = expr[i].m_Value.lng;
				if(curOptrEyes == 0 || curOptrEyes == 1 &&
					(expr[i].m_eType == eUT_Symbol && exprValue != eOI_LPBr && exprValue != eOI_LSBr) ||
					(expr[i].m_eType == eUT_Ltword && expr[i].m_eFunType == eVT_Operator && COptrTbl::m_uOptrEyes[exprValue] == 2))
				{	//单目字母组合算子的下一个单元若为非左括号符号或双目字母组合算子，则转为零目算子
					//零目字母组合算子（只有字母组合允许零目，符号不允许零目）
					curOptrEyes = 0;

					//注2：这里的写法与注1的两种写法都可以，因为prev_cUnit.m_cUnit不会被用到，如果会被用到，则应该根据使用需要运算后的数值还是运算前的信息决定采用哪种写法
					OpndUnit.Set(Operate0Eyes(c.lng, pFighter, cUnit.m_eTargetType), eUT_Number);
					Opnd.push_back(OpndUnit);
					prev_cUnit = cUnit;

					prev_cUnit.m_eType = eUT_Number;
					prev_cUnit.SetFunType();						//目前数值计算未区分浮点和小数
					cUnit = expr[i++];
					c = cUnit.m_Value;
					continue;
				}
			}
		case eUT_Symbol:										//如果是操作算子（包括函数和符号）
			switch(c.lng)
			{
			case eOI_LPBr:
			case eOI_LSBr:										//如果是左括号
				grade += BRACKET_GRADE;
				cUnit = expr[i++];
				c = cUnit.m_Value;
				break;
			case eOI_RPBr:										//如果是右括号
			case eOI_RSBr:
				grade -= BRACKET_GRADE;
				if(grade < 0)
				{
					stringstream ExpStr;
					Combine(ExpStr);
					ExpStr << "表达式括号不匹配\n";
					if(!m_vecOpndArray.empty())
						ClearVector(m_vecOpndArray);
					Optr.clear();
					Opnd.clear();
					GenErr(ExpStr.str());
					return 0.0;
				}
				cUnit = expr[i++];
				c = cUnit.m_Value;
				break;
			default:
				curOptrEyes = COptrTbl::m_uOptrEyes[c.lng];

				if(curOptrEyes == 1 || (prev_cUnit.m_eType != eUT_Number && prev_cUnit.m_eType != eUT_String))		//上一个非括号单元是符号或起始号（则当前运算符为单目运算符） 单目运算符第一步
				{
					if (c.lng == eOI_Shell)
					{
						stringstream ExpStr;
						Combine(ExpStr);
						ExpStr << "表达式未结束，最后的元素不能为符号\n";
						if(!m_vecOpndArray.empty())
							ClearVector(m_vecOpndArray);
						Optr.clear();
						Opnd.clear();
						GenErr(ExpStr.str());
						return 0.0;
					}
					curOptrEyes = 1;			//这个函数里的curOptrEyes暂时没什么作用，以后可能会用到
					OptrUnit.Set(c.lng, 1, grade, cUnit.m_eTargetType);				//当前运算符为单目运算符则直接入栈
					Optr.push_back(OptrUnit);
					prev_cUnit = cUnit;
					cUnit = expr[i++];
					c = cUnit.m_Value;
				}
				else
				{
					switch(COptrTbl::OptrPrecede(Optr.back(), c.lng, grade))
					{
					case 1:			//运算符优先级 前者<后者
						OptrUnit.Set(c.lng, 2, grade, cUnit.m_eTargetType);
						Optr.push_back(OptrUnit);
						prev_cUnit=cUnit;
						cUnit=expr[i++];
						c=cUnit.m_Value;
						break;
					case 2:			//运算符优先级 前者>后者
						OptrUnit=Optr.back();
						Optr.pop_back();
						switch(OptrUnit.eyes)
						{
						case 1:		//如果是单目运算符	单目运算符第二步
							b = Opnd.back();
							Opnd.pop_back();
							//fResult = Operate1Eyes(OptrUnit.theta, b);
							OpndUnit.Set(Operate1Eyes(OptrUnit.theta, b, pFighter, OptrUnit.targettype,
								index), eUT_Number);
							Opnd.push_back(OpndUnit);
							break;
						case 2:		//如果是双目运算符
							b=Opnd.back();		
							Opnd.pop_back(); 
							a=Opnd.back();		
							Opnd.pop_back();
							//OpndUnit.Set(Operate2Eyes(a, OptrUnit.theta, b), eUT_Number);
							Opnd.push_back(Operate2Eyes(a, OptrUnit.theta, b));
							break;
						}
						break;
					}//switch
				}//if
			}//switch
			break;
		default:
			{
				stringstream ExpStr;
				Combine(ExpStr);
				ExpStr << "不明的表达式单元类型" << cUnit.m_eType << "\n";
				if(!m_vecOpndArray.empty())
					ClearVector(m_vecOpndArray);
				Optr.clear();
				Opnd.clear();
				GenErr(ExpStr.str());
				return 0.0;
			}
		}//switch
	}
	if(!m_vecOpndArray.empty())
		ClearVector(m_vecOpndArray);
	if(Opnd.empty())
	{
		Optr.clear();
		Opnd.clear();
		return 0.0;
	}
	else
	{
		double fRet = Opnd.back().value.flt;
		Optr.clear();
		Opnd.clear();
		return fRet;
	}
}



template <typename FighterType>
double CCfgCalc::GetDblValue(const FighterType* pFighterSelf, const FighterType* pFighterTarget, uint32 index) const
{
	const FighterType* pFighter[] = {pFighterSelf, pFighterTarget};
	switch(m_eStrType)
	{
	case eST_Null:
		m_eStrType = eST_Expression;
		Parse();
	case eST_Expression:
		{
			size_t exprsCount = m_vecExprs.size();
			if(m_eExprType == eET_Empty)
			{
				stringstream ExpStr;
				ExpStr << "空表格不能用GetValue获取值\n";
				GenErr(ExpStr.str());
				//return 0.0;
			}
			else if(exprsCount != 1 && index >= exprsCount)
			{
				//stringstream ExpStr;
				//ExpStr << "GetValue下标" << index << "超出表达式的数量" << exprsCount << "\n";
				//GenErr(ExpStr.str());
				////return 0.0;
				return m_vecExprs[exprsCount - 1]->Eval(pFighter, index);
			}
			else if(exprsCount == 1)
			{
				if(m_eExprType == eET_Args)
				{
					if(index >= m_vecArgs.size())
					{
						if(m_vecArgs.size() > 0)
						{
							stringstream ExpStr;
							ExpStr << "GetValue下标" << index << "超出参数的数量" << m_vecArgs.size() << "\n";
							GenErr(ExpStr.str());
							//return 0.0;
						}
					}
					return m_vecExprs[0]->Eval(pFighter, index);
				}
				else
				{
					return m_vecExprs[0]->Eval(pFighter, index);
				}
			}
			else
			{
				return m_vecExprs[index]->Eval(pFighter, index);
			}
		}
	default:
		GenPermitChangeExprTypeExp();
		return 0.0;
	}
}

template <typename FighterType>
int32 CCfgCalc::GetIntValue(const FighterType* pFighterSelf,const FighterType* pFighterTarget, uint32 index) const
{
	return int32(floor(GetDblValue(pFighterSelf, pFighterTarget, index) + 0.5));
}



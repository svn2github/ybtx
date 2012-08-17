#pragma once
//#include "LoadSkillCommon.h"
//#include "LoadSkillCommon.inl"
//
//class CCfgCalc;
//typedef map<string, CCfgCalc*>					MapGlobalSkillParam;
//
////全局变量类
//class CCfgGlobal
//{
//private:
//	static MapGlobalSkillParam					m_mapVar;
//
//public:
//	//											CCfgGlobal();
//	//											~CCfgGlobal();
//	static bool									LoadConfig(const char* cfgFile);	//读配置表
//	static void									UnloadConfig();						//卸载配置表
//	static CCfgCalc*							Get(const string& name);			//获取全局变量的值
//	static pair<bool, CCfgCalc*>				GetCfgValue(const string&name);		//获取全局变量是否存在以及值
//
////private:
////	string										strVarName;
////	CCfgCalc*									pVarValue;
////
////public:
////	string&										GetName();
////	CCfgCalc*									GetValue();
//};
//
////扩展函数
//extern int32 Rand(double fMin, double fMax);										//随机函数，返回整数
//extern double Randf(double fMin, double fMax);										//随机函数，返回浮点数
//extern double IfElse(double fIf, double fThen, double fElse);						//条件取值函数
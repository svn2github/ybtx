#pragma once
#include "TCfgCalcMallocAllocator.h"

#define EYES1_GRADE		50
#define BRACKET_GRADE	100
//#define VARIABLE_X		4294967295

//操作算子枚举编号
enum EOptrID
{
	//边界
	eOI_Shell,
	//括号
	eOI_LPBr,		eOI_LSBr,		eOI_RPBr,		eOI_RSBr,
	//二元操作
	eOI_Start2Eyes,
	eOI_Add,		eOI_Sub,		eOI_Mul,		eOI_Div,		eOI_Pow,
	eOI_EQ,			eOI_UE,			eOI_GT,			eOI_GE,			eOI_LT,
	eOI_LE,			eOI_And,		eOI_Xor,		eOI_Or,			eOI_Comma,
	eOI_DivSptZero,
	//一元操作
	eOI_Start1Eyes,
	eOI_Not,
	eOI_Rand,
	eOI_Randf,		eOI_IfElse,		eOI_ExistState,	eOI_ExistMagicState,	eOI_ExistTriggerState,
	eOI_ExistDamageChangeState,	eOI_ExistSpecialState,eOI_StateCount,eOI_TriggerCount,eOI_StateLeftTime,
	eOI_Floor,		eOI_Ceil,		eOI_Distance,	eOI_TargetIsNPC,	eOI_CharaLevel,
	eOI_IsActiveSkill,eOI_CurRlserStateCount,
	//结束
	eOI_End
};

//取消对以下函数的支持，有同等功能的变量
/*eOI_VarCfg,		eOI_VarGift,	eOI_VarMasterGift,eOI_VarTemp,	eOI_VarMasterTemp,
eOI_VarAttr,	eOI_VarMasterAttr,eOI_VarServantAttr,*/

//操作算子字符串
const string g_sOptrStr[] =
{
	"#",
	"(",			"[",			")",			"]",
	"#2",
	"+",			"-",			"*",			"/",			"^",
	"==",			"!=",			">",			">=",			"<",
	"<=",			"&&",			"^^",			"||",			",",
	"//",			
	"#1",
	"!",
	"rand",
	"randf",		"ifelse",		"existstate",	"existmagicstate",		"existtriggerstate",
	"existdamagechangestate",	"existspecialstate","statecount","triggercount","statelefttime",
	"floor",		"ceil",			"distance",		"targetisnpc",	"fighterlevel",
	"isactiveskill","currlserstatecount",
	"#e"
};

//取消对以下函数的支持，有同等功能的变量
/*"varcfg",		"vargift",		"varmastergift","vartemp",		"varmastertemp",
"varattr",		"varmasterattr","varservantattr",*/


//操作算子优先级
const int32 g_uOptrGrade[] =
{
	0,		
	BRACKET_GRADE,	BRACKET_GRADE,	BRACKET_GRADE,	BRACKET_GRADE,			
	0,		
	12,				12,				13,				13,				14,
	10,				10,				11,				11,				11,
	11,				5,				4,				3,				2,
	13,
	0
};

//表达式类型
enum EExprType
{
	eET_Empty,
	eET_Single,
	eET_Multiple,
	eET_Args,
};

//字符串类型
enum EStrType
{
	eST_Null,
	eST_Expression,
	eST_String,
	eST_MutliString,
};

//词法分析类型
enum EUnitType
{
	eUT_Null,		//初始值
	eUT_Number,		//数字
	eUT_Ltword,		//字母组合
	eUT_Symbol,		//符号
	eUT_String,		//字符串
	eUT_Array,		//用逗号隔开的参数表
	eUT_Other,		//其它
};

//执行功能类型
enum EFunType
{
	eVT_Unknow,		//未知
	eVT_Float,		//浮点型
	eVT_Integer,	//整型
	eVT_Operator,	//操作算子
	eVT_Arg,		//参数变量	
	eVT_Talent,		//天赋变量
	eVT_MasterTalent,	//主人天赋变量
	eVT_Attr,		//属性变量
	eVT_MasterAttr,	//主人属性变量
	eVT_ServantAttr,//仆人属性变量
	eVT_Temp,		//临时变量
	eVT_MasterTemp,	//主人临时变量
	eVT_Cfg,		//全局变量
	eVT_End,		//建map循环结束标志，无意义
};

//各表达式元素类型表示方式
/*			词法分析类型	执行功能类型
整数		eUT_Number		eVT_Integer
浮点数		eUT_Number		eVT_Float
字符串		eUT_String
运算符		eUT_Symbol		eVT_Operate
函数		eUT_Ltword		eVT_Operate
参数变量	eUT_Ltword		eVT_Talent
天赋变量	eUT_Ltword		eVT_Talent
主人天赋变量	eUT_Ltword		eVT_MasterTalent
属性变量	eUT_Ltword		eVT_Attr
临时变量	eUT_Ltword		eVT_Temp
全局变量	eUT_Ltword		eVT_Cfg
函数参数组	eUT_Array
*/

//变量类型字符串
const string g_sVarTypeStr[] =
{
	"",
	"",
	"",
	"",
	"参数",
	"天赋",
	"主人天赋",
	"属性",
	"主人属性",
	"仆人属性",
	"临时",
	"主人临时",
	"全局",
	"#e"
};

enum ECalcTargetType
{
	eCTT_Self,			//自身
	eCTT_Target,		//目标
	eCTT_Master,		//主人
	eCTT_End,			//结束标志
};

const string g_sCalcTargetTypeStr[] = 
{
	"自身",
	"目标",
	"主人",
	"#e"
};

//操作算子元素
class COptrUnit			//操作符类
	: public CCfgCalcMallocObject
{
public:
	uint32	theta;
	uint32	eyes;
	uint32	grade;
	ECalcTargetType	targettype;
	COptrUnit(uint32 t, uint32 e, uint32 g = 0, ECalcTargetType target = ECalcTargetType(0));
	void Set(uint32 t, uint32 e, uint32 g, ECalcTargetType target = ECalcTargetType(0)) {
		theta = t; eyes = e; grade = g; targettype = target;}

};

class COpndUnit;
class VecOpnd : public vector<COpndUnit, TCfgCalcMallocAllocator<COpndUnit> >		
	, public CCfgCalcMallocObject
{

};

class CTempVarMgrServer;

//操作值共用体类型
union UUnitValue
{
	double		flt;
	uint32		lng;
	string*		pStr;
	VecOpnd*	pArr;
	//类成员指针问题2010-12-21
	//int32 CTempVarMgrServer::*		pOff;

};

//操作值元素
class COpndUnit			
	: public CCfgCalcMallocObject
{
public:
	UUnitValue	value;
	EUnitType	type;
	COpndUnit();
	COpndUnit(double v, EUnitType t = eUT_Null) {Set(v,t);}
	COpndUnit(VecOpnd* v, EUnitType t = eUT_Null) {Set(v,t);}
	void Set(double v, EUnitType t = eUT_Null) {value.flt = v; type = t; }
	void Set(uint32 v, EUnitType t = eUT_Null) {value.lng = v; type = t; }
	void Set(string* v, EUnitType t = eUT_Null) {value.pStr = v; type = t; }
	void Set(VecOpnd* v, EUnitType t = eUT_Null) {value.pArr = v; type = t; }
	void Set(UUnitValue v, EUnitType t = eUT_Null) {value = v; type = t; }
	void Set(const COpndUnit& v) {value = v.value; type = v.type;}
};

typedef map<string, EOptrID, less<string>, TCfgCalcMallocAllocator<pair<string, EOptrID> > >			MapStr2Optr;
typedef map<string, EFunType, less<string>, TCfgCalcMallocAllocator<pair<string, EFunType> > >			MapStr2VarType;
typedef map<string, ECalcTargetType, less<string>, TCfgCalcMallocAllocator<pair<string, ECalcTargetType> > >	MapStr2CalcTargetType;

//表达式用的静态映射表。
class COptrTbl
	: public CCfgCalcMallocObject
{
public:
	static uint32				OptrPrecede(const COptrUnit& optr_a, uint32 b, uint32 grade_b);
	//操作算子优先级比较
	static EOptrID				GetOptrID(const string& str) {return m_mapOptrNo[str];}
	//获取操作算子ID
	static MapStr2Optr			m_mapOptrNo;						//操作算子map
	static uint32				m_uOptrEyes[eOI_End];				//操作算子目数数组
	static MapStr2VarType		m_mapVarTypeNo;						//变量类型map
	static MapStr2CalcTargetType m_mapCalcTargetTypeNo;				//目标类型map
private:
	static bool					Init();
	static bool					__init;
};


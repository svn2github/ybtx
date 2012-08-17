#pragma once
#include "CCfgCalcOptrTable.h"
#include "CDynamicObject.h"
#include "TCfgCalcMallocAllocator.h"

#ifdef _WIN32
#pragma warning (push)
#pragma warning (disable:4996)
#endif


//解析部分

//CExprUnit表达式元素类
class CExpression;
class CCfgCalc;
class CCfgRelationDeal;
class CExprUnit
	: public CCfgCalcMallocObject
{
	friend class CExpression;
	friend class CCfgCalc;
	friend class CCfgRelationDeal;
public:
	~CExprUnit()								{}
private:
	CExprUnit():m_eType(eUT_Null),m_eFunType(eVT_Unknow)			{}
	CExprUnit(double value)						{Set(value, eUT_Number);}
	CExprUnit(uint32 value)						{Set(value);}
	CExprUnit(uint32 value, EUnitType type)		{Set(value, type);}

	CExprUnit& operator =(double value)	{Set(value, eUT_Number); return *this;}
	CExprUnit& operator =(uint32 value)	{Set(value); return *this;}
	void Set(double value);
	void Set(uint32 value);
	void Set(string* value);
	void Set(double value, EUnitType type);
	void Set(uint32 value, EUnitType type);
	void Set(string* value, EUnitType type);
	void SetType(EUnitType type);
	void SetFunType(EFunType type = eVT_Unknow);					//设置功能类型
	void SetTargetType(ECalcTargetType type);
	//void GetFloat();
	//void GetInt();

	UUnitValue	m_Value;
	EUnitType	m_eType;
	EFunType	m_eFunType;
	ECalcTargetType	m_eTargetType;
};



//CPreUnit预处理表达式元素类
class CPreUnit
	: public CCfgCalcMallocObject
{
	friend class CExpression;
	friend class CCfgCalc;
private:
	string		str;
	EUnitType	type;
	inline void	clear()							{str.clear(); type = eUT_Null;}
	inline bool	empty()							{return type == eUT_Null;}
	CPreUnit():type(eUT_Null)					{str.clear();}
	inline void Set(const string& s, EUnitType t)		{str = s;	type = t;}
};

//VecPreUnit预处理表达式类
typedef vector<CPreUnit*, TCfgCalcMallocAllocator<CPreUnit*> >		VecPreUnit;
typedef vector<double, TCfgCalcMallocAllocator<double> >			VecArgs;

//enum EFighterTypeIndex{eFTI_Gift, eFTI_Attr, eFTI_Temp};

//Expression表达式类
class CTalentHolder;
//class CFighterDictator;
//class CFighterDirector;
class CFighterNull;
class CFighterClient;
class CSkillInstServer;
class CSkillInstNull;
class CCfgCalc;
class CExpression
	: public CCfgCalcMallocObject
{
	friend class CCfgCalc;
	friend class CCfgRelationDeal;
private:
	typedef vector<CExprUnit, TCfgCalcMallocAllocator<CExprUnit> >		VecExprUnit;
	typedef vector<VecOpnd*, TCfgCalcMallocAllocator<VecOpnd*> >			VecVecOpnd;
	typedef vector<COptrUnit, TCfgCalcMallocAllocator<COptrUnit> >		VecOptrUnit;
	typedef vector<COpndUnit, TCfgCalcMallocAllocator<COpndUnit> >		VecOpndUnit;

public:
	~CExpression();

private:
	CExpression(const string& str, const CCfgCalc* pCfgCalc);
	CExpression(double dblValue, const CCfgCalc* pCfgCalc);
	CExpression(const CExpression& srcExpr, const CCfgCalc* pCfgCalc);

	template<typename FighterType>
	double			Eval(const FighterType* pFighter[], uint32 index = 0);

	double			Eval(uint32 index = 0)
	{
		const CFighterNull* pFighter[] = {(CFighterNull*)NULL, (CFighterNull*)NULL};
		Eval(pFighter, index);
		return 0;
	}

	void			Test();

private:
	void			Input(const string& str, VecArgs* pVecArgs);		//输入表达式的字符串
	void			Input(double dblValue);								//直接由一个数值产生一个表达式
	void			GetCharType(TCHAR c, EUnitType &type);				//获取单个字符类型
	uint32			Separate();											//表达式预处理第一步：分离字符串元素
	void			Combine(stringstream& str);							//合并字符串元素，Separate()的逆操作
	void			Convert();											//表达式预处理第二步：把字符串元素序列转换成表达式
	string&			GetVarName(uint32 index);							//由下标获取变量名（暂时不用）
	EFunType		GetVarType(string& varName);						//获取变量类型
	ECalcTargetType	GetTargetType(string& varName);						//获取目标类型

	template<typename FighterType>
	double			GetVarValue(UUnitValue varHandle, EFunType& vartype, const FighterType* pFighter[],
		ECalcTargetType eTargetType, uint32 index);
	//获取变量的值

	pair<bool, double>	GetArgValue(uint32 index);						//获取参数的值

	template<typename FighterType>
	pair<bool, double>	GetTalentValue(const string& varName, const FighterType* pFighter);		//获取天赋的值

	template<typename FighterType>
	pair<bool, double>	GetMasterTalentValue(const string& varName, const FighterType* pFighter);		//获取主人天赋的值

	template<typename FighterType>
	pair<bool, double>	GetAttrValue(uint32 uVarID, const FighterType* pFighter);				//获取人物属性的值

	template<typename FighterType>
	pair<bool, double>	GetMasterAttrValue(uint32 uVarID, const FighterType* pFighter);				//获取人物属性的值

	template<typename FighterType>
	pair<bool, double>	GetServantAttrValue(uint32 uVarID, const FighterType* pFighter);				//获取人物属性的值

	template<typename FighterType>
	pair<bool, double>	GetTempValue(UUnitValue uVarPtr, const FighterType* pFighter);		//获取临时的值（用于没有服务器技能上下文的情况，一律返回<false, 0.0>）

	template<typename FighterType>
	pair<bool, double>	GetMasterTempValue(UUnitValue uVarPtr, const FighterType* pFighter);		//获取临时的值（用于没有服务器技能上下文的情况，一律返回<false, 0.0>）

	template<typename FighterType>
	double			Operate0Eyes(uint32 theta, const FighterType* pFighter[], ECalcTargetType eTargetType);

	template<typename FighterType>
	double			Operate1Eyes(uint32 theta, const COpndUnit& opnd_b, const FighterType* pFighter[],
		ECalcTargetType eTargetType, uint32 index);
	//计算一元运算符或函数的操作（普通的函数也算一元算子）

	COpndUnit		Operate2Eyes(const COpndUnit& opnd_a, uint32 theta, const COpndUnit& opnd_b);
	//计算二元运算符的操作

	bool			SetVarHandle(string& sName, CExprUnit& exprunit);

	template<typename FighterType>
	const FighterType*	GetCalcFighter(const FighterType* pFighter[], ECalcTargetType eTargetType);

	string			m_sExprStr;											//用于保存输入的表达式字符串
	VecPreUnit		m_vecPreExpr;										//用于保存分离成字符串元素序列的表达式
	VecExprUnit		m_vecExpr;											//用于保存序列化和类型化的表达式
	VecArgs*		m_pVecArgs;											//用于保存参数表达式的参数序列
	VecVecOpnd		m_vecOpndArray;										//用于保存逗号操作符运算过程中产生的值的序列
	//CTalentHolder*	m_pTalentHolder;
	const CCfgCalc*				m_pCfgCalc;

	//只在Eval中用的成员
	VecOptrUnit Optr;
	VecOpndUnit Opnd;
};


//CCfgCalc计算格类
typedef vector<CExpression*, TCfgCalcMallocAllocator<CExpression*> >			VecExprs;
typedef vector<string*, TCfgCalcMallocAllocator<string*> >					VecString;
//class CFighterMediator;
class CCfgCalc
	: public virtual CDynamicObject
	, public CCfgCalcMallocObject
{
	friend class CExpression;
	friend class CCfgRelationDeal;
public:
	CCfgCalc();
	~CCfgCalc();
	CCfgCalc(int32 intValue);
	CCfgCalc(double dblValue);
	CCfgCalc(const string& str);
	CCfgCalc(const CCfgCalc& srcCfgCalc);
	void					InputString(const string& str);					//用于输入表达式字符串

	//获取表达式的浮点结果值
	template <typename FighterType>
	double					GetDblValue(const FighterType* pFighter, uint32 index = 0) const
	{
		return GetDblValue(pFighter, pFighter, index);
	}

	template <typename FighterType>
	double					GetDblValue(const FighterType* pFighterSelf, const FighterType* pFighterTarget, uint32 index = 0) const;

	//获取表达式的整型结果值
	template <typename FighterType>
	int32					GetIntValue(const FighterType* pFighter, uint32 index = 0) const
	{
		return GetIntValue(pFighter, pFighter, index);
	}

	template <typename FighterType>
	int32					GetIntValue(const FighterType* pFighterSelf, const FighterType* pFighterTarget, uint32 index = 0) const;

	double GetDblValue(uint32 index = 0) const
	{
		return GetDblValue((CFighterNull*)NULL, index);
	}

	int32 GetIntValue(uint32 index = 0) const
	{
		return GetIntValue((CFighterNull*)NULL, index);
	}



	const string&			GetString() const;								//获取单字符串情况时的整个字符串
	const string&			GetString(uint32 index) const;					//获取多字符串情况时的第index+1个字符串
	const TCHAR*				GetChars(uint32 index) const	{return GetString(index).c_str();}	//获取多字符串情况时的第index+1个字符串的字符数组

	const uint32			GetStringCount() const;							//获取多字符串情况时的字符串的数量
	const uint32			GetValueCount() const;							//获取表达式情况时的结果值的数量

	void					SetTypeExpression() const;						//强制设置类型为表达式
	void					SetTypeSingleString() const;					//强制设置类型为单字符串
	void					SetTypeMultiString() const;						//强制设置类型为多字符串

	inline EExprType		GetExprType() const		{return m_eExprType;}	//获取表达式的类型
	inline EStrType			GetStrType() const		{return m_eStrType;}	//获取字符串的类型
	const bool				ExprIsEmpty() const		{return m_eExprType == eET_Empty;}
	const bool				StrIsEmpty() const		{return m_eStrType == eST_String && m_sString.empty();}
	const bool				IsEmpty() const			{return m_eExprType == eET_Empty || m_eExprType != eET_Empty && m_sString.empty();}

	bool					IsSingleNumber() const;								//用来判断是否为纯数字。这个函数的条件很复杂，尽量不要在初始化和读配置表以外的地方用

	void					Test();

	const string&			GetTestString()	const	{return m_sString;}		//调试用，用于获取任何情况下的m_sString

	void					SetStringSplitter(TCHAR cSplitter = ',') const	{m_cStringSplitter = cSplitter;}

	bool					CalcNeedTarget() const	{return m_bCalcNeedTarget;}
	bool					IsConstValue() const	{return m_bIsConstValue;}

private:
	void					Exuviate(const string& str);					//去掉字符串中由Excel表保存成txt文件时额外添加的双引号
	void					Parse() const;									//分析出每个表达式
	void					ParseString() const;							//分析出每个字符串
	void					GenPermitChangeExprTypeExp() const;				//抛出禁止改变类型的错误

	mutable EExprType		m_eExprType;
	mutable EStrType		m_eStrType;
	mutable VecExprs		m_vecExprs;
	mutable string			m_sString;
	mutable VecString		m_vecString;
	mutable VecArgs			m_vecArgs;
	mutable TCHAR			m_cStringSplitter;
	mutable bool			m_bCalcNeedTarget;
	mutable bool			m_bIsConstValue;
};











class CFighterNull
	: public CCfgCalcMallocObject
{
	friend class CExpression;
	friend class CCfgCalc;
private:
	CTalentHolder* GetTalentHolder() const 	{return NULL;}
	uint32 CppGetLevel() const	{return 0;}
};

#ifdef _WIN32
#pragma warning (pop)
#endif 


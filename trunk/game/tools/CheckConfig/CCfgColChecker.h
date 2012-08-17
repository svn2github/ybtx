#pragma once
#include "CTxtTableFile.h"
#include "TConfigAllocator.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

typedef map<string, uint32>				MapEnum;

typedef bool (*FIELD_CHECKER) (float, float);
typedef bool (*FIELD_CHECKER_STRING) (const string&);

class CCfgCalc;

enum EErrorReactType
{
	eERT_AtOnce,
	eERT_AtEnd,
	eERT_Never,
};



//列约束检测命名空间
namespace CfgChk
{
#define CANEMPTY	true
#define MUSTFILL	false
#define YES			true
#define NO			false
#define CHECKCFG

	//输出信息用的全局对象
	extern bool g_bIsCheckingCfg;			//表示正在检测配置表时的标志位
	extern CTxtTableFile* g_pTabFile;			//表对象
	extern string g_sTabName;				//表名
	extern string g_sSrcTabLineName;
	extern string g_sMagicEffName;
	extern int32 g_iLine;					//行号
	extern string g_sTitle;					//列标题
	extern bool g_bCheckInReading;			//是否在读表时检测
	extern EErrorReactType g_uErrReact;		//错误反应
	extern bool g_bExistError;				//存在错误
	extern uint64 g_uTimeForTest;				//测试用

	bool	ExistError();
	bool	ExpIsForbidden();
	bool	ExpAtOnce();
	bool	ExpAtEnd();
	void	GenExpInfo(const string& sExp);
	void	SetTabFile(CTxtTableFile& TabFile,  const string& TabName);		//关联配置表文件
	void	ResetTab(CTxtTableFile& TabFile, const string& TabName);
	void	SetTabName(const string& TabName);
	void	SetTabNull();
	void	SetLineNo(int32 iLine);
	void	SetItemTitle(const TCHAR* sTitle);

	template<class EnumType>
	void	CreateMap(map<string, EnumType> &mapEnum, uint32 uCount, ...);

	template<class NodeType>
	void	CreateVector(vector<NodeType> &vecNode, uint32 uCount, ...);

	
	bool	GT(const float x, const float a);					//x > a
	bool	GE(const float x, const float a);					//x >= a
	bool	LT(const float x, const float a);					//x < a
	bool	LE(const float x, const float a);					//x <= a
	bool	EQ(const float x, const float a);					//x == a
	bool	NE(const float x, const float a);					//x != a
	bool	IsRate(const float x, const float a);				//x >= 0.0f && x <= 1.0f


	//读取配置表的一行的sTitle项到tReader
	void ReadItem(uint32& tReader, const TCHAR* sTitle,
		FIELD_CHECKER pFunFieldChecker, uint32 fFieldCheckerParam);

	void ReadItem(uint32& tReader, const TCHAR* sTitle, bool bCanEmpty = false,
		uint32 tDefault = 0, FIELD_CHECKER pFunFieldChecker = NULL, uint32 fFieldCheckerParam = 0);

	void ReadItem(string& tReader, const TCHAR* sTitle, FIELD_CHECKER_STRING pFunFieldChecker);

	void ReadItem(string& tReader, const TCHAR* sTitle, bool bCanEmpty = false,
		const string& tDefault = "", FIELD_CHECKER_STRING pFunFieldChecker = NULL);

	void ReadItem(CCfgCalc*& tReader, const TCHAR* sTitle, FIELD_CHECKER pFunFieldChecker,
		float fFieldCheckerParam = 0.0f);

	void ReadItem(CCfgCalc*& tReader, const TCHAR* sTitle, bool bCanEmpty = false,
		float tDefault = 0.0f, FIELD_CHECKER pFunFieldChecker = NULL, float fFieldCheckerParam = 0.0f);
	
	void ReadMixedItem(CCfgCalc*& tReader, const TCHAR* sTitle, bool bCanEmpty = false, 
		const string& tDefault = "");

	template<class ReaderType>
	void ReadItem(ReaderType& tReader, const TCHAR* sTitle,
		FIELD_CHECKER pFunFieldChecker, ReaderType fFieldCheckerParam);

	template<class ReaderType>
	void ReadItem(ReaderType& tReader, const TCHAR* sTitle, bool bCanEmpty = false,
		ReaderType tDefault = 0, FIELD_CHECKER pFunFieldChecker = NULL, ReaderType fFieldCheckerParam = 0);

	template<class EnumType>
	void ReadItem(EnumType& tReader, const TCHAR* sTitle, map<string, EnumType, less<string>, 
		TConfigAllocator<pair<string, EnumType > > >& pFieldMap);

	template<class EnumType>
	void ReadItem(EnumType& tReader, const TCHAR* sTitle, bool bCanEmpty, EnumType tDefault, map<string, EnumType, less<string>, 
		TConfigAllocator<pair<string, EnumType > > >& pFieldMap);

	template<class EnumType1, class EnumType2>
	uint32 ReadItem(EnumType1& tReader1, EnumType2& tReader2, const TCHAR* sTitle,
		map<string, EnumType1>& pFieldMap1, map<string, EnumType2>& pFieldMap2);

	template<class EnumType1, class EnumType2>
	uint32 ReadItem(EnumType1& tReader1, EnumType2& tReader2, const TCHAR* sTitle,
		bool bCanEmpty, map<string, EnumType1>& pFieldMap1,
		map<string, EnumType2>& pFieldMap2);


	
	template<class ReaderType>
	void	GenExpInfo(const string& sExp, const ReaderType& tValue);



	//检测是否可为空以及赋默认值
	template<class DefaultType>
	void CheckEmpty(CCfgCalc*& tReader, const string& sValue, bool bCanEmpty, const DefaultType& tDefault);

	template<class ReaderType>
	void CheckEmpty(ReaderType& tReader, const string& sValue, bool bCanEmpty, const ReaderType& tDefault);


	//赋值
	void SetValue(int32& tReader, const string& sValue);
	void SetValue(float& tReader, const string& sValue);
	void SetValue(bool& tReader, const string& sValue);
	void SetValue(string& tReader, const string& sValue);
	void SetValue(CCfgCalc*& pCalc, const string& sValue, bool bIsSingle = true);

	template<class EnumType>
	void SetValue(EnumType& tReader, const string& sValue, map<string, EnumType, less<string>, 
		TConfigAllocator<pair<string, EnumType > > >& pFieldMap);
	
	
	//检测值是否符合uint32类型（即大于或等于0）
	void CheckUIntType(int32& tReader);


	//检测取值范围
	void CheckField(string& sValue, FIELD_CHECKER_STRING pFunFieldChecker);

	template<class ReaderType>
	void CheckField(ReaderType& tReader, FIELD_CHECKER pFunFieldChecker, ReaderType fFieldCheckerParam);

	template<class EnumType>
	bool InField(string& sValue, map<string, EnumType>& pFieldMap);

	template<class EnumType>
	void CheckField(string& sValue, map<string, EnumType, less<string>, 
		TConfigAllocator<pair<string, EnumType > > >& pFieldMap);


	//插入配置表的一行到配置表map
	template<class CfgNodeType>
	bool InsertMap(map<string, CfgNodeType*>& mapCfg, CfgNodeType* pCfgNode);

	template<class CfgNodeType>
	bool InsertMap(map<uint32, CfgNodeType*>& mapCfg, CfgNodeType* pCfgNode);

};

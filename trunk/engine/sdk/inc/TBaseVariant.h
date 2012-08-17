#pragma once
#include "VariantTypes.h"
#include "TVariantTraits.h"
#include "CommonDefs.h"
#include "CVariantMallocObject.h"

namespace sqr
{
	template <typename ImpVariant>
	class COMMON_API TBaseVariant
		:public CVariantMallocObject
	{
	public:
		TBaseVariant();
		TBaseVariant(const TBaseVariant&);
		virtual ~TBaseVariant();

		//获得当前值的类型

		//ValueType只支持c++内建数值类型
		//将当前值作为ValueType类型获取其值，如果当前值不是ValueType类型，则产生异常。
		template<typename ValueType>ValueType GetNumber()const;
		template<typename ValueType>const ValueType* GetNumberPtr()const;

		//ValueType只支持c++内建数值类型
		//将当前值设置成一个ValueType数值
		template<typename ValueType>
		void SetNumber(ValueType Value,ostream* strm=NULL);

		//将当前值作为字符串类型获取其值，如果当前值不是字符串，则产生异常。
		const char* GetString()const;
		//将当前值设置为一个字符串（必须以\0结尾，支持\0结尾性价比比较低）。
		void SetString(const char* szValue,ostream* strm=NULL);

		//将当前值设置成空表
		void SetEmptyTable(ostream* pStrm=NULL);

		bool IsTable()const;
		bool IsEmptyTable() const;
		//szName是变量名，必须满足下列规则：
		//1、变量名必须以字母开头
		//2、变量名只能由字母、数字和下划线组成，而不能包含空格、标点符号、运算符等其它符号。


		//将当前值作为一个表来使用（如果当前值不是表，则先将其变成一个空表），并设置一个名称为szName（\0结尾）的空的表成员。
		ImpVariant* SetMember(const char* szName,ostream* strm=NULL);

		//将当前值作为表类型并获取其某个成员，如果当前值不是表，则产生异常，如果成員不存在，則返回NULL。
		ImpVariant* GetMember(const char* szName)const;

		//将当前值作为一个表来使用（如果当前值不是表，则产生异常），并删除表的名为szName（\0结尾）的成员。
		//return: true表示删除成功，false表示没有找到成员
		bool DelMember(const char* szName,ostream* strm=NULL);

		bool EnumMember(const void* pContext,bool(*)(const void* pContext,const char* szKey,ImpVariant& Member))const;

	protected:
		char* IntGetString();
		void IntFreeString();

		template<typename ValueType>
		ValueType IntGetNumber() const;

		template<typename ValueType>
		const ValueType* IntGetNumberPtr() const;

	private:
		bool IsType(EVariantType eType) const;
		void SetType(EVariantType eType);
		
	protected:
		void*			m_pData;
		char			m_szNumBuffer[sizeof(uint64)];
		bool			m_bDataNotInit;
	};

}

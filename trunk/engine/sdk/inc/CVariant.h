#pragma once
#include "VariantTypes.h"
#include "TVariantTraits.h"
#include "CommonDefs.h"
#include "CVariantMallocObject.h"
#include "TVariantAllocator.h"
//#include "TBaseVariant.h"
#include "ValueVariantDef.h"
#include "CDynamicObject.h"
#include "CompileTimeHelper.h"

namespace sqr
{
	class CVariant;
	class CBaseObject;

	class COMMON_API CVariant
		:public CVariantMallocObject
		//: public TBaseVariant<CVariant>
	{
	public:
		//friend class TBaseVariant<CVariant>;

		CVariant();	//默认值是一个空的table
		CVariant(const CVariant& var);
		virtual ~CVariant(void);

		const CVariant& operator=(const CVariant& OtVar);

		EValueVariantType GetType() const
		{
			return m_eType;
		}

		void SetString(const char* szValue);
		const char* GetString()const;

		template<typename ValueType>
		void SetNumber(ValueType Value);
		template<typename ValueType>
		ValueType GetNumber()const;
		template<typename ValueType>
		const ValueType* GetNumberPtr()const;

		CVariant* SetMember(const char* szName);
		CVariant* GetMember(const char* szName)const;
		bool DelMember(const char* szName);
		void SetEmptyTable();
		bool IsEmptyTable() const;

		//这个跟string不一样的地方在于只会保存外部字符串的指针
		void SetCStr(const char* szCStr);
		const char* GetCStr() const;
		
		void SetPtr(const void* pObjectPtr);
		void SetPtr(const CBaseObject* pObject);
		const void* GetPtr() const;
		
		template<typename ClassType>
		const ClassType* GetPtr()
		{
			const void* pPointer = GetPtr();

			//如果这个类是CBaseObject的子类，那么我们通过CBaseObject转成这个类指针
			if (ClassConversion<ClassType*, CBaseObject*>::exists == 1)
			{
				return dynamic_cast<const ClassType*>((const CBaseObject*)pPointer);
			}
			else
			{
				return (const ClassType*)pPointer;
			}
		}

		CVariant* GetRoot()const;
		CVariant* GetParent()const;

		void Assign(const CVariant& OtVar);

		bool EnumMember(const void* pContext,bool(*)(const void* pContext,const char* szKey,CVariant& Member))const;

	private:
		typedef basic_string<char, std::char_traits<char>, TVariantAllocator<char> > String_t;
		typedef map<String_t,CVariant*,less<String_t>,
			TVariantAllocator<pair<const String_t,CVariant*> > >	MemberMap_t;

		//using TBaseVariant<CTableVariantTraits>::GetNumber;
		//using TBaseVariant<CTableVariantTraits>::SetNumber;

		//将当前值作为一个表来使用（如果当前值不是表，则先将其变成一个空表），并设置一个名称为szName（\0结尾）的空的表成员。
		CVariant* IntSetMember(const char* szName);

		//将当前值作为表类型并获取其某个成员，如果当前值不是表，则产生异常，如果成員不存在，則返回NULL。
		CVariant* IntGetMember(const char* szName)const;

		//将当前值作为一个表来使用（如果当前值不是表，则产生异常），并删除表的名为szName（\0结尾）的成员。
		//return: true表示删除成功，false表示没有找到成员
		bool IntDelMember(const char* szName);

		bool IntEnumMember(const void* pContext,bool(*)(const void* pContext,const char* szKey,CVariant& Member))const;
		

		virtual CVariant* CreateVariant()const;

		void CheckVariableName(const char* szName) const;

		const MemberMap_t::iterator& GetIt()const;
		
		void IntSetType(EVariantType eType);
		bool IntIsType(EVariantType eType) const;

		static bool AssignMember(const void* pThis,const char* szKey,CVariant& OtVar);
		template<typename NumberType>void AssignNumber(const CVariant& OtVar);

		void IntFreeString();
		void FreeDataMem();

		void ReturnToBank();

		MemberMap_t* GetMemberMap();
		const MemberMap_t* GetMemberMap() const;

		void SetMemberMap(MemberMap_t* pMemberMap);

		CVariant*				m_pParent;
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4251)
#endif
		MemberMap_t::iterator	m_it;
#ifdef _WIN32
#pragma warning(pop)
#endif

		EValueVariantType	m_eType;
		void*				m_pData;
		char				m_szNumBuffer[sizeof(uint64)];
	};
}

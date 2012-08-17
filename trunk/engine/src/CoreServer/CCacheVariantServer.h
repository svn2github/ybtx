#pragma once

#include <sstream>

#include "BaseDefs.h"
#include "CCypherVariant.h"

namespace sqr
{
	class CCypherVariantHolder;
	class CSyncVariantServer;
	class CSyncVariantServerHolder;
	class CCacheVariantServerHolder;

	class CCacheVariantServer
		:protected CCypherVariant
	{
		friend class CSyncVariantServer;
		friend class CSyncVariantServerHolder;
		friend class CCacheVariantServerHolder;
		friend class CCypherVariantHolder;
	public:

		const char* GetString()const;

		template<typename ValueType>
		ValueType GetNumber() const;
		bool IsEmptyTable() const;
		//将当前值作为表类型并获取其某个成员，如果当前值不是表，则产生异常，如果成員不存在，則返回NULL。
		CCacheVariantServer* GetMember(const char* szName)const;

		inline CCacheVariantServer* GetParent()const
		{
			return static_cast<CCacheVariantServer*>( CCypherVariant::GetParent() );
		}
	private:
		explicit CCacheVariantServer(CCacheVariantServerHolder *pHolder);
		~CCacheVariantServer(void);
	
		inline CCacheVariantServerHolder* GetHolder()const
		{
			return m_pHolder;
		}

		//ValueType只支持c++内建数值类型
		//将当前值设置成一个ValueType数值
		template<typename ValueType>
		void SetNumber(ValueType Value);
		//将当前值设置为一个字符串（必须以\0结尾，支持\0结尾性价比比较低）。
		void SetString(const char* szValue);
		//将当前值作为一个表来使用（如果当前值不是表，则先将其变成一个空表），并设置一个名称为szName（\0结尾）的空的表成员。
		CCacheVariantServer* SetMember(const char* szName);
		//将当前值设置成空表
		void SetEmptyTable();

		//将当前值作为一个表来使用（如果当前值不是表，则产生异常），并删除表的名为szName（\0结尾）的成员。
		//return: true表示删除成功，false表示没有找到成员
		bool DelMember(const char* szName);

		virtual CCypherVariant* CreateVariant()const;
		virtual void ResetDataMember();
		virtual void RecycleSubTree();
		//void InitAllMember();

		CCacheVariantServerHolder *m_pHolder;

		//void SetSyncVariantServer(CSyncVariantServer* pSyncVariantServer);
		//CSyncVariantServer* GetSyncVariantServer()const;
		//CSyncVariantServer* m_pSyncVariant;
	};
}

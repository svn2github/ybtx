#pragma once

#include "CCypherVariantHolder.h"
#include "CVariantHolderDef.h"
#include "CCypherVariantStream.h"

namespace sqr
{
	class CSyncCoreObjectDictator;
	class IMulticast;
	class CCacheVariantServer;
	class CSynConnServer;
	template<typename VariantHolderType>
	class TVariantHolderPoolServer;
	class CObjVarDefCfg;
	class CVariantSyncStream;

	template <class T>class TVariantJobAllocator;
	typedef basic_stringstream<char,std::char_traits<char>,TVariantJobAllocator<char> > CVariantJobStream;
	typedef basic_string<char,std::char_traits<char>,TVariantJobAllocator<char> > CVariantJobString;
	
	class CCacheVariantServerHolder
		:private CCypherVariantHolderForDecode
	{
		friend class CCacheVariantServer;
	public:
		CCacheVariantServerHolder(uint32 uVarDefID, CCypherVariantCodeTable *pCodeTable, CSyncCoreObjectDictator* pCoreObj, bool bForSync, ECodeTableIndex eCodeTableIndex);
		~CCacheVariantServerHolder();

		using CCypherVariantMallocObject::operator new;
		using CCypherVariantMallocObject::operator new[];
		using CCypherVariantMallocObject::operator delete;
		using CCypherVariantMallocObject::operator delete[];

		static CObjVarDefCfg* GetVarDefCfg();
		
		CCacheVariantServer* GetRootVariant() const;		
		
		//同步
		void SyncToConn(CSynConnServer* pConn);

		//Decode逻辑线程发过来的stream并同步给客户端
		void DecodeAndSyncStream(CVariantSyncStream& strm);
		uint32 GetRealEncodeTimes();
	private:
		
		void GetStringFromStaticStream( string& str );

		bool NoTargetToSync()const;

		void SyncStreamAndSetFlag();

		template<typename StreamType>
		void SyncStringToAll( const StreamType& str );
		void IncEncodeTime();
		void IncRealEncodeTime();
		

		template <class CmdType>
		static void MulticastCmd(CCacheVariantServerHolder* pThis,const char* szStr,size_t stSize,CSynConnServer* pConn);

		typedef void (*MulticastCmdFunc)(CCacheVariantServerHolder* pThis,const char* szStr,size_t stSize,CSynConnServer* pConn);
		MulticastCmdFunc GetMulticastCmdFunc(size_t uSize);



		template <class CmdType>
		static void UniquecastCmd(CCacheVariantServerHolder* pThis,const char* szStr,size_t stSize,CSynConnServer* pConn);

		typedef void (*UniquecastCmdFunc)(CCacheVariantServerHolder* pThis,const char* szStr,size_t stSize,CSynConnServer* pConn);
		UniquecastCmdFunc GetUniquecastCmdFunc(size_t uSize);

		//设置脏数据位
		inline void SetDirty(bool bDirty = true)
		{
			m_bDirtyFlag = bDirty;
		}
		inline bool IsDirty() const
		{
			return m_bDirtyFlag;
		}
		//获得缓冲
		void UpdateStringBufferIfNeeded();
		virtual CCypherVariant* CreateVariant() const;

		virtual CCypherVariant* IntAddSubTreeForDecode(CCypherVariant* pRoot, uint32 uSubTreeType, uint32 uSubTreeIndex);

		bool m_bForSync;
		ECodeTableIndex m_eCodeTableIndex;
		IMulticast* m_pMulticast;
		CSyncCoreObjectDictator* m_pCoreObj;
		uint8 m_uEncodeTimes;
		uint32 m_uRealEncodeTimes;
		bool m_bDirtyFlag;

		CCypherVariantStream m_sInitSyncData;
		uint32 m_uVarDefID;
	};

}

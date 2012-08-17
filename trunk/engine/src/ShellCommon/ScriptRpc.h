#pragma once
#include "BaseTypes.h"
#include "ScriptX.h"
#include "CBaseScriptApp.h"
#include <iosfwd>
#include "CDynamicObject.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include "IPipe.h"

//int32 RegisterRpc(sqr::CScript& Script);
namespace sqr
{

	template<typename ConnType>class TPackerTable;
	template<typename ConnType>class TUnPackerTable;

	template<typename ConnType,typename ImpClass>
	class TBasePacker
	{
		friend class TPackerTable<ConnType>;
	public:
		HPACKAGE m_hHandle;
	private:
		static bool	_Pack( uint32 uID, void * pPack, CBaseObject * pBase, void* pBuf, size_t stBufSize )
		{
			ConnType* pConn = dynamic_cast<ConnType*>(pBase);
			if (pConn == NULL)
				return false;

			uint16 uID16 = uint16(uID);

			//这个静态buffer决定了rpc只能在整个进程中的唯一一个线程里面使用
			static string ls_sBuffer;

			ls_sBuffer.clear();
			ls_sBuffer.append((char*)(&uID16),2);
			
			uID16 ^= 55555;

			ls_sBuffer.append((char*)(&uID16),2);

			uint32 uBufSize = (uint32)stBufSize;
			ls_sBuffer.append((char*)(&uBufSize), sizeof(uint32));

			if( stBufSize < 32 )
			{
				ls_sBuffer.append( (char*)(pBuf) , stBufSize );

				return ImpClass::Send( pConn , (void*)(ls_sBuffer.data()) , ls_sBuffer.size() );
			}
			else
			{
				if( !ImpClass::Send( pConn ,(void*)(ls_sBuffer.data()) , ls_sBuffer.size()) )
					return false;

				return ImpClass::Send( pConn , pBuf , stBufSize );
			}
		}
	};

	template<typename ConnType>
	class TPacker	
		:public TBasePacker<ConnType,TPacker<ConnType> >
	{
		friend class TBasePacker<ConnType,TPacker<ConnType> >;
	private:
		inline static bool Send(ConnType* pConn,void* pBuf,size_t stBufSize)
		{
			return pConn->Send(pBuf,stBufSize) !=0;
		}
	};

	template<>
	class TPacker<IPipe>
		:public TBasePacker<IPipe,TPacker<IPipe> >
	{
		friend class TBasePacker<IPipe,TPacker<IPipe> >;
	private:
		inline static bool Send(IPipe* pPipe,void* pBuf,size_t stBufSize)
		{
			size_t stSize=pPipe->Send(pBuf,stBufSize);

			if( stSize == stBufSize )
				return true;

			if( !pPipe->IsConnected() )
				return true;

			ostringstream strm;
			CAddress addr;
			pPipe->GetRemoteAddress(addr);
			strm<< "data size:" << stBufSize << endl
				<< "addr:"<<addr.GetAddress();

			LogErr( "RpcIPipe 发送缓冲区溢出，导致断线", strm.str() );
			pPipe->ShutDown( true );

			return false;
		}
	};

	template<typename ConnType,typename ImpClass>
	class TBaseUnPacker
		:public virtual CDynamicObject
	{
		friend class TUnPackerTable<ConnType>;
	public:
		HPACKAGE m_hHandle;
		UNPACK_FUNCTION m_pFun;

		// call by script
		int32 UnPack(ConnType* pConn)
		{
			Ast(pConn);

			const size_t stDataSize=pConn->GetRecvDataSize();

			if( stDataSize < (4 + sizeof(uint32)))
				return 0;

			char *pBuf = reinterpret_cast<char *>( pConn->GetRecvData() );			
			
			uint16 uId = *reinterpret_cast<uint16*>( pBuf );

			CBaseObject* pContext = static_cast<CBaseObject*>(pConn);
			
			const int32 nRetLen = m_pFun( uId, m_hHandle, reinterpret_cast<void *>(pContext), pBuf + 2, stDataSize -2 );

			switch(nRetLen)
			{
			case -1:
				ImpClass::ShutDownOnError(pConn);
				return -1;
			case -2:
				return 0;
			default:
				break;
			}

			const int32 nReadLen = nRetLen + 4 + sizeof(uint32);

			pConn->PopRecvData(nReadLen);

			return nReadLen;
		}
	};

	template<typename ConnType>
	class TUnPacker
		:public TBaseUnPacker<ConnType,TUnPacker<ConnType> >
	{
		friend class TBaseUnPacker<ConnType,TUnPacker<ConnType> >;
	private:
		static void ShutDownOnError(ConnType* pConn)
		{
			pConn->ShutDown("ConnRpc接受到无效数据，解包失败，连接断开");
		}
	};
	
	template<>
	class TUnPacker<IPipe>
		:public TBaseUnPacker<IPipe,TUnPacker<IPipe> >
	{
		friend class TBaseUnPacker<IPipe,TUnPacker<IPipe> >;
	private:
		static void ShutDownOnError(IPipe* pPipe)
		{
			LogErr( "PipeRpc接受到无效数据，解包失败，连接断开" );
			pPipe->ShutDown(true);
		}
	};


	template<typename ConnType>
	class TPackerTable
		:public virtual CDynamicObject
	{
	private:
		TPackerTable(){};
		~TPackerTable(){};
		map<string, TPacker<ConnType> >	m_mapPacker;
	public:
		static TPackerTable<ConnType>& Inst()
		{
			static TPackerTable<ConnType> s_Inst;
			return s_Inst;
		}

		TPacker<ConnType>& GetPacker(string szTableName)
		{
			return m_mapPacker[szTableName];
		}

		HPACKAGE Init(const char* szTableName)
		{
			REGIST_PACKFUNCTIONTABLE( *(CBaseScriptApp::Inst()->GetScript()), m_mapPacker[szTableName].m_hHandle, (PACK_FUNCTION)TPacker<ConnType>::_Pack, NULL, szTableName );
			return (m_mapPacker[szTableName].m_hHandle);
		}

		void RegistPackFunc( HPACKAGE hPackage, const char* szFunction, uint32 uID, const char* szFormat )
		{
			REGIST_PACKFUNCTION( *(CBaseScriptApp::Inst()->GetScript()), hPackage, szFunction, uID, true, szFormat );
		}
	};

	template<typename ConnType>
	class TUnPackerTable
		:public virtual CDynamicObject
	{
	private:
		TUnPackerTable(){};
		~TUnPackerTable(){};

		typedef map<string, TUnPacker<ConnType> > MapUnPacker;
		MapUnPacker	m_mapUnPacker;
	public:
		static TUnPackerTable<ConnType>& Inst()
		{ 
			static TUnPackerTable<ConnType> s_Inst;
			return s_Inst; 
		}

		TUnPacker<ConnType>& GetUnPacker(const char* szTableName)
		{
			string key = szTableName;
			typename MapUnPacker::iterator iter = m_mapUnPacker.find(key);
			Ast(iter!=m_mapUnPacker.end());

			return iter->second;
		}

		HPACKAGE Init(const char* szTableName)
		{
			REGIST_PACKCALLBACKTABLE( *(CBaseScriptApp::Inst()->GetScript()), m_mapUnPacker[szTableName].m_hHandle, &m_mapUnPacker[szTableName].m_pFun, szTableName );
			return (m_mapUnPacker[szTableName].m_hHandle);
		}
		void RegistUnPackFunc( HPACKAGE hPackage, const char* szFunction, uint32 uID, const char* szFormat )
		{
			REGIST_PACKCALLBACK( *(CBaseScriptApp::Inst()->GetScript()), hPackage, szFunction, uID, true, szFormat );
		}
	};


	template<typename RecvType,typename SendType>
	class TRegisterRpc
	{
	public:
		typedef TPackerTable<RecvType> CPackerTable;

		typedef TUnPacker<SendType> CUnPacker;
		typedef TUnPackerTable<SendType> CUnPackerTable;

		static int32  Register(CScript& Script, const char * szName)
		{
			//	CScript& Script = *(CBaseScriptApp::Inst()->GetScript());
			REGIST_ABSTRACT_B_CLASS( Script, CPackerTable);
			REGIST_CLASSFUNCTION(Script,CPackerTable,Init, "s:t");
			REGIST_CLASSFUNCTION(Script,CPackerTable,RegistPackFunc, "tsIs");
			REGIST_STATICFUNCTION( Script, CPackerTable, Inst, ":(CPackerTable)" );

			REGIST_ABSTRACT_B_CLASS( Script, CUnPacker);
			REGIST_CLASSFUNCTION(Script,CUnPacker,UnPack, (  ("("+string(szName)+"):i").c_str()   )    );

			REGIST_ABSTRACT_B_CLASS( Script, CUnPackerTable);
			REGIST_CLASSFUNCTION(Script,CUnPackerTable,Init, "s:t");
			REGIST_CLASSFUNCTION(Script,CUnPackerTable,RegistUnPackFunc, "tsIs");
			REGIST_CLASSFUNCTION( Script, CUnPackerTable, GetUnPacker, "s:(CUnPacker)" );
			REGIST_STATICFUNCTION( Script, CUnPackerTable, Inst, ":(CUnPackerTable)" );
			return 1;
		}
	};

#define CREATE_RPC_CHANNEL( ScriptClass, SendPipeName, RecvPipeName ) \
	TRegisterRpc< SendPipeName,RecvPipeName >::Register(ScriptClass, #RecvPipeName);

}

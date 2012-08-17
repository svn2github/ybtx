#include "stdafx.h"
#include "CQueryJob.h"
#include "IDatabase.h"
#include "ExpHelper.h"
#include "BaseHelper.h"
#include "CDbThreadMgr.h"
#include "CDbContext.h"
#include "CValueTable.h"
#include "CVariant.h"
#include "ErrLogHelper.h"
#include "CDbResultParser.h"
#include "TSqrAllocator.inl"
#include "CDbVariantBank.h"
#include "TObjectBank.inl"
#include <iomanip>

void CQueryJob::Init( CDbContext* pContext )
{
	Ast( !m_pContext );
	Ast( pContext );
	AdvAddRef(pContext);
	m_pContext	= pContext;
	m_pNext = NULL;
}

void CQueryJob::Unit( CDbThreadMgr* pMgr )
{
	if( m_pContext )
	{
		AdvDelRef(m_pContext);
		m_pContext=NULL;
	}
	DoUnit( pMgr );
}

CQueryJob::CQueryJob()
{
	m_pContext=NULL;
	m_pNext = NULL;
}

CQueryJob::~CQueryJob()
{
	AdvDelRef( m_pContext );
}

void CQueryJob::DoJob(void)
{
	SQR_TRY
	{
		RealDoJob();
	}
	SQR_CATCH(exp)
	{
		const char* szMsg=exp.ErrorMsg();
		if ( strstr( szMsg,"Deadlock found" ) )
		{
			GetContext()->SetReport("Deadlock found");
		}
		else if( strstr( szMsg,"MySQL server has gone away" ) )
		{
			GetContext()->SetReport("MySQL server has gone away");
		}
		else if( strstr( szMsg, "Commands out of sync; you can't run this command now" ) )
		{
			GetContext()->SetReport("Commands out of sync; you can't run this command now");
		}
		else
		{
			LogExp(exp);
			GetContext()->SetReport(exp.ErrorTitle());
		}
	}
	SQR_TRY_END;	
}

CDbContext* CQueryJob::GetContext()const
{
	return m_pContext;
}


//-----------------------------------------------------------------------------------------
void CQueryDummyJob::DoUnit( CDbThreadMgr* pMgr )
{
	delete this;
}

void CQueryDummyJob::RealDoJob(void)
{
	GenErr("CQueryDummyJob::RealDoJob:Shouldn't run this.");
}

IDbConnection* CQueryDummyJob::GetConnection()const
{
	GenErr("CQueryDummyJob::GetConnection:Shouldn't run this.");
	return NULL;
}



//-----------------------------------------------------------------------------------------
void CQueryConnectionJob::Init( CDbContext* pContext , IDbConnection* pConn )
{
	CQueryJob::Init( pContext );

	m_pConn = pConn;
}

IDbConnection* CQueryConnectionJob::GetConnection()const
{
	return m_pConn;
}


//-----------------------------------------------------------------------------------------

void CQueryBaseStatementJob::Init(IDbStatement* pStatement, CDbContext* pContext,vector<CVariant*> *pArgList,CDbThreadMgr* pMgr)
{
	CQueryJob::Init( pContext );

	m_ArgList.resize( pArgList->size() );

	m_pStatement=pStatement;

	vector<CVariant*>::const_iterator iter, end;
	iter = pArgList->begin();
	end = pArgList->end();

	CDbVariantBank* pStrBank	=pMgr->m_pStrVariantBank;
	CDbVariantBank* pBank	=pMgr->m_pVariantBank;

	m_stStrParamNum = 0;
	
	for(size_t i=0  ;iter != end ;++i)
	{
		const CVariant* pArgVariant=*iter++;

		CVariant *pVal;	
		
		
		EValueVariantType eType =pArgVariant->GetType();
		//如果是cstr类型的，我们必须用string进行保存，因为这个要运到多线程去处理的
		if (eType == eVVT_CStr)
		{			
			pVal = pStrBank->PopOrNew();
			++m_stStrParamNum;
			pVal->SetString(pArgVariant->GetCStr());
		}
		else
		{
			if( eType == eVVT_String )
			{
				pVal = pStrBank->PopOrNew();			
				++m_stStrParamNum;
			}
			else
			{
				pVal = pBank->PopOrNew();			
			}

			pVal->Assign(*pArgVariant);
		}
		m_ArgList[i] = pVal;
	}
}

void CQueryBaseStatementJob::DoUnit( CDbThreadMgr* pMgr )
{
	CDbVariantBank* pBank=pMgr->m_pVariantBank;
	CDbVariantBank* pStrBank=pMgr->m_pStrVariantBank;

	VecArgList_t::iterator iter, end;
	iter = m_ArgList.begin();
	end = m_ArgList.end();
	
	while (iter != end)
	{
		CVariant* pVariant=*iter++;

		if( pVariant->GetType() == eVVT_String )
		{
			pBank->Push(pVariant);
		}
		else
		{
			pStrBank->Push(pVariant);
		}
	}
	
	m_ArgList.clear();
}

inline size_t CQueryBaseStatementJob::GetStrParamNum()const
{
	return m_stStrParamNum;
}

IDbConnection* CQueryBaseStatementJob::GetConnection()const
{
	return GetStatement()->GetConnection();
}


static bool BindParameter(IDbParamBinder* pPb,const CVariant* pVariant,size_t nIndex , size_t* pLenBuffer)
{	
	static bool bIsNull = true;

	if( ! pVariant )
	{
		pPb->SetParamNull(&bIsNull, nIndex);
		return false;
	}

	EValueVariantType eType = pVariant->GetType();

	switch(eType)
	{
	case eVVT_Table:
		pPb->SetParamNull(&bIsNull, nIndex);
		break;
	case eVVT_Bool:
		pPb->SetParamInt8(reinterpret_cast<const int8*>(pVariant->GetNumberPtr<bool>()), nIndex);
		break;
	case eVVT_Int8:
		pPb->SetParamInt8(pVariant->GetNumberPtr<int8>(), nIndex);
		break;
	case eVVT_Uint8:
		pPb->SetParamUint8(pVariant->GetNumberPtr<uint8>(), nIndex);
		break;
	case eVVT_Int16:
		pPb->SetParamInt16(pVariant->GetNumberPtr<int16>(), nIndex);
		break;
	case eVVT_Uint16:
		pPb->SetParamUint16(pVariant->GetNumberPtr<uint16>(), nIndex);
		break;
	case eVVT_Int32:
		pPb->SetParamInt32(pVariant->GetNumberPtr<int32>(), nIndex);
		break;
	case eVVT_Uint32:
		pPb->SetParamUint32(pVariant->GetNumberPtr<uint32>(), nIndex);
		break;
	case eVVT_Int64:
		pPb->SetParamInt64(pVariant->GetNumberPtr<int64>(), nIndex);
		break;
	case eVVT_Uint64:
		pPb->SetParamUint64(pVariant->GetNumberPtr<uint64>(), nIndex);
		break;
	case eVVT_Float:
		pPb->SetParamFloat(pVariant->GetNumberPtr<float>(), nIndex);
		break;
	case eVVT_Double:
		pPb->SetParamDouble(pVariant->GetNumberPtr<double>(), nIndex);
		break;
	case eVVT_String:
		{
			const char *str = pVariant->GetString();			
			const size_t stLen=strlen(str);

			*pLenBuffer = stLen;
			
			pPb->SetParamText(str, stLen + 1, pLenBuffer, nIndex);
			return true;
		}
		break;
	default:
		{
			ostringstream strm;
			strm<<"Invalid variant type:"<<eType;
			GenErr("CQueryBaseStatementJob::BindStatementParam Error", strm.str().c_str());
		}
	}

	return false;
}

CQueryBaseStatementJob::VecArgList_t::const_iterator
CQueryBaseStatementJob::BindAllParam(IDbParamBinder* pPb,VecArgList_t::const_iterator iter,size_t* pLenBuffer)
{
	VecArgList_t::const_iterator end = m_ArgList.end();

	const size_t stParamNum = m_pStatement->GetParamNum();

	for( size_t i=0 ; i<stParamNum ; ++ i )
	{
		const CVariant* pVar = (iter==end) ? NULL : (*iter++);
		if( BindParameter(pPb, pVar, i,pLenBuffer) )
			++pLenBuffer;
	}

	pPb->Bind();

	return iter;
}

const CQueryBaseStatementJob::VecArgList_t& CQueryBaseStatementJob::GetArgList()const
{
	return m_ArgList;
}

IDbStatement* CQueryBaseStatementJob::GetStatement()const
{
	return m_pStatement;
}


//-------------------------------------------------------------------------------------------------------

void CQuerySingleStmtJob::Init(IDbStatement* pStatement, CDbContext* pContext, vector<CVariant*> *pArgList,CDbThreadMgr* pMgr)
{
	CQueryBaseStatementJob::Init( pStatement, pContext, pArgList, pMgr );
}

void CQuerySingleStmtJob::DoUnit( CDbThreadMgr* pMgr )
{
	CQueryBaseStatementJob::DoUnit( pMgr );
	pMgr->m_pBankSingleStmtJob->Push( this );
}

void CQuerySingleStmtJob::RealDoJob()
{
	CValueTable* pValueTable = NULL;

	IDbParamBinder *pPb=NULL;

	IDbStatement* pStmt=GetStatement();
	const size_t stParamNum = pStmt->GetParamNum();

	size_t* pLenBuffer = reinterpret_cast<size_t*>( alloca( sizeof( size_t ) * GetStrParamNum() ) );

	if( stParamNum>0 )
	{
		pPb= pStmt->CreateParamBinder();
		BindAllParam( pPb , GetArgList().begin(),pLenBuffer );
	}

	GetStatement()->Execute();

	if( pPb )
		pPb->Release();
	
	pValueTable = CDbResultParser::ParseStatementResult( GetStatement() );

	GetContext()->SetValueTable(pValueTable);
}


//-------------------------------------------------------------------------------------------------------

void CQueryBatchStmtJob::Init(IDbStatement* pStatement, CDbContext* pContext,uint32 uExecTime, vector<CVariant*> *pArgList,CDbThreadMgr* pMgr)
{
	CQueryBaseStatementJob::Init( pStatement, pContext, pArgList, pMgr );
	m_uExecTime = uExecTime;
}

void CQueryBatchStmtJob::DoUnit( CDbThreadMgr* pMgr )
{
	CQueryBaseStatementJob::DoUnit( pMgr );
	pMgr->m_pBankBatchStmtJob->Push( this );
}

void CQueryBatchStmtJob::RealDoJob()
{
	IDbParamBinder *pPb = GetStatement()->CreateParamBinder();

	VecArgList_t::const_iterator iter, end;

	const VecArgList_t& ArgList=GetArgList();
	iter = ArgList.begin();
	end = ArgList.end();

	size_t* pLenBuffer = reinterpret_cast<size_t*>( alloca( sizeof( size_t ) * GetStrParamNum() ) );

	for( uint32 i=0; i<m_uExecTime; ++ i )
	{		
		iter = BindAllParam( pPb , iter, pLenBuffer );

		GetStatement()->Execute();
	}

	pPb->Release();
}




//	CQueryTextJob
void CQueryTextJob::Init(IDbConnection* pConnection, const char* szSql, unsigned uSize, CDbContext* pContext)
{
	CQueryConnectionJob::Init( pContext, pConnection );
	m_sCmd.assign(static_cast<const char*>(szSql),uSize);
}


void CQueryTextJob::DoUnit( CDbThreadMgr* pMgr )
{
	pMgr->m_pBankTextJob->Push( this );
}


void CQueryTextJob::RealDoJob()
{
	CValueTable* pValueTable = NULL;

	IDbTextResult*	pTextResult= GetConnection()->Execute(m_sCmd.c_str());

	pValueTable = CDbResultParser::ParseTextResult(pTextResult);	
	GetContext()->SetValueTable(pValueTable);
}


//--------------------------------------------------------

void CCommitTransJob::Init(IDbConnection* pConnection, CDbContext* pContext)
{
	CQueryConnectionJob::Init( pContext ,pConnection );
}

void CCommitTransJob::DoUnit( CDbThreadMgr* pMgr )
{
	pMgr->m_pBankCommitTransJob->Push( this );
}


void CCommitTransJob::RealDoJob()
{
	GetConnection()->CommitTrans();
}

void CRollbackTransJob::Init(IDbConnection* pConnection, CDbContext* pContext)
{
	CQueryConnectionJob::Init( pContext,pConnection );
}

void CRollbackTransJob::DoUnit( CDbThreadMgr* pMgr )
{
	pMgr->m_pBankRollbackTransJob->Push( this );
}


void CRollbackTransJob::RealDoJob()
{
	GetConnection()->RollbackTrans();
}

//
void CSetSavepointJob::Init(IDbConnection* pConnection, CDbContext* pContext, const char* szName)
{
	CQueryConnectionJob::Init( pContext, pConnection );
	m_sCmd.assign(static_cast<const char*>(szName), strlen(szName));
}


void CSetSavepointJob::DoUnit( CDbThreadMgr* pMgr )
{
	pMgr->m_pBankSetSavepointJob->Push( this );
}



void CSetSavepointJob::RealDoJob()
{
	GetConnection()->SetSavepoint(m_sCmd.c_str());
}

//
void CRollbackTransToJob::Init(IDbConnection* pConnection, CDbContext* pContext, const char* szName)
{
	CQueryConnectionJob::Init( pContext, pConnection );
	m_sCmd.assign(static_cast<const char*>(szName), strlen(szName));
}


void CRollbackTransToJob::DoUnit( CDbThreadMgr* pMgr )
{
	pMgr->m_pBankRollbackTransToJob->Push( this );
}


void CRollbackTransToJob::RealDoJob()
{
	GetConnection()->RollbackTransTo(m_sCmd.c_str());
}

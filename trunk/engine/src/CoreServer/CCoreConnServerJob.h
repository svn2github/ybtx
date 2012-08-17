#pragma once

#include "CCoreServerJob.h"
#include "CAddress.h"

namespace sqr
{
	class IMulticast;

	class CCoreBeginServiceJob
		:public CCoreServerJob
	{
	public:
		CCoreBeginServiceJob(const CAddress& addr,uint32 uSendBufferSize,uint32 uRecvBufferSize);
		
		~CCoreBeginServiceJob();
	private:
		char*	m_sAddress;
		uint32	m_uPort;
		uint32 m_uSendBufferSize;
		uint32 m_uRecvBufferSize;
	};

	class CCoreEndServiceJob
		:public CCoreServerJob
	{
	public:
		CCoreEndServiceJob();
		
		~CCoreEndServiceJob();
	};

	class CCoreSendMsgJob
		:public CCoreServerJob
	{
	public:
		CCoreSendMsgJob(const void* pData,uint32 uSize);
		
		~CCoreSendMsgJob();
	private:
		uint32				m_uSize;
		void*				m_pData;
	};

	class CCoreSendErrMsgToAllConnJob : public CCoreServerJob
	{
	public:
		CCoreSendErrMsgToAllConnJob(const char* szError);
		~CCoreSendErrMsgToAllConnJob();
	private:
		uint32	m_uLen;
		char*	m_sError;
	};

	class CCoreSendErrMsgToConnJob
		:public CCoreServerJob
	{
	public:
		CCoreSendErrMsgToConnJob(uint32 uId, const char* szError);
		
		~CCoreSendErrMsgToConnJob();
	private:
		uint32				m_uId;
		uint32				m_uLen;
		char*				m_sError;
		
	};

	class CCoreConnServerSendJob
		:public CCoreServerJob
	{
	public:
		CCoreConnServerSendJob(uint32 uConnId, const void* pData, uint32 uSize);
		
		~CCoreConnServerSendJob();
	private:
		uint32				m_uConnID;
		uint32				m_uSize;
		void*				m_pData;
	};

	
	class CCoreShutDownJob
		:public CCoreServerJob
	{
	public:
		CCoreShutDownJob(uint32 uConnID, const char* szMsg);
		
		~CCoreShutDownJob();
	private:
		uint32		m_uConnID;
		char*		m_sMsg;
	};

	class CCoreLogOnOffLineMsgJob
		:public CCoreServerJob
	{
	public:
		CCoreLogOnOffLineMsgJob(uint32 uConnId, const char* msg);
		
		~CCoreLogOnOffLineMsgJob();
	private:
		uint32			m_uConnID;
		char*			m_sMsg;
	};


	class CCoreMultiMsgSenderDoSendJob
		:public CCoreServerJob
	{
	public:
		CCoreMultiMsgSenderDoSendJob(uint32 uMultiMsgSenderID, const void* pData, uint32 uSize);
		
		~CCoreMultiMsgSenderDoSendJob();
	private:
		uint32					m_uMultiMsgSenderID;
		uint32					m_uSize;
		void*					m_pData;
	};

	class CCoreMultiMsgSenderAddPipeJob
		:public CCoreServerJob
	{
	public:
		CCoreMultiMsgSenderAddPipeJob(uint32 uMultiMsgSenderID, uint32 uConnID);
		
		~CCoreMultiMsgSenderAddPipeJob();
	private:
		uint32			m_uMultiMsgSenderID;
		uint32			m_uConnID;
	};

	class CCoreMultiMsgSenderDelPipeJob
		:public CCoreServerJob
	{
	public:
		CCoreMultiMsgSenderDelPipeJob(uint32 uMultiMsgSenderID, uint32 uConnID);
		
		~CCoreMultiMsgSenderDelPipeJob();
	private:
		uint32			m_uMultiMsgSenderID;
		uint32			m_uConnID;
	};

	class CCoreCreateSynMultiMsgSenderJob
		:public CCoreServerJob
	{
	public:
		CCoreCreateSynMultiMsgSenderJob(uint32 uMultiMsgSenderID);
		
		~CCoreCreateSynMultiMsgSenderJob();
	private:
		uint32				m_uMultiMsgSenderID;
	};

	class CCoreMultiMsgSenderDestoryJob
		:public CCoreServerJob
	{
	public:
		CCoreMultiMsgSenderDestoryJob(uint32 uMultiMsgSenderID);
		
		~CCoreMultiMsgSenderDestoryJob();
	private:
		uint32				m_uMultiMsgSenderID;
	};

	class CCoreConnSetValueJob : public CCoreServerJob
	{
	public:
		CCoreConnSetValueJob(uint32 uConnID, const char* szKey, const char* szValue);
		~CCoreConnSetValueJob();

	private:
		uint32	m_uConnID;
		char*	m_szKey;
		char*	m_szValue;
	};
}



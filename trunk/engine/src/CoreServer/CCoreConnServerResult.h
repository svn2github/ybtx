#pragma once

#include "CCoreServerResult.h"
#include "CAddress.h"

namespace sqr
{

	class CCoreDispatchShellMsgResult 
		: public CCoreServerResult
	{
	public:
		CCoreDispatchShellMsgResult(uint32 uConnId, const void* pData, size_t stSize);
		~CCoreDispatchShellMsgResult();

	private:
		uint32			m_uConnId;
		const void*			m_pData;
		size_t			m_uSize;
	};

	class CCoreCreateConnServerResult
		:public CCoreServerResult
	{
	public:
		CCoreCreateConnServerResult(uint32 uConnID, CAddress& address);
		~CCoreCreateConnServerResult();
	private:
		uint32		m_uConnID;
		char*		m_sAddress;
		uint32		m_uPort;
	};

	class CCoreDestoryConnServerResult
		:public CCoreServerResult
	{
	public:
		CCoreDestoryConnServerResult(uint32 uConnID);
		~CCoreDestoryConnServerResult();
	private:
		uint32			m_uConnID;
	};

	class CCoreOnDisconnectResult
		:public CCoreServerResult
	{
	public:
		CCoreOnDisconnectResult(uint32 uConnID);
		~CCoreOnDisconnectResult();
	private:
		uint32		m_uConnId;
	};

	class CCoreOnNewConnAcceptedResult
		:public CCoreServerResult
	{
	public:
		CCoreOnNewConnAcceptedResult(uint32 uConnID);
		~CCoreOnNewConnAcceptedResult();
	private:
		uint32			m_uConnID;
	};

	class CCoreBeginServiceResult
		:public CCoreServerResult
	{
	public:
		CCoreBeginServiceResult(bool bSuccess, const char* szErrMsg);
		~CCoreBeginServiceResult();
	private:
		bool		m_bSuccess;
		char*		m_sError;
	};

	class CCoreEndServiceResult
		:public CCoreServerResult
	{
	public:
		CCoreEndServiceResult();
		~CCoreEndServiceResult();
	};

	class CCoreCheatResult
		:public CCoreServerResult
	{
	public:
		CCoreCheatResult(uint32 uConnID);
		~CCoreCheatResult();
	private:
		uint32			m_uConnID;
	};

	class CCoreConnSetValueResult : public CCoreServerResult
	{
	public:
		CCoreConnSetValueResult(uint32 uConnID, const char* szKey, const char* szValue);
		~CCoreConnSetValueResult();

	private:
		uint32	m_uConnID;
		char*	m_szKey;
		char*	m_szValue;
	};

	class CCoreConnSetLatency : public CCoreServerResult
	{
	public:
		CCoreConnSetLatency(uint32 uConnID, uint16 uLatency);
		~CCoreConnSetLatency();
	private:
		uint32 m_uConnID;
		uint16 m_uLantency;
	};

	class  CCoreSetTestingConnResult : public CCoreServerResult
	{
	public:
		CCoreSetTestingConnResult(uint32 uConnID, bool bTestingConn);
		~CCoreSetTestingConnResult();
	private:
		uint32 m_uConnID;
		bool m_bTestingConn;
	};
}

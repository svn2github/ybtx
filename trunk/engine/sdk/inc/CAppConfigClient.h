#pragma once
#include "TAppConfig.h"
#include "CTraitsClient.h"

namespace sqr
{
	class CIniFile;
	class CSceneConfigClient;

	class CAppConfigClient
		: public TAppConfig<CTraitsClient>
	{
	private:
		typedef TAppConfig<CTraitsClient> ParentConfig_t;

		uint32	m_uSkipFrameWaitTime;
		uint32	m_uRenderCycInBackground;
		wstring	m_strLogPath;

		CIniFile* m_pIniFile;

		bool	m_bTestingClient;
		bool	m_bErrMsgShow;
		string	m_bLogUploadFaultMsg;
		bool	m_bAntiDebug;
		wstring	m_strAppIconFileName;
		bool	m_bReload;

		CSceneConfigClient*	m_pSceneConfig;

	public:
		CAppConfigClient(istream& strmConfig, const string& SceneConfig );
		~CAppConfigClient(void);

		bool	HandleSE()const;
			
		uint32	GetRenderCycInBackground()const;
		uint32	GetRenderCycInForground()const;
		void	SetRenderCycInForground( uint32 cyc );
		uint32	GetDefaultWndWidth()	const;
		uint32	GetDefaultWndHeight()	const;
		
		
		uint32	GetDefaultRenderCyc()	const;		
		uint32	GetDefaultLanguage()	const;		
				
		string	GetCoreDumpMsg()const;
		string	GetGuiRes()const;
		string	GetGuiFxPath() const;
		string GetEmtRes()const;

		wstring GetWindowTitle()const;
		string GetGTPortName()const;
		uint32 GetSkipFrameWaitTime()const;
		//------------------------------------------
		const wstring& GetLogPath() const;
	
		//设置是否为测试客户端
		bool	IsTestClient() const;
		bool	BeErrMsgShow() const;
		void	SetTestClientEnabled(bool bEnabled);
		void	SetErrMsgShowEnabled(bool bEnabled);	

		//上传log文件失败提示信息
		const string& GetLogUploadFaultMsg() const;

		//ybtx game icon file name
		const wchar_t* GetAppIconFileName() const;

		bool	IsAntiDebug() const;

		/// 从ini加载配置, 没有的就以xml配置的默认值为准

		bool GetReload() const;
		
	};

	inline const wstring& CAppConfigClient::GetLogPath() const
	{
		return m_strLogPath;
	}
	
	//------------------------------------------------------------------------------
	inline bool CAppConfigClient::HandleSE()const
	{
		return !GetCoreDumpMsg().empty();
	}

	//------------------------------------------------------------------------------
	inline uint32 CAppConfigClient::GetSkipFrameWaitTime()const
	{
		return m_uSkipFrameWaitTime;
	}
	//------------------------------------------------------------------------------
	inline bool CAppConfigClient::IsTestClient() const
	{
		return m_bTestingClient;
	}

	inline void CAppConfigClient::SetTestClientEnabled(bool bEnabled)
	{
		m_bTestingClient = bEnabled;
	}

	//------------------------------------------------------------------------------
	inline bool CAppConfigClient::BeErrMsgShow() const
	{
		return m_bErrMsgShow;
	}

	inline void CAppConfigClient::SetErrMsgShowEnabled(bool bEnabled)
	{
		m_bErrMsgShow = bEnabled;
	}

	//------------------------------------------------------------------------------
	inline const wchar_t*
		CAppConfigClient::GetAppIconFileName() const
	{
		return m_strAppIconFileName.c_str();
	}

	//------------------------------------------------------------------------------
	inline uint32
		CAppConfigClient::GetRenderCycInBackground()const
	{
		return m_uRenderCycInBackground;
	}

}// namespace sqr

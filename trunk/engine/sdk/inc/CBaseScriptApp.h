#pragma once
#include "CDynamicObject.h"
#include "ThreadTypes.h"

namespace sqr
{
	class CScript;
	class CPathMgr;
	class CXmlConfig;

	class CBaseScriptApp
		:public virtual CDynamicObject
	{
	public:
		CScript*				GetScript();
		CScript*				GetMainVM();
		const char*				GetCfgFilePath(const char* szRelaFilePath)const;
		const char*				GetRootFilePath(const char* szRelaFilePath)const;
		const char*				GetXMLFilePath(const char* szRelaFilePath)const;
		const wchar_t*			GetLangFilePath();
		const wchar_t*			GetGUIFilePath();

		bool					GetIsCfgPkgUsed();

		/**
		\brief
			用于支持注册给lua的 U8函数（用于转换mbcs字符串到utf8字符串）
		*/
		static void				InitCfgTableTlsValue();
		static void				UnInitCfgTableTlsValue();
		static string*			GetCfgTableTlsData();

		static void				InitStrIndexTlsValue();
		static void				UnInitStrIndexTlsValue();
		static string*			GetStrIndexTlsData();

		static void				InitStr64IndexTlsValue();
		static void				UnInitStr64IndexTlsValue();
		static string*			GetStr64IndexTlsData();
		static CBaseScriptApp*	Inst();

	protected:
		CBaseScriptApp(void);
		~CBaseScriptApp(void);
		int32					RegisterModule(CScript& Script);
		int32					RegisterCommon(CScript& Script);
		int32					RegisterScene(CScript& Script);
		int32					RegisterNetwork(CScript& Script);
		int32					RegisterCoreCommon(CScript& Script);
		int32					RegisterCoreServer(CScript& Script);
		int32					RegisterCoreStress(CScript& Script);
		int32					RegisterCoreScriptCommon(CScript& Script);
		int32					RegisterCoreScriptServer(CScript& Script);

#ifdef _WIN32
		int32					RegisterGraphic(CScript& Script);
		int32					RegisterGUI(CScript& Script);
		int32					RegisterThink(CScript& Script);
		int32					RegisterCoreClient(CScript& Script);
		int32					RegisterCoreScriptClient(CScript& Script);
#endif

		CScript*				CreateMainVM(float fCallbackGCRatio,float fNormalUnpackGCRatio,float fSerialUnpackGCRatio);
		bool					PkgFileUsed();

		void					AddScriptLoadPath(const char* szAliasPath, const char* szPath);
		void					SetScriptLoadPath(CScript* pScript);

		static void				InitCfgTableTlsKey();
		static void				UnInitCfgTableTlsKey();

		static void				InitStrIndexTlsKey();
		static void				UnInitStrIndexTlsKey();

		static void				InitStr64IndexTlsKey();
		static void				UnInitStr64IndexTlsKey();

	private:
		void					ReleaseMainVM();
		void					SetPkgFileNameMapping();
		void					LoadPkgFileConfig();
		CXmlConfig*				LoadCfgFile(const string& cfg_file_name, const string& root_name);
		void					LoadScriptSearchPathXml();
		void					ConfigSearchPath();
		void					CheckIsPkgFileUsed();
		void					ChangeCurPathToRes();

		void					AddLoadPath(const string& szAliasPath, const string& szPath, bool UseScript);

		static void				AddRefFun(CBaseObject* pObj);
		static void				DelRefFun(CBaseObject* pObj);
		static void				DisbindFun(CBaseObject* pObj);

		static void				AddPvRefFun(void* pObj);
		static void				DelPvRefFun(void* pObj);
		static void				DisbindPvFun(void* pObj);

		static void*			GetCallBackData(CBaseObject* pObject, const char* szFunName);
		static void				GetCallBackAttr(void* pCall, size_t& stBufSize, size_t*& pParamOffse, size_t& iRetOffset);
		static bool				CallClassCallBack(void* pCall, CBaseObject* pObject, char* pDataBuf);
	
	// ---- data member ---------------------------------------------------------------------------

	protected:
		bool					m_bCfgPkgFileUsed;
		bool					m_bPkgFileUsed;
		mutable string			m_szCfgFilePath;
		mutable string			m_szRootFilePath;
		mutable string			m_szXMLFilePath;

		static HTLSKEY			m_tlsCfgKey;
		static HTLSKEY			m_tlsStrIndexKey;		//MemH16函数使用通过字符串得到内存16进制串
		static HTLSKEY			m_tlsStr64IndexKey;		//MemH16函数使用通过字符串得到内存64进制串
		static const wstring	LANGUAGE_FOLDER_NAME;	

		map<string, string>		m_ScriptLoadPath;

	private:
		static CBaseScriptApp*	ms_pInst;

		//启动路径
		CPathMgr*				m_pRootPathMgr;
		CPathMgr*				m_pCfgPathMgr;
		CPathMgr*				m_pXMLPathMgr;
		CPathMgr*				m_pLangPathMgr;	
		CPathMgr*				m_pGUIPathMgr;
		CPathMgr*				m_pResPathMgr;
	};

}


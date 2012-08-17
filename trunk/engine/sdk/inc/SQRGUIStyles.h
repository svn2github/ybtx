#pragma once
#include "GUIBase.h"
#include "GUIXmlResSaver.h"
namespace sqr
{
	class SQRGUIStyle 
		: public SQRGuiMallocObject
		, public GUIXmlResSaverClass
	{
	public:
		SQRGUIStyle(const string& Name,const string& Path);
		~SQRGUIStyle();
	public:
		typedef	UIMap<UIString,StyleStruct*>	RectMap; 

		bool Initialize(void);
		void Release(void);
		void Save(void);		
	
		inline const char* GetStyleName(void)		{ return m_StyleName.c_str();	}
		inline const char* GetPath(void)			{ return m_strPath.c_str();		}
		inline const char* GetTexturePath(void)		{ return m_strTexture.c_str();	}
		
		StyleStruct*	GetStyleStruct(const string& str);
		StyleStruct*	SetStyleStruct(const string& str , const StyleStruct& Style);
		
		inline RectMap::iterator Begin(void)	{ return m_mapRect.begin(); };
		inline RectMap::iterator End(void)		{ return m_mapRect.end(); };
	private:
		// 整理无符号整形转换后的字符串
		void ReBuiltU32Str(string& val);

	protected:
		//我时常在想这样会不会太消耗，暂时先这样 回头优化，一时间也没有更好的办法
		RectMap		m_mapRect;
		bool		m_isLoad;
		UIString		m_StyleName;
		UIString		m_strPath;
		UIString		m_strTexture;
		UIString		m_strStylePath;
	};

	class SQRGUIStyleManager 
		: public Singleton<SQRGUIStyleManager>
		, public SQRGuiMallocObject
	{
	public:
		SQRGUIStyleManager();
		~SQRGUIStyleManager();

		SQRGUIStyle* GetStyle(const string& sName);
		SQRGUIStyle* AddStyle(const string& sName);
		SQRGUIStyle* AddStyle(const string& sName,const string& sPath);
		bool	   DelStyle(const string& sName);

		SQRGUIStyle*		ActiveStyle(const string& sName);
		
		inline SQRGUIStyle*		GetActiveStyle(void);
		inline const char*		GetActiveName(void);
		inline const char*		GetXMLFileName(void);
		
		bool	Load(const string& fName);
		bool	Save(const string& fName);
		inline	bool	Save(void); //保存至读入的文件

		void	Release();
	public: //编辑常规功能
		typedef UIMap<UIString,SQRGUIStyle*> STYLE_MAP;
		
		inline UINT	GetSize(void)				{ return  m_Styles.size();	}
		inline STYLE_MAP::iterator	Begin(void)	{ return m_Styles.begin();	}
		inline STYLE_MAP::iterator	End(void)	{ return m_Styles.end();	}

	protected:		
		STYLE_MAP	m_Styles;
		SQRGUIStyle*	m_pActiveStyle;
		UIString		m_ActiveStyleName;
		UIString		m_XMLFileName;
	};

	inline const char* SQRGUIStyleManager::GetXMLFileName(void)
	{
		return m_XMLFileName.c_str();
	}

	inline const char* SQRGUIStyleManager::GetActiveName(void)
	{
		return m_ActiveStyleName.c_str();
	}

	inline SQRGUIStyle* SQRGUIStyleManager::GetActiveStyle(void)
	{ 
		return m_pActiveStyle; 
	}

	inline bool    SQRGUIStyleManager::Save(void)
	{ 
		return Save(m_XMLFileName.c_str()); 
	}
}
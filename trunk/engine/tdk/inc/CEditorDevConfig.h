#pragma once
#include "TSingleton.h"

class TiXmlNode;
namespace sqr
{
	class CXmlConfig;
	class CEditorDevConfig : public Singleton<CEditorDevConfig>
	{
		bool		m_coreDumpIsEnabled;
		bool		m_errMsgIsShow;
		CXmlConfig*	m_pXmlConfig;
	public:
		CEditorDevConfig();
		~CEditorDevConfig(void);

		void DefaultDevConfig();

		void LoadDevConfig(TiXmlNode* Node);
		void SaveDevConfig(TiXmlNode* Node);

		inline bool GetCoreDumpIsEnabled() const
		{
			return m_coreDumpIsEnabled;
		}

		inline bool GetErrMsgIsShow() const
		{
			return m_errMsgIsShow;
		}

	};

}

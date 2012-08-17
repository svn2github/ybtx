#pragma once

namespace sqr_tools
{
	enum PROPTYPE
	{
		PROP_SCENE,
		PROP_MODEL,
		PROP_LIGHT,
		PROP_AUDIO,
		PROP_BASE
	};

	class CEditContext;
	class CBasePropCtrlImp
	{
	public:
		CBasePropCtrlImp(CEditContext* context=NULL);
		~CBasePropCtrlImp(void);

		virtual void SetContext(CEditContext* context);

	protected:
		CEditContext*	m_pOwnContext;
		PROPTYPE		m_PropType;		
	};

//!namespace
}
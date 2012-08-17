#pragma once
#include "CEditorConfig.h"
#include "CRendererBase.h"
#include "CEditModel.h"
class	CPortraitConfig;

namespace sqr_tools
{
	class   CPortraitViewCtrlImp;
	class CPortraitView 
		: public CRendererBase
	{
		friend class	::CPortraitConfig;
		friend class	::CPortraitViewCtrlImp;
	public:
		CPortraitView( CWindowTarget* pTarget );
		~CPortraitView();

		void Render();

		void SetRect(CIRect& Rect);
		void SetVisible(bool bVis);
		void SetFullScreen(bool bScreen);
		void SetModel( CEditModel* pModel );
		void			ShowSettingDlg(void);
		CLightSystem*	GetLightSystem(void);
	protected:
		bool			Initialize(void);
		void			UpdateRect(void);

		RenderParam			m_Rp;

		CIRect				m_PortraitRect;
		CRenderOperation	m_Op;


		CWindowTarget*		m_pParentTarget;
		CRenderGroup*		m_pParentGroup;

		CEditModel*			m_pRenderModel;
		CViewport*			m_pViewPort;


		CRenderGroup*		m_pRenderGroup;
		string				m_strPhotoFrame;
		
		bool				m_bVisible;
		bool				m_bFullScreen;
		bool				m_bShowFrame;
		//string				m_szConfigType;
		//TiXmlNode*			m_config_xml;
	};
}
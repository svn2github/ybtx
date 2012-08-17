#pragma once
#include "SQRButton.h"

namespace sqr
{
	class GUI_API SQRItemWnd : public SQRButton
	{
		DYNAMIC_DECLARE_WND( SQRItemWnd )

	protected:
		/*WND_RENDER_OBJ			m_RenderObject;*/
		IDNAME					m_DisableAni;
		IDNAME					m_EnableAni;
		IDNAME					m_MouseOverAni;
		IDNAME					m_ClickDownAni;
		IDNAME					m_CurAni;

		bool					m_bSel;					// 是否被选中
		int32					m_bkeyboard;			// TODO: 该状态值magic number不太明白，需要整理

	public:
		SQRItemWnd(void);
		virtual ~SQRItemWnd(void);
		virtual int32			Create( const WndCreateParam& param );

		/*virtual WND_RENDER_OBJ*	GetRenderObject();*/

		virtual const char*		GetRenderObjAni( const IMAGE_PARAM& ImageFlag );
		virtual void			SetRenderObjAni( const IMAGE_PARAM& ImageFlag, const char* szAni );
		virtual void			DrawRect( WND_IMAGE_LIST& ImageList, const CFRect* rt, float offsetX, float offsetY);

		void					SetSelected( bool bSel );
		void					SetSelectedByClick( bool bSel );
		bool					IsSelected() const;
		void					SetHighLight()
		{
			m_bkeyboard = 1;
		}
		void					CancelHighLight()
		{
			m_bkeyboard = 0;
		}
		void					CancelMouseMove()
		{
			m_bkeyboard = 3;
		}
		int8					GetHighLightState()
		{
			return static_cast<int8>(m_bkeyboard);
		}

	protected:
		virtual void			OnLButtonUp( uint32 nFlags, int32 x, int32 y );
		virtual void			OnLButtonClick( uint32 nFlags, int32 x, int32 y );
		virtual void			DrawWndBackground();

	};
}
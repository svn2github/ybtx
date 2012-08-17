#pragma once
#include "TSingleton.h"
#include "CGraphicMallocObject.h"
#include "CSceneDebugHelper.h"
#include "CRendererBase.h"

namespace sqr
{
	extern void DebugText(uint color, const char* fmt, ...);

	class CRenderFont;

	class CDebugText 
		: public CRendererBase
		, public Singleton<CDebugText>
	{
	public:
		CDebugText();
		~CDebugText();

		bool	Open();
		void	Close();
		void	Render();
		void	DrawDebugText( const string& text, DWORD color = 0xffffffff );
		void	DrawDebugText( DWORD color, const char* format, ... );

	private:
		CRenderFont*	m_font;

		struct TextUnit
		{
			GString text;
			DWORD color;
		};
		typedef SVector<TextUnit> TextBuffer;

		TextBuffer textBuffer;
	};
}

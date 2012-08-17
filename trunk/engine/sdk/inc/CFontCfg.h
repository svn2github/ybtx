#pragma once
#include "TSingleton.h"

namespace sqr
{
	struct GDIFontCfgNode
	{
		string		font_file;
		wstring		font_name;
	};

	class CFontCfg : public Singleton<CFontCfg>
	{
	public:
		bool	LoadFongConfig( const string& szLanFolder );

		const char*		
				GetDisFontName();

		size_t	GetGDIFontCount() const;

		float	GetFontZoom() const;
		
		const wchar_t*	
				GetGDIFontName( size_t font_id );
		
		const char*		
				GetGDIFontFileName( size_t font_id );
		
		const wchar_t*	
				GetGDIDefFontName();

		void	UseDefault();

		static const string FONT_CFG_ROOT;
		static const string GDI_FONT_NODE;
		static const string DEFAULT_FONT_NODE;
		static const string DISTANCE_FONT_NODE;
		static const string FONT_SIZE_NODE;
		static const string FONT_ZOOM;
		static const string FONT_NAME;
		static const string FONT_FILE;
		static const string FONT_XML_FILEPATH;

	private:
		typedef vector<GDIFontCfgNode> FontCfgNodeList;

		string			m_szDisFontName;
		wstring			m_szDefaultFontName;
		FontCfgNodeList	m_GDIFontNodeList;
		float			m_fFontZoom;
	};
}


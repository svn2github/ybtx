#include "stdafx.h"
#include "CFontCfg.h"
#include "CGraphicExp.h"
#include "PkgStream.h"
#include "CodeCvs.h"
#include "CXmlConfig.inl"
#include "TableFileHelper.h"

namespace sqr
{
	const string CFontCfg::FONT_CFG_ROOT		= "FontConfig";
	const string CFontCfg::GDI_FONT_NODE		= "Font";
	const string CFontCfg::DEFAULT_FONT_NODE	= "DefaultFont";
	const string CFontCfg::DISTANCE_FONT_NODE	= "DistanceFont";
	const string CFontCfg::FONT_SIZE_NODE		= "FontSize";
	const string CFontCfg::FONT_ZOOM			= "zoom";
	const string CFontCfg::FONT_NAME			= "name";
	const string CFontCfg::FONT_FILE			= "file";
	const string CFontCfg::FONT_XML_FILEPATH	= "/fonts/FontConfig.xml";

	bool CFontCfg::LoadFongConfig( const string& szLanFolder )
	{
		SQR_TRY
		{
			string szCfgPath = szLanFolder + FONT_XML_FILEPATH;
			m_GDIFontNodeList.clear();

			ipkgstream file_strm;
			file_strm.open(L"lan", szCfgPath.c_str(), true, eRO_File);
			if (!file_strm)
			{
				szCfgPath = GetOriLangPath() + FONT_XML_FILEPATH;
				file_strm.open(L"lan", szCfgPath.c_str(), true, eRO_File);
				if (!file_strm)
				{
					GenErr("字体配置文件打开失败", string("Can't open ") + szCfgPath);
				}
			}

			CXmlConfig *pFontXmlCfg = new CXmlConfig(FONT_CFG_ROOT.c_str(), file_strm);
			file_strm.close();

			TiXmlNode* pGDIFontNode = pFontXmlCfg->Get<TiXmlElement*>(GDI_FONT_NODE.c_str());
			while ( pGDIFontNode )
			{
				GDIFontCfgNode newFontNode;
				newFontNode.font_name = utf8_to_utf16( pGDIFontNode->ToElement()->Attribute(FONT_NAME.c_str()) ).c_str();
				newFontNode.font_file = pGDIFontNode->ToElement()->Attribute(FONT_FILE.c_str());
				m_GDIFontNodeList.push_back(newFontNode);

				pGDIFontNode = pGDIFontNode->NextSibling(GDI_FONT_NODE.c_str());
			}

			m_szDefaultFontName = utf8_to_utf16(pFontXmlCfg->AttribGet<string>(DEFAULT_FONT_NODE.c_str(), FONT_NAME.c_str()));
			m_szDisFontName		= pFontXmlCfg->AttribGet<string>(DISTANCE_FONT_NODE.c_str(), FONT_FILE.c_str());
			m_fFontZoom			= pFontXmlCfg->AttribGet<float>(FONT_SIZE_NODE.c_str(), FONT_ZOOM.c_str());
			delete pFontXmlCfg;
			pFontXmlCfg = NULL;
		}
		SQR_CATCH(exp)
		{
			GfkLogExp(exp);
		}
		SQR_TRY_END

		return true;
	}

	const char* CFontCfg::GetDisFontName()
	{
		return m_szDisFontName.c_str();
	}

	size_t CFontCfg::GetGDIFontCount() const
	{
		return m_GDIFontNodeList.size();
	}

	float CFontCfg::GetFontZoom() const
	{
		return m_fFontZoom;
	}

	const wchar_t* CFontCfg::GetGDIFontName( size_t font_id )
	{
		return m_GDIFontNodeList[font_id].font_name.c_str();
	}

	const char* CFontCfg::GetGDIFontFileName( size_t font_id )
	{
		return m_GDIFontNodeList[font_id].font_file.c_str();
	}

	const wchar_t* CFontCfg::GetGDIDefFontName()
	{
		return m_szDefaultFontName.c_str();
	}

	void CFontCfg::UseDefault()
	{
		GDIFontCfgNode newFontNode;
		newFontNode.font_name = L"宋体";
		newFontNode.font_file = "simsun.ttc";
		m_GDIFontNodeList.push_back(newFontNode);
		m_GDIFontNodeList.push_back(newFontNode);
		m_GDIFontNodeList.push_back(newFontNode);
		m_GDIFontNodeList.push_back(newFontNode);

		m_szDefaultFontName = L"宋体";
		m_szDisFontName		= "dis_youyuan";
		m_fFontZoom			= 1.0f;
	}
}

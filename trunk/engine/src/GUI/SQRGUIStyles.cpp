#include "stdafx.h"
#include "SQRGUIStyles.h"
#include "BaseHelper.h"
#include "tinyxml.h"
#include "PkgStream.h"
#include "CGraphic.h"
#include "SQRGUIManager.h"
#include "GUILayoutHandler.h"
#include "StringHelper.h"
#define UPDATE_WND(class_name)\
	class_name::Style##class_name = GetStyleStruct(class_name::class##class_name.m_ClassName.c_str());
//GUI列表
#include "SQRDialog.h"
#include "SQRButton.h"
#include "SQRCheckButton.h"
#include "SQRComboBox.h"
#include "SQREdit.h"
#include "SQRRichText.h"
#include "SQRProgress.h"
#include "SQRStatic.h"
#include "SQRScrollBar.h"
#include "SQRSpin.h"
#include "SQRWnd.h"
#include "SQRClock.h"
#include "SQRWndOffSet.h"
#include "SQRTreeCtrl.h"
#include "SQRListCtrl.h"
#include "SQRChart.h"
#include "SQRIcon.h"
#include "SQRVideo.h"
#include "SQRFlashWnd.h"
#include "SQRWebBrowser.h"

#include "TSqrAllocator.inl"

SQRGUIStyle::SQRGUIStyle(const string& Name,const string& Path)
: m_isLoad(false)
, m_StyleName(Name.c_str())
, m_strPath(Path.c_str())
{
	m_strStylePath = ((SQRGUIManager*)IGUIManager::GetInst())->m_strPkgName + "style\\" + m_strPath + "\\";
	m_strTexture   = "guitex/tex/";
}

SQRGUIStyle::~SQRGUIStyle()
{
	Release();
	RectMap::iterator it,eit = m_mapRect.end();
	for(it = m_mapRect.begin();it!=eit;++it)
		delete it->second;
	m_mapRect.clear();

}

bool SQRGUIStyle::Initialize(void)
{
	if(!m_isLoad)
	{
		ipkgstream iXML;
		TiXmlDocument XmlDoc;
		UIString Str = m_strStylePath + "Config.xml";
		if (iXML.open(GUI_PATH_ALIAS_GUI.c_str(), Str.c_str()))
			return false;

		iXML>>XmlDoc;

		TiXmlNode* pStyle = XmlDoc.FirstChild("Style");

		for( TiXmlNode* pNode = pStyle->FirstChild("GUI");pNode; pNode = pNode->NextSibling("GUI") )
		{
			TiXmlElement* pElement = pNode->ToElement();
			if (!pElement)
				continue;
			StyleStruct* pStyle = SetStyleStruct(pElement->Attribute("Name"),StyleStruct());

			pStyle->m_TexString =  pElement->Attribute("Texture");
			Str = m_strTexture + pStyle->m_TexString;
			if(pStyle->m_TexString.size()>0)
				CGraphic::GetInst()->CreateTexture(GUI_PATH_ALIAS_GUITEX.c_str(), Str.c_str(), &(pStyle->pTexture));

			for( TiXmlNode* pElemNode = pNode->FirstChild("Elem"); pElemNode; pElemNode = pElemNode->NextSibling("Elem") )
			{
				TiXmlElement* pElem  = pElemNode->ToElement();
				if(!pElem)
					continue;
				int		Type;
				pElem->QueryIntAttribute("EventState",&Type);
				BLOCK_POOL& BlockPool = pStyle->m_vecPool[static_cast<EventStateMask>(Type)];

				for( TiXmlNode* pBlockPoolNode = pElemNode->FirstChild("BlockPool"); pBlockPoolNode; pBlockPoolNode = pBlockPoolNode->NextSibling("BlockPool") )
				{
					TiXmlElement* pElemBlockPool  = pBlockPoolNode->ToElement();
					if (!pElemBlockPool)
						continue;
					int Index;
					pElemBlockPool->QueryIntAttribute("Index",&Index);
					if( static_cast<UINT>(Index)>BLOCK_POOL::MAX_BLOCK_NUM || Index < 1)
						continue;

					GUI_BLOCK& Block = BlockPool.InitBlock(Index);
					for( TiXmlNode* pBlockNode = pElemBlockPool->FirstChild("Block"); pBlockNode; pBlockNode = pBlockNode->NextSibling("Block") )
					{
						CFRect	Rect;
						int		BlockMask;
						TiXmlElement* pElemBlock  = pBlockNode->ToElement();
						if (!pElemBlock)
							continue;
						pElemBlock->QueryIntAttribute("BlockMask",&BlockMask);
						pElemBlock->QueryFloatAttribute("Top",&(Rect.top));
						pElemBlock->QueryFloatAttribute("Left",&(Rect.left));
						pElemBlock->QueryFloatAttribute("Right",&(Rect.right));
						pElemBlock->QueryFloatAttribute("Bottom",&(Rect.bottom));
						Block[BlockMask] = Rect;
					}

					TiXmlNode* pFontNode = pElemBlockPool->FirstChild("Font");
					if ( pFontNode )
					{
						// 有font信息
						TiXmlElement* pElemFont = pFontNode->ToElement();
						if (!pElemFont)
							continue;

						if (pStyle->m_vecFont.find(Index) == pStyle->m_vecFont.end())
							pStyle->m_vecFont[Index] = new GUI_FONT();
						GUI_FONT* GuiFont = pStyle->m_vecFont[Index];


						std::string winFontId = pElemFont->Attribute(GUILayoutHandler::PropertyNameFont.c_str());
						int32 fontid;
						integer(winFontId,fontid);
						GuiFont->m_nFontID			= fontid;

						std::string winFontSize = pElemFont->Attribute(GUILayoutHandler::PropertyNameFontSize.c_str());
						uint32 FontSize;
						uinteger(winFontSize,FontSize);
						GuiFont->m_FontSize = uint32(FontSize);

						

						std::string winFGCol = pElemFont->Attribute(GUILayoutHandler::PropertyNameFontGCol.c_str());
						uint32 tempCol;
						uinteger(winFGCol, tempCol);
						GuiFont->m_uFontGradualColor = (uint32)tempCol;

						std::string winFGEff = pElemFont->Attribute(GUILayoutHandler::PropertyNameFontEffe.c_str());
						uint32 tempeff;
						uinteger(winFGEff, tempeff);
						GuiFont->m_uFontEffectMask = (uint32)tempeff;
					}
					TiXmlNode* pEvtFontNode = pElemBlockPool->FirstChild("EvtFont");
					if (pEvtFontNode)
					{
						TiXmlElement* pElemFont = pEvtFontNode->ToElement();
						if (!pElemFont)
							continue;
						// 有这类结点
						if (pStyle->m_vecFont.find(Index) != pStyle->m_vecFont.end())
						{
							// 只有创建过的才去读
							GUI_FONT* GuiFont = pStyle->m_vecFont[Index];
							if (GuiFont)
							{
								// 字体颜色
								std::string sStateCol = pElemFont->Attribute(GUILayoutHandler::PropertyStateColor.c_str());
								uint32 iStateCol;
								uinteger(sStateCol, iStateCol);
								GuiFont->m_EventFont[static_cast<EventStateMask>(Type)].m_FontColor =  (uint32)iStateCol;

								std::string sRimCol = pElemFont->Attribute(GUILayoutHandler::PropertyRimColor.c_str());
								uint32 iRimCol;
								uinteger(sRimCol, iRimCol);
								GuiFont->m_EventFont[static_cast<EventStateMask>(Type)].m_uFontBkColor = (uint32)iRimCol;
							}
						}
					}
				}
			}
		}
		m_isLoad = true;
	}

	
	UPDATE_WND(SQRWnd);
	UPDATE_WND(SQRButton);
	UPDATE_WND(SQRCheckButton);
	UPDATE_WND(SQREdit);
	UPDATE_WND(SQRComboBox);
	UPDATE_WND(SQRRichText);
	UPDATE_WND(SQRProgress);
	UPDATE_WND(SQRStatic);
	UPDATE_WND(SQRScrollBar);
	UPDATE_WND(SQRSpin);
	UPDATE_WND(SQRClock);
	UPDATE_WND(SQRWndOffSet);
	UPDATE_WND(SQRListCtrl);
	UPDATE_WND(SQRTreeCtrl);
	UPDATE_WND(SQRChart);
	UPDATE_WND(SQRIcon);

	SQRDialog::StyleSQRDialog = SQRWnd::StyleSQRWnd;
	SQRRootWnd::StyleSQRRootWnd = SQRWnd::StyleSQRWnd;
	return m_isLoad;
}

void SQRGUIStyle::Save(void)
{
	UIString	CommentStr = "\n\
		EventStateMask:\n\
		MASK							= 0, \n\
		MOUSEDESC					= 1, \n\
		ENABLE						= 2, \n\
		DISABLE						= 3, \n\
		MOUSEOVER					= 4, \n\
		CLICKDOWN					= 5, \n\
		UNCHECK_ENABLE		= 6, \n\
		UNCHECK_DISABLE		= 7, \n\
		UNCHECK_MOUSEOVER	= 8, \n\
		UNCHECK_CLICKDOWN	= 9, \n\
		FILL							= 10,\n\
		TR_ENABLE					= 11,\n\
		TR_DISABLE				= 12,\n\
		LS_SEL						= 13,\n\
		LS_UNSEL					= 14,\n\
		\n\
		GuiBlockMask:\n\
		GBM_TOP						= 0,\n\
		GBM_LEFT					= 1,\n\
		GBM_RIGHT					= 2,\n\
		GBM_BOTTOM				= 3,\n\
		GBM_CENTER				= 4,\n\
		GBM_TOP_LEFT			= 5,\n\
		GBM_TOP_RIGHT			= 6,\n\
		GBM_BOTTOM_LEFT		= 7,\n\
		GBM_BOTTOM_RIGHT	= 8,\n\
		\n\
		MAX_BLOCK_NUM			= 32\n\
		";

	TiXmlDocument		XmlDoc;
	TiXmlDeclaration	XmlDecl;
	TiXmlComment		XmlComment( CommentStr.c_str() );

	XmlDecl.Parse( "<?xml version='1.0' encoding='UTF-8'?>", 0, TIXML_ENCODING_UNKNOWN );
	TiXmlElement EleStyle("Style");
	RectMap::iterator it,eit = m_mapRect.end();
	for(it = m_mapRect.begin();it!=eit;++it)
	{
		TiXmlElement EleGUI("GUI");
		EleGUI.SetAttribute("Name",it->first.c_str());
		EleGUI.SetAttribute("Texture",it->second->m_TexString.c_str());
		string temp_str = it->second->m_TexString.c_str();
		AddResData(eRes_T_Style, temp_str);

		bool bFirst[DEF_MAX_BLOCK_NUM];
		for (uint32 ui = 0; ui < DEF_MAX_BLOCK_NUM; ++ui)
			bFirst[ui] = true;
		StyleStruct::PoolVector::iterator vit,evit = it->second->m_vecPool.end();
		for( vit = it->second->m_vecPool.begin();vit!=evit;++vit )
		{	
			TiXmlElement Elem("Elem");
			Elem.SetAttribute("EventState",vit->first);
			BLOCK_POOL& BlockPool = (vit->second);
			for(uint32 pidx = 0; pidx< BLOCK_POOL::MAX_BLOCK_NUM; ++pidx)
			{
				if( BlockPool[pidx + 1] == NULL )
					continue;

				TiXmlElement ElemBlockPool("BlockPool");
				ElemBlockPool.SetAttribute("Index",int(pidx + 1));

				GUI_BLOCK& Block = *BlockPool[pidx+1];
				for(int i = 0; i<GBM_COUNT; ++i)
				{
					if( int(Block[i].top)	== 0 &&
						int(Block[i].left)	== 0 &&
						int(Block[i].right)	== 0 &&
						int(Block[i].bottom)== 0 )
						continue;

					TiXmlElement ElemBlock("Block");
					ElemBlock.SetAttribute("BlockMask",int(i));
					ElemBlock.SetAttribute("Top",int(Block[i].top));
					ElemBlock.SetAttribute("Left",int(Block[i].left));
					ElemBlock.SetAttribute("Right",int(Block[i].right));
					ElemBlock.SetAttribute("Bottom",int(Block[i].bottom));
					ElemBlockPool.InsertEndChild(ElemBlock);
				}
				
				StyleStruct::FontVector::iterator fvit = it->second->m_vecFont.find(int(pidx+1));
				StyleStruct::FontVector::iterator fevit = it->second->m_vecFont.end();
				if (bFirst[pidx])
				{
					bFirst[pidx] = false;
					if (fvit != fevit)
					{
						// save for the first time
						GUI_FONT* pfont = fvit->second;
						if (pfont)
						{
							TiXmlElement ElemFont("Font");
							string tempStr;
							// save font id
							int32     nFontId		= pfont->m_nFontID;
							tostring(tempStr, nFontId);
							ReBuiltU32Str( tempStr );
							ElemFont.SetAttribute( GUILayoutHandler::PropertyNameFont.c_str(), tempStr.c_str() );

							// save font size
							int		nFontSize		= (int32)(pfont->m_FontSize);
							tostring(tempStr, nFontSize);
							ReBuiltU32Str( tempStr );
							ElemFont.SetAttribute( GUILayoutHandler::PropertyNameFontSize.c_str(), tempStr.c_str() );

							// save font gradual
							uint32 fontGradualColor	= pfont->m_uFontGradualColor;
							tostring(tempStr, fontGradualColor);
							ReBuiltU32Str( tempStr );
							ElemFont.SetAttribute( GUILayoutHandler::PropertyNameFontGCol.c_str(), tempStr.c_str() );

							

							// save font effect
							uint32 fontEffectMask	= pfont->m_uFontEffectMask;
							tostring(tempStr, fontEffectMask);
							ReBuiltU32Str( tempStr );
							ElemFont.SetAttribute( GUILayoutHandler::PropertyNameFontEffe.c_str(), tempStr.c_str() );

							ElemBlockPool.InsertEndChild(ElemFont);
						}
					}
					
				}
				if (fvit != fevit)
				{
					string tempStr;
					GUI_FONT* pfont = fvit->second;
					if (pfont)
					{
						GUI_EVENT_FONT* pEvtFont = pfont->GetEventFont((EventStateMask)(vit->first));
						if (pEvtFont)
						{
							TiXmlElement ElemEvtFont("EvtFont");
							uint32 nStateColor = pEvtFont->m_FontColor;
							tostring(tempStr, nStateColor);
							ReBuiltU32Str( tempStr );
							ElemEvtFont.SetAttribute( GUILayoutHandler::PropertyStateColor.c_str(), tempStr.c_str() );

							uint32 uRimColor = pEvtFont->m_uFontBkColor;
							tostring(tempStr, uRimColor);
							ReBuiltU32Str( tempStr );
							ElemEvtFont.SetAttribute( GUILayoutHandler::PropertyRimColor.c_str(), tempStr.c_str() );
							ElemBlockPool.InsertEndChild(ElemEvtFont);
						}
					}
				}
				// 和状态有关的字体信息
				Elem.InsertEndChild(ElemBlockPool);
			}
			
			EleGUI.InsertEndChild(Elem);
		}
		EleStyle.InsertEndChild(EleGUI);
	}

	XmlDoc.InsertEndChild(XmlDecl);
	XmlDoc.InsertEndChild(XmlComment);
	XmlDoc.InsertEndChild(EleStyle);

	string sub_style_save_path = utf16_to_gbk(CPkgFile::FindPathInclude(L"gui")) + "/";
	XmlDoc.SaveFile((sub_style_save_path + m_strStylePath.c_str() + "Config.xml").c_str());
}

void SQRGUIStyle::ReBuiltU32Str(string& val)
{
	size_t pos = val.find_first_of(',');
	while( pos != string::npos)
	{
		val.erase(pos,1);
		pos = val.find_first_of(',');
	}
}

void SQRGUIStyle::Release(void)
{
	m_isLoad = false;
}

StyleStruct* SQRGUIStyle::GetStyleStruct(const string& str)
{
	RectMap::iterator it = m_mapRect.find(str.c_str());
	if(it!=m_mapRect.end())
		return it->second;
	return NULL;
}

StyleStruct* SQRGUIStyle::SetStyleStruct(const string& str , const StyleStruct& Style)
{
	RectMap::iterator it = m_mapRect.find(str.c_str());
	if(it!=m_mapRect.end())
	{
		*(it->second) = Style;
		return it->second;
	}
	else
	{
		StyleStruct* pStyle = new StyleStruct(Style);
		*pStyle = Style;
		m_mapRect.insert(RectMap::value_type(str.c_str(),pStyle));
		return pStyle;
	}
	
}


//---------------------------------------------------------------------------
bool	SQRGUIStyleManager::Load(const string& fName)
{
	m_XMLFileName = fName.c_str();
	ipkgstream iXML;
	TiXmlDocument XmlDoc;
	if (iXML.open(GUI_PATH_ALIAS_GUI.c_str(), m_XMLFileName.c_str()))
		return false;

	iXML>>XmlDoc;
 	TiXmlNode* pStyle = XmlDoc.FirstChild("Styles");

	for( TiXmlNode* pNode = pStyle->FirstChild("Style");pNode; pNode = pNode->NextSibling("Style") )
	{
		if (!pNode->ToElement())
			continue;
		AddStyle(pNode->FirstChild()->ValueStr(),pNode->ToElement()->Attribute("Path"));
	}
	
	TiXmlElement* pElement = pStyle->ToElement();
	ActiveStyle(pElement->Attribute("DefaultActive"));
	iXML.close();
	return true;
}

bool	SQRGUIStyleManager::Save(const string& fName)
{
	TiXmlDocument XmlDoc;
	TiXmlDeclaration XmlDecl;
	XmlDecl.Parse( "<?xml version='1.0' encoding='UTF-8'?>", 0, TIXML_ENCODING_UNKNOWN );

	TiXmlElement pStyles("Styles");
	pStyles.SetAttribute("DefaultActive",m_ActiveStyleName.c_str());

	STYLE_MAP::iterator it,eit = m_Styles.end();
	for(it = m_Styles.begin();it!=eit;++it)
	{	
		TiXmlElement pStyle("Style");
		pStyle.SetAttribute("Path",it->second->GetPath());
		TiXmlText pText(it->second->GetStyleName());
		pStyle.InsertEndChild(pText);
		pStyles.InsertEndChild(pStyle);
	}
	XmlDoc.InsertEndChild(XmlDecl);
	XmlDoc.InsertEndChild(pStyles);
	XmlDoc.SaveFile(fName);
	return true;
}


SQRGUIStyleManager::SQRGUIStyleManager()
: m_pActiveStyle(NULL)
{}

SQRGUIStyleManager::~SQRGUIStyleManager()
{ Release(); }

SQRGUIStyle* SQRGUIStyleManager::GetStyle(const string& sName)
{
	return m_Styles[sName.c_str()];
}

SQRGUIStyle* SQRGUIStyleManager::AddStyle(const string& sName)
{ return AddStyle(sName,sName); }

SQRGUIStyle* SQRGUIStyleManager::AddStyle(const string& sName,const string& sPath)
{
	STYLE_MAP::iterator it = m_Styles.find(sName.c_str());
	if(it!=m_Styles.end())
		return it->second;
	
	SQRGUIStyle* pStyle = new SQRGUIStyle(sName,sPath);
	if(pStyle==NULL)
		return NULL;

	m_Styles.insert(STYLE_MAP::value_type(sName.c_str(),pStyle));
	return pStyle;
}

bool	SQRGUIStyleManager::DelStyle(const string& sName)
{
	STYLE_MAP::iterator it = m_Styles.find(sName.c_str());
	if(it!=m_Styles.end())
	{
		if(m_pActiveStyle==it->second)
		{
			m_pActiveStyle = NULL;
			m_ActiveStyleName.clear();
		}
		delete it->second;
		m_Styles.erase(it);
		return true;
	}
	return false;
}

SQRGUIStyle* SQRGUIStyleManager::ActiveStyle(const string& sName)
{ 
	STYLE_MAP::iterator it = m_Styles.find(sName.c_str());
	if(it!=m_Styles.end())
	{
		if(m_pActiveStyle == it->second)
			return m_pActiveStyle;	
		SafeRelease(m_pActiveStyle);
		m_pActiveStyle = it->second;
		m_ActiveStyleName = sName.c_str();
		m_pActiveStyle->Initialize();
		return m_pActiveStyle;
	}
	m_ActiveStyleName.clear();
	m_pActiveStyle = NULL;
	return NULL;
}

void	SQRGUIStyleManager::Release()
{
	STYLE_MAP::iterator it,eit = m_Styles.end();
	for(it = m_Styles.begin();it!=eit;++it)
		delete it->second;
	m_Styles.clear();
}

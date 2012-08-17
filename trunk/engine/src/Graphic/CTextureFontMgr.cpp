#include "stdafx.h"
#include "CTextureFontMgr.h"
#include "tinyxml/tinyxml.h"
#include "PkgStream.h"
#include "StringHelper.h"
#include "CodeCvs.h"
#include <iostream>

namespace sqr
{
	const GString CTextureFontMgr::Emt_Root	= "Emoticon";			// 根
	const GString CTextureFontMgr::Emt_Face	= "Face";					// 节点
	const GString CTextureFontMgr::Emt_Fc_Tex	= "Tex";				// 材质
	const GString CTextureFontMgr::Emt_Fc_Id	= "Id";					// ID
	const GString CTextureFontMgr::Emt_Fc_Key	= "Key";				// key two
	const GString CTextureFontMgr::Emt_Fc_Pos	= "Pos";				// 位置
	const GString CTextureFontMgr::Emt_Fc_Tip	= "Tip";				// 描述
	const GString CTextureFontMgr::Emt_Fc_Idx	= "Idx";				// 索引
	CTextureFontMgr::CTextureFontMgr()
		: m_isLoad(false)
		, m_pGraphic(NULL)
		, m_pTextue(NULL)
		, m_RS(RS_EMT)
		, m_scale(1.0f)
		, m_nCurTextureBufPos(0)
	{
		m_EmtDList.clear();
		m_EmtMap.clear();
		m_TextureBuf.clear();
		m_EmtIdxMap.clear();
		IntInst() = this;
	}

	CTextureFontMgr::~CTextureFontMgr()
	{
		IntInst() = NULL;
		Release();
	}

	CTextureFontMgr* CTextureFontMgr::Inst()
	{
		return IntInst();
	}

	CTextureFontMgr*& CTextureFontMgr::IntInst()
	{
		static CTextureFontMgr*	ls_pTextureFontMgr=NULL;
		return ls_pTextureFontMgr;
	}

	CFRect CTextureFontMgr::StringToCFRect(std::string& str)
	{
		int pos, tempInt;
		string sub, subend;
		CFRect tempRt;

		pos = str.find_first_of(',');
		sub = str.substr(0,pos);
		subend = str.substr(pos + 1, str.length() - pos - 1);
		integer(sub, tempInt);
		tempRt.left = (float)tempInt;

		pos = subend.find_first_of(',');
		sub = subend.substr(0,pos);
		subend = subend.substr(pos + 1, subend.length() - pos - 1);
		integer(sub, tempInt);
		tempRt.top = (float)tempInt;

		pos = subend.find_first_of(',');
		sub = subend.substr(0,pos);
		subend = subend.substr(pos + 1, subend.length() - pos - 1);
		integer(sub, tempInt);
		tempRt.right = (float)tempInt;

		integer(subend, tempInt);
		tempRt.bottom = (float)tempInt;

		return tempRt;
	}

	// 初始化表情系统 材质字体
	bool CTextureFontMgr::Initialize(const string& Path, IGraphic* pIGraphic)
	{
		if(!m_isLoad)
		{
			m_pGraphic = pIGraphic;
			// 表情
			if ( !InitializeEmt(Path) )	return false;
			// 材质
			if ( !InitializeTex(Path) ) return false;
		}
		m_isLoad = true;
		return m_isLoad;
	}

	// 初始化表情系统
	bool CTextureFontMgr::InitializeEmt(const string& Path)
	{
		ipkgstream iXML;
		TiXmlDocument XmlDoc;

		const GString Str = Path.c_str();
		if (iXML.open(PATH_ALIAS_EMT.c_str(), Str.c_str()))
			return false;
 
		iXML>>XmlDoc;
		TiXmlNode* EmtRoot = XmlDoc.FirstChild( Emt_Root.c_str() );

		for( TiXmlNode* pNode = EmtRoot->FirstChild( Emt_Face.c_str() );pNode; pNode = pNode->NextSibling( Emt_Face.c_str() ) )
		{
			TiXmlElement* pElement = pNode->ToElement();
			if (!pElement)
				continue;

			uint32		temp_idx;
			GString		temp_texname;
			GString		temp_strIdx;
			GWString	temp_strId = L"F";
			CFRect		temp_pos; 
			GWString	temp_des;

			// ID
			std::string tempStr = pElement->Attribute( Emt_Fc_Id.c_str() );
			uinteger(tempStr, temp_idx);

			// POS
			tempStr = pElement->Attribute( Emt_Fc_Pos.c_str() );
			temp_pos =  StringToCFRect(tempStr);

			// STRING IDX
			temp_strIdx = pElement->Attribute( Emt_Fc_Idx.c_str() );
			temp_strId[0] = temp_idx;

			//DES
			std::wstring wtemp_des = L"EMT";

			// TEXTURE NAME
			TiXmlNode* pElemNode = pNode->FirstChild( Emt_Fc_Tex.c_str() );
			TiXmlElement* pElem  = pElemNode->ToElement();
			if(!pElem)
			{
				continue;
			}
			temp_texname = pElem->GetText();

			TextureFontInfo* pEmt = new TextureFontInfo();
			pEmt->Init(m_pGraphic, temp_idx, temp_texname, temp_pos, wtemp_des.c_str());

			// emt map
			m_EmtMap.insert(pair<uint32, TextureFontInfo*>(pEmt->EmtIdx, pEmt));
			// id map
			m_EmtIdxMap.insert(pair<GString, GWString>(temp_strIdx, temp_strId));
		}
		return true;
	}
	
	// 初始化材质字体
	bool CTextureFontMgr::InitializeTex(const string& Path)
	{
		ipkgstream iXML;
		TiXmlDocument XmlDoc;

		GString Str = Path.c_str();
		Str = Str.substr(0, Str.size() - 15) + "TextureFontCfg.xml";
		if (iXML.open(PATH_ALIAS_EMT.c_str(), Str.c_str()))
			return false;

		iXML>>XmlDoc;
		TiXmlNode* EmtRoot = XmlDoc.FirstChild( Emt_Root.c_str() );

		for( TiXmlNode* pNode = EmtRoot->FirstChild( Emt_Face.c_str() );pNode; pNode = pNode->NextSibling( Emt_Face.c_str() ) )
		{
			TiXmlElement* pElement = pNode->ToElement();
			if (!pElement)
				continue;

			uint32		temp_idx;
			uint32		temp_key;
			GString		temp_texname;
			CFRect		temp_pos; 
			GWString	temp_des;

			// ID
			std::string tempStr = pElement->Attribute( Emt_Fc_Id.c_str() );
			uinteger(tempStr, temp_idx);

			// KEY
			tempStr = pElement->Attribute( Emt_Fc_Key.c_str() );
			uinteger(tempStr, temp_key);

			// POS
			tempStr = pElement->Attribute( Emt_Fc_Pos.c_str() );
			temp_pos =  StringToCFRect(tempStr);

			std::wstring wtemp_des = L"EMT";

			// TEXTURE NAME
			TiXmlNode* pElemNode = pNode->FirstChild( Emt_Fc_Tex.c_str() );
			TiXmlElement* pElem  = pElemNode->ToElement();
			if(!pElem)
			{
				continue;
			}
			temp_texname = pElem->GetText();

			TextureFontInfoII* pTex = new TextureFontInfoII();
			IdxKey keyIdx = make_pair(temp_idx, temp_key);
			pTex->Init(m_pGraphic, keyIdx, temp_texname, temp_pos, wtemp_des.c_str());

			m_TexMap.insert(TextureFontIIMap::value_type(pTex->TexIdx, pTex));
		}
		return true;
	}
	
	// 释放申请资源
	void CTextureFontMgr::Release(void)
	{
		SafeRelease( m_pTextue );
		TextureFontMap::iterator itr = m_EmtMap.begin();
		TextureFontMap::iterator itr_end = m_EmtMap.end();
		while(itr != itr_end)
		{
			GfkSafeDelete(itr->second);
			itr++;
		}
		m_EmtMap.clear();
		m_EmtIdxMap.clear();

		TextureFontIIMap::iterator itrII = m_TexMap.begin();
		TextureFontIIMap::iterator itrII_end = m_TexMap.end();
		while(itrII != itrII_end)
		{
			GfkSafeDelete(itrII->second);
			itrII++;
		}
		m_TexMap.clear();
	}

	void CTextureFontMgr::EraserTextureFont(TextureFontMap::iterator it)
	{
		GfkSafeDelete(it->second);
		m_EmtMap.erase(it);
	}

	TextureFontInfo* CTextureFontMgr::GetTexFontInfo(uint32 id, uint8 idx)
	{
		if ( m_EmtMap.end() != m_EmtMap.find(id) )
			return m_EmtMap[id];

		IdxKey temp_key = make_pair(id, (uint32)idx);
		if ( m_TexMap.end() != m_TexMap.find( temp_key ) )
			return m_TexMap[temp_key];
		return NULL;
	}

	// 渲染
	void CTextureFontMgr::DrawEmtList()
	{
		size_t sz = m_EmtDList.size();
		if ( sz == 0 )
			return;

		for (size_t i = 0; i < sz; i++)
		{
			DrawRect( m_EmtDList[i]->posRect, m_EmtDList[i]->pEmtImg->dwColor, m_EmtDList[i]->pEmtImg->pTexture,
					  m_EmtDList[i]->texRect, m_RS, 0);
			
		}
		FlushRect();
		for (size_t i = 0; i < sz; i++)
		{
			delete m_EmtDList[i];
		}
		m_EmtDList.clear();
	}

	void CTextureFontMgr::DrawRect( const CFRect& rt, uint32 uColor, ITexture* pTexture, 
								 const CFRect& rtText, ERenderStyle rs, float z )
	{
		if ( m_nCurTextureBufPos > 0 && ( m_pTextue != pTexture || m_RS != rs ) )
			FlushRect();
		if ( pTexture )
			pTexture->AddRef();
		SafeRelease( m_pTextue );

		if ( m_nCurTextureBufPos >= m_TextureBuf.size() )
			m_TextureBuf.push_back( RectGroup() );
		m_pTextue									= pTexture;
		m_RS										= rs;
		m_TextureBuf[m_nCurTextureBufPos].bFullText	= false;
		m_TextureBuf[m_nCurTextureBufPos].Color		= uColor;
		m_TextureBuf[m_nCurTextureBufPos].z			= z;
		m_TextureBuf[m_nCurTextureBufPos].vec[0].Init( rt.left,  rt.bottom );
		m_TextureBuf[m_nCurTextureBufPos].vec[1].Init( rt.left,  rt.top );
		m_TextureBuf[m_nCurTextureBufPos].vec[2].Init( rt.right, rt.bottom );
		m_TextureBuf[m_nCurTextureBufPos].vec[3].Init( rt.right, rt.top );

		m_TextureBuf[m_nCurTextureBufPos].rtText[0].Init( rtText.left,  rtText.bottom );
		m_TextureBuf[m_nCurTextureBufPos].rtText[1].Init( rtText.left,  rtText.top );
		m_TextureBuf[m_nCurTextureBufPos].rtText[2].Init( rtText.right, rtText.bottom );
		m_TextureBuf[m_nCurTextureBufPos].rtText[3].Init( rtText.right, rtText.top );

		m_nCurTextureBufPos++;
	}

	void CTextureFontMgr::FlushRect()
	{
		if ( m_nCurTextureBufPos )
		{
			m_nTotalRect += m_nCurTextureBufPos;
			m_nFlush++;
			if ( m_pTextue )
				m_pGraphic->DrawRectGroup( m_pTextue, m_RS, &m_TextureBuf[0], m_nCurTextureBufPos );
			m_nCurTextureBufPos = 0;
		}
	}

	wchar_t CTextureFontMgr::GetEmtByIdx(char* idx)
	{
		GString temp_idx = idx;
		GWString temp_char;
	
		if ( m_EmtIdxMap.end() != m_EmtIdxMap.find(temp_idx) )
			temp_char = m_EmtIdxMap[temp_idx];
		return temp_char[0];
	}

	const wchar_t* CTextureFontMgr::GetEmtPtrByIdx(char* idx)
	{
		GString temp_idx = idx;
		GWString temp_char;

		if ( m_EmtIdxMap.end() != m_EmtIdxMap.find(temp_idx) )
			temp_char = m_EmtIdxMap[temp_idx];

		return temp_char.c_str();
	}

	const char* CTextureFontMgr::GetEmtStrByIdx(char* idx)
	{
		GString temp_idx = idx;
		GWString temp_char;

		if ( m_EmtIdxMap.end() != m_EmtIdxMap.find(temp_idx) )
			temp_char = m_EmtIdxMap[temp_idx];

		static string temp_str;
		temp_str = utf16_to_utf8(temp_char.c_str());
		return temp_str.c_str();
	}

	void CTextureFontMgr::SetImgScale(float sc)
	{
		m_scale = sc;
	}

	float CTextureFontMgr::GetImgScale()
	{
		return m_scale;
	}
	
}// end of namespace

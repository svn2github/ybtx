#include "StdAfx.h"
#include "Brush/TerrainTextureData.h"
#include "CEditContext.h"
#include "CDataScene.h"
#include "CTerrainMgr.h"
#include "CGraphic.h"
#include "CTexture.h"
#include "EditorExp.h"

const string strDefaultTex = "res\\default.bmp";

CTerrainTextureData::CTerrainTextureData(void)
{
	
}

CTerrainTextureData::~CTerrainTextureData(void)
{
	m_mapTerrainTexIndex.clear();
	m_mapTerrainTexName.clear();
}

void CTerrainTextureData::SetEditContext( CEditContext* pOwnerContext )
{
	m_pContext = pOwnerContext;
}

void CTerrainTextureData::InitTerrainTexture()
{
	bool isAsyc = CPkgFile::IsEnableAsyncRead();
	if( isAsyc )
		CGraphic::GetInst()->SetAsynSwitch(false);

	uint EXPAND_MAX_SIZE = CTerrainMgr::MAX_TEXTURE + CTerrainMgr::EXPAND_SIZE * CTerrainMgr::MAX_ROW_INDEX * 2;
	HRESULT hr = CGraphic::GetInst()->CreateEmptyTexture( EXPAND_MAX_SIZE,EXPAND_MAX_SIZE, 1, TF_ARGB32, &m_pTerrainTexture );
	if ( hr != S_OK )
	{
		MessageBox(NULL,"创建拼接纹理失败","错误",MB_OK);
	}

	vector<string> texnames;
	texnames.push_back(strDefaultTex);
	CreateTerrainTexture(texnames);
}

bool CTerrainTextureData::CreateTerrainTexture( const vector<string> &texnames )
{
	if ( m_pTerrainTexture == NULL )
		return false;

	bool bFailedBrush = false;
	ITexture* pTexture = NULL;
	string texname = "";
	size_t sizetexs = texnames.size();
	for ( size_t i = 0; i < sizetexs; ++ i )
	{
		texname = texnames[i];
		size_t npos = texname.find(".dds");
		if( npos != -1 )
			bFailedBrush = true;
	}
	if( bFailedBrush )
	{
		static string log = "";
		string msg = texname+"不属于地表纹理，不能刷地表";
		if( log != msg )
		{
			log = msg;
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
		}

		return false;
	}

	CIRect rectT;
	_TEXLOCKED_RECT lockT;
	bool bNeedSave = false;

	EDITOR_TRY
	{
		static const UINT MAX_TEXTURE = 2048;
		static const UINT TEXTURE_SIZE = 128;
		static const UINT EXPAND_SIZE = 8;
		static const UINT ROW_SIZE = MAX_TEXTURE/TEXTURE_SIZE;
		static const UINT TEXTURE_CNT = ROW_SIZE*ROW_SIZE;
		static const UINT EXPAND_SIZE_TWO = EXPAND_SIZE*2;
		static const UINT EXPAND_TEXTURE_SIZE = TEXTURE_SIZE + EXPAND_SIZE_TWO;
		static const UINT EXPAND_MAX_SIZE = MAX_TEXTURE + ROW_SIZE*EXPAND_SIZE_TWO;
		static const UINT PixelSize = 4;
		bool isAsyc = CPkgFile::IsEnableAsyncRead();
		if( isAsyc )
			CGraphic::GetInst()->SetAsynSwitch(false);

		for ( size_t i = 0; i < sizetexs; ++ i )
		{
			texname = texnames[i];
			if ( texname == "" )
				continue;

			// 查找纹理是否已经存在，如果存在不进行拼图
			if ( m_mapTerrainTexName.find(texname) != m_mapTerrainTexName.end() )
				continue;

			// 纹理不存在，则先判断是否超过纹理总数256
			if ( m_mapTerrainTexName.size() > TEXTURE_CNT )
			{
				string log = texname+"地表纹理总数已达到256张";
				MessageBox(NULL,log.c_str(),"错误",MB_OK);

				return false;
			}

			if( FAILED(CGraphic::GetInst()->CreateTexture(L"res", texname.c_str(), &pTexture) ) )
			{
				MessageBox(NULL, texname.c_str(), "找不到", MB_OK);
				continue;
			}

			CSyncSystem::BeginSyncFun();

			if (NULL==pTexture)
			{
				string log = "拼接过程:"+texname+"纹理无法找到";
				MessageBox(NULL,log.c_str(),"错误",MB_OK);
				return false;
			}

			if (NULL==pTexture->GetBuffer())
			{
				string log = "拼接过程:"+texname+"GetBuffer失败";
				MessageBox(NULL,log.c_str(),"错误",MB_OK);
				return false;
			}

			size_t texCnt = m_mapTerrainTexName.size();
			UINT H = (texCnt/ROW_SIZE)*EXPAND_TEXTURE_SIZE;
			UINT W = (texCnt%ROW_SIZE)*EXPAND_TEXTURE_SIZE;

			if (pTexture->GetWidth()!=TEXTURE_SIZE||pTexture->GetHeight()!=TEXTURE_SIZE)
			{
				string log = "拼接过程:"+texname+"尺寸不合法,所有贴图纹理应为128X128";
				MessageBox(NULL,log.c_str(),"错误",MB_OK);
				return false;
			}

			CIRect rectO;
			_TEXLOCKED_RECT lockO;
			rectO.top = rectO.left = 0;
			rectO.bottom = rectO.right = TEXTURE_SIZE;
			rectT.top = H;
			rectT.bottom = H + EXPAND_TEXTURE_SIZE;
			rectT.left = W;
			rectT.right = W + EXPAND_TEXTURE_SIZE;

			if ( FAILED( this->m_pTerrainTexture->Lock(0,&lockT,&rectT,0,LOCK_DISCARD) ) )
			{
				return false;
			}

			pTexture->Lock(0,&lockO,&rectO,0,LOCK_READONLY);
			TCHAR* pTar = (TCHAR*)lockT.pBits;
			TCHAR* pOrc = (TCHAR*)lockO.pBits;

			for (UINT i=0;i<EXPAND_SIZE;++i,pTar+=lockT.Pitch)
			{
				for ( UINT j=0;j<EXPAND_SIZE;++j)
				{
					memcpy(pTar+PixelSize*j,pOrc,PixelSize);
					memcpy(pTar+lockO.Pitch+PixelSize*(j+EXPAND_SIZE),pOrc+lockO.Pitch-PixelSize,PixelSize);
				}
				memcpy(pTar+PixelSize*EXPAND_SIZE,pOrc,lockO.Pitch);
			}

			for (UINT i = 0;i<TEXTURE_SIZE;++i,pTar+=lockT.Pitch,pOrc+=lockO.Pitch)
			{
				for ( UINT j=0;j<EXPAND_SIZE;++j)
				{
					memcpy(pTar+PixelSize*j,pOrc,PixelSize);
					memcpy(pTar+lockO.Pitch+PixelSize*(j+EXPAND_SIZE),pOrc+lockO.Pitch-PixelSize,PixelSize);
				}
				memcpy(pTar+PixelSize*EXPAND_SIZE,pOrc,lockO.Pitch);
			}
			pOrc-=lockO.Pitch;
			for (UINT i=0;i<EXPAND_SIZE;++i,pTar+=lockT.Pitch)
			{
				for ( UINT j=0;j<EXPAND_SIZE;++j)
				{
					memcpy(pTar+PixelSize*j,pOrc,PixelSize);
					memcpy(pTar+lockO.Pitch+PixelSize*(j+EXPAND_SIZE),pOrc+lockO.Pitch-PixelSize,PixelSize);
				}
				memcpy(pTar+PixelSize*EXPAND_SIZE,pOrc,lockO.Pitch);
			}

			pTexture->Unlock(0);
			m_pTerrainTexture->Unlock(0);
		
			m_mapTerrainTexIndex.insert(make_pair(texCnt, texname));
			m_mapTerrainTexName.insert(make_pair(texname, texCnt));
			bNeedSave = true;
		}

		if( bNeedSave )
		{
			string selfTexPath = /*this->m_strMapName + */"map.jpg";

			m_pTerrainTexture->SaveToFile(selfTexPath.c_str(),IFF_JPG);
		}
	}
	EDITOR_CATCH(exp)
	{
		pTexture->Unlock(0);
		m_pTerrainTexture->Unlock(0);

		string log = exp.ErrorMsg();
		string msg = texname + "纹理不能用于刷地表，有可能是水纹理，请检查";
		::MessageBox(NULL, msg.c_str(), "提示", MB_ICONEXCLAMATION);
		return false;
	}
	EDITOR_TRY_END;

	return bNeedSave;
}

void CTerrainTextureData::LoadTerrainTexture()
{
	CTerrainMgr* pTerrainMgr = m_pContext->GetDataScene()->GetTerrainMgr();
	if( pTerrainMgr == NULL )
		return;

	string selfTexPath = /*this->m_strMapName + */"map.jpg";
	pTerrainMgr->LoadTexture(selfTexPath.c_str());
}

void CTerrainTextureData::InsertTerrainTexture( const vector<string>& texnames )
{
	if( CreateTerrainTexture(texnames) )
		LoadTerrainTexture();
}

void CTerrainTextureData::DeleteTerrainTexture( const vector<string> &texnames )
{
	TerrainTexNamePool::iterator iter;
	string texname;
	size_t size = texnames.size();
	for ( size_t m = 0; m < size; ++m )
	{
		texname = texnames[m];

		iter = m_mapTerrainTexName.find(texname);
		if( iter != m_mapTerrainTexName.end() )
			m_mapTerrainTexName.erase(iter);
	}

	m_mapTerrainTexIndex.clear();
	
	vector<string> newtexnames;
	TerrainTexNamePool::iterator beg = m_mapTerrainTexName.begin();
	TerrainTexNamePool::iterator end = m_mapTerrainTexName.end();
	for ( beg; beg != end; ++beg )
	{
		newtexnames.push_back(beg->first);
	}

	if( !newtexnames.empty() )
	{
		if( CreateTerrainTexture(newtexnames) )
			LoadTerrainTexture();
	}
}

void CTerrainTextureData::GetTextureNameByIndex( const int& index, string& texname )
{
	TerrainTexIndexPool::const_iterator iter = m_mapTerrainTexIndex.find(index);
	if( iter->first == index )
		texname = iter->second;
	else
		texname = "";
}

int CTerrainTextureData::GetIndexByTextureName(const string& texname )
{
	TerrainTexNamePool::const_iterator iter = m_mapTerrainTexName.find(texname);
	if( iter == m_mapTerrainTexName.end() )
		return -1;

	if( iter->first == texname )
		return iter->second;

	return -1;
}
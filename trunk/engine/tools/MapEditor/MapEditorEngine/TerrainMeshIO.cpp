#include "stdafx.h"
#include "TerrainMesh.h"
#include "CTexture.h"
#include "Render.h"
#include "Engine.h"
#include "Operator.h"
#include "Operator.inl"
#include "CMapEditApp.h"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "DepathGroupModelInfo.h"
#include "IoSystem.h"
#include "VertexColorSaver.h"
#include "VertexColorLoader.h"
#include "BaseHelper.h"
#include "CWindowTarget.h"
#include "CGraphic.h"
#include "Float.h"
#include "Algorithm.h"
#include "LocalVersion.h"
#include "EditorExp.h"

//------------------------------------------------------------------------------
void
CTerrainMesh::LoadAsColor(IProgressCallback* progress)
{
	//Ast(NULL != progress);
	CTerrainMesh* terrain = CMapEditApp::GetInst()->GetEditingMesh();
	Ast(NULL != terrain);

	if ( IDNO == MessageBox(NULL, "是否载入烘培文件", "YES载入烘培文件", MB_YESNO) )
		return;

	//progress->SetDescription("载入烘焙文件...");

	VertexColorLoaderPtr loader = new VertexColorLoader();
	loader->SetTerrainMesh(terrain);
	loader->SetProcessCallback(progress);

	string path = "map:baking.cl";
	if (!loader->Load(path))
	{
		string msg = IoSystem::GetInst()->GetFullPath(path) + "载入失败";
		MessageBox(NULL, msg.c_str(), NULL, 0);
	}
}

void CTerrainMesh::SaveAsColor(IProgressCallback* progress)
{
	//Ast(NULL != progress);
	//progress->SetDescription("保存烘焙文件...");
	this->DepathMapGroupModel();

	string path = "map:baking.cl";
	VertexColorSaverPtr saver = new VertexColorSaver();
	saver->SetProcessCallback(progress);
	saver->SetTerrainMesh(this);
	if (!saver->Save(path))
	{
		string msg = IoSystem::GetInst()->GetFullPath(path) + "保存失败";
		MessageBox(NULL, msg.c_str(), NULL, 0);
	}
}

void CTerrainMesh::CreateTerrainTexture( string& texname )
{
	if ( m_pTerrainTexture == NULL )
	{
		CRenderDevice * pRender = CRenderDevice::GetInst();

		HRESULT hr = CGraphic::GetInst()->CreateEmptyTexture( EXPAND_MAX_SIZE,EXPAND_MAX_SIZE,1, TF_ARGB32,&m_pTerrainTexture );

		if ( hr != S_OK )
		{
			MessageBox(NULL,"创建拼接纹理失败","错误",MB_OK);
			return;
		}

		ITexture* pTexture = NULL;
		pTexture = pRender->GetTexture(texname);
		if (NULL==pTexture)
		{
			string log = "拼接过程:"+texname+"纹理无法找到";
			MessageBox(NULL,log.c_str(),"错误",MB_OK);
			return;
		}

		//LPDIRECT3DTEXTURE9 pTempTexture;

		//pTempTexture = (LPDIRECT3DTEXTURE9)pTexture->GetBuffer();
		if (NULL==pTexture->GetBuffer())
		{
			string log = "拼接过程:"+texname+"纹理无法找到";
			MessageBox(NULL,log.c_str(),"错误",MB_OK);
			return;
		}

		size_t texCnt = pRender->m_TerrainTexIndexs.size();

		UINT H = (texCnt/ROW_SIZE)*EXPAND_TEXTURE_SIZE;
		UINT W = (texCnt%ROW_SIZE)*EXPAND_TEXTURE_SIZE;
		//D3DSURFACE_DESC desc;
		//pTempTexture->GetLevelDesc(0,&desc);
		if (pTexture->GetWidth()!=TEXTURE_SIZE||pTexture->GetHeight()!=TEXTURE_SIZE)
		{
			string log = "拼接过程:"+texname+"尺寸不合法,所有贴图纹理应为128X128";
			MessageBox(NULL,log.c_str(),"错误",MB_OK);
		}

		CIRect rectT,rectO;
		_TEXLOCKED_RECT lockT,lockO;
		rectO.top = rectO.left = 0;
		rectO.bottom = rectO.right = TEXTURE_SIZE;
		rectT.top = H;
		rectT.bottom = H + EXPAND_TEXTURE_SIZE;
		rectT.left = W;
		rectT.right = W + EXPAND_TEXTURE_SIZE;

		hr = pTexture->Lock(0,&lockO,&rectO,0,LOCK_READONLY);
		if (  S_OK != hr )
		{
			string log = texname+"lock失败";
			MessageBox(NULL,log.c_str(),"错误",MB_OK);
			return;
		}

		hr = m_pTerrainTexture->Lock(0,&lockT,&rectT,0,LOCK_DISCARD);
		if ( S_OK != hr )
		{
			MessageBox(NULL,"拼接贴图Lock失败","错误",MB_OK);
			return;
		}

		pRender->m_TerrainTexIndexs[texname] = texCnt;

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

		/*PALETTEENTRY pale;
		pale.peBlue = 8;
		pale.peFlags = 8;
		pale.peGreen = 8;
		pale.peRed = 8;*/

		this->m_strMapName = "新世界";
		IoSystem* ioSystem = IoSystem::GetInst();
		string homePath = IoSystem::GetInst()->GetFullPath("home:");
		string selfTexPath = homePath + this->m_strMapName + ".jpg";

		//D3DXSaveTextureToFile(selfTexPath.c_str(),D3DXIFF_JPG,this->m_pTerrainTexture,&pale);
		m_pTerrainTexture->SaveToFile(selfTexPath.c_str(),IFF_JPG);
	}
}

bool CTerrainMesh::CreateTerrainTexture( vector<string> &texnames )
{
	DWORD pre = ::timeGetTime();
	if ( this->m_bWater )
	{
		return false;
	}

	if ( this->m_pTerrainTexture == NULL )
		return false;

	CRenderDevice * pRender = CRenderDevice::GetInst();

	ITexture* pTexture = NULL;
	string texname = "";
	size_t sizetexs = texnames.size();

	bool bFailedBrush = false;
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
		for ( size_t i = 0; i < sizetexs; ++ i )
		{
			texname = texnames[i];

			// 查找纹理是否已经存在，如果存在不进行拼图
			if ( pRender->m_TerrainTexIndexs.find(texname) != pRender->m_TerrainTexIndexs.end() )
				continue;

			// 纹理不存在，则先判断是否超过纹理总数255
			if ( pRender->m_TerrainTexIndexs.size() > 255 )
			{
				string log = texname+"地表纹理总数已达到255张";
				MessageBox(NULL,log.c_str(),"错误",MB_OK);

				return false;
			}

			pTexture = pRender->GetTexture(texname);

			if ( texname == "" )
				continue;

			if (NULL==pTexture)
			{
				string log = "拼接过程:"+texname+"纹理无法找到";
				MessageBox(NULL,log.c_str(),"错误",MB_OK);
				return false;
			}

			bNeedSave = true;

			if (NULL==pTexture->GetBuffer())
			{
				string log = "拼接过程:"+texname+"纹理无法找到";
				MessageBox(NULL,log.c_str(),"错误",MB_OK);
				return false;
			}

			size_t texCnt = pRender->m_TerrainTexIndexs.size();

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

			pRender->m_TerrainTexIndexs[texname] = texCnt;
		}

		if( bNeedSave )
		{
			IoSystem* ioSystem = IoSystem::GetInst();
			string homePath = IoSystem::GetInst()->GetFullPath("home:");
			string selfTexPath = homePath + this->m_strMapName + ".jpg";

			m_pTerrainTexture->SaveToFile(selfTexPath.c_str(),IFF_JPG);
			this->LoadTerrainTexture();
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

	return true;
}

bool CTerrainMesh::CreateTerrainTextureAgain( vector<string> &deletetexnames, vector<string> &texnames )
{
	if ( this->m_bWater )
	{
		return false;
	}

	CRenderDevice * pRender = CRenderDevice::GetInst();

	///删掉原有纹理相关信息
	map<string, DWORD>::iterator iter;
	string texname = "";
	size_t deletesize = deletetexnames.size();
	size_t npos = 0;

	for ( size_t i = 0; i < deletesize; ++i )
	{
		texname = deletetexnames[i];

		size_t npos = texname.find("TILE\\地编专用\\TEX");

		if ( npos != -1 )
		{
			string log = texname+"不能删除";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
			return false;
		}

		if ( !pRender->DeleteTexture(texname) )
		{
			string log = texname+"删除失败";
			MessageBox(NULL,log.c_str(),"错误",MB_OK);

			return false;
		}

		iter = pRender->m_TerrainTexIndexs.find(texname);
		if ( iter != pRender->m_TerrainTexIndexs.end() )
		{
			pRender->m_TerrainTexIndexs.erase(iter);
		}
	}

	if ( this->m_pTerrainTexture == NULL )
		return false;
	else
	{
		SafeRelease(m_pTerrainTexture);
		CGraphic::GetInst()->CreateEmptyTexture( EXPAND_MAX_SIZE,EXPAND_MAX_SIZE,1, TF_ARGB32,&m_pTerrainTexture );
		//GetD3DDevice()->CreateTexture(EXPAND_MAX_SIZE, EXPAND_MAX_SIZE, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &this->m_pD3DTerrainTexture, NULL );
	}

	CIRect rectT;
	_TEXLOCKED_RECT lockT;

	size_t sizetexs = texnames.size();
	size_t texCnt = pRender->m_TerrainTexIndexs.size();

	for ( size_t i = 0; i < sizetexs; ++ i )
	{
		texname = texnames[i];
		pRender->m_TerrainTexIndexs[texname] = texCnt;
		++texCnt;
	}

	sizetexs = pRender->m_TerrainTexIndexs.size();
	iter = pRender->m_TerrainTexIndexs.begin();
	texCnt = 0;
	for ( size_t i = 0; i < sizetexs; ++ i )
	{
		texname = iter->first;
		pRender->m_TerrainTexIndexs[texname] = texCnt;
		++texCnt;
		++iter;
	}

	ITexture* pTexture = NULL;
	EDITOR_TRY
	{
		texCnt = pRender->m_TerrainTexIndexs.size();
		iter = pRender->m_TerrainTexIndexs.begin();
		for ( size_t i = 0; i < texCnt; ++i )
		{
			texname = iter->first;

			// 纹理不存在，则先判断是否超过纹理总数255
			if ( pRender->m_TerrainTexIndexs.size() > 255 )
			{
				string log = texname+"地表纹理总数已达到255张";
				MessageBox(NULL,log.c_str(),"错误",MB_OK);

				return false;
			}

			pTexture = pRender->GetTexture(texname);
			if (NULL==pTexture)
			{
				string log = "拼接过程:"+texname+"纹理无法找到";
				MessageBox(NULL,log.c_str(),"错误",MB_OK);
				return false;
			}

			if (NULL==pTexture->GetBuffer())
			{
				string log = "拼接过程:"+texname+"纹理无法找到";
				MessageBox(NULL,log.c_str(),"错误",MB_OK);
				return false;
			}

			UINT H = (i/ROW_SIZE)*EXPAND_TEXTURE_SIZE;
			UINT W = (i%ROW_SIZE)*EXPAND_TEXTURE_SIZE;

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

			if ( FAILED( m_pTerrainTexture->Lock(0,&lockT,&rectT,0,LOCK_DISCARD) ) )
			{
				MessageBox(NULL,"lock失败","错误",MB_OK);
				return false;
			}

			if ( FAILED( pTexture->Lock(0,&lockO,&rectO,0,LOCK_READONLY) ) )
			{
				MessageBox(NULL,"lock失败","错误",MB_OK);
				return false;
			}

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

			++iter;
		}

		IoSystem* ioSystem = IoSystem::GetInst();
		string homePath = IoSystem::GetInst()->GetFullPath("home:");
		string selfTexPath = homePath + this->m_strMapName + ".jpg";

		//PALETTEENTRY pale;
		//pale.peBlue = 8;
		//pale.peFlags = 8;
		//pale.peGreen = 8;
		//pale.peRed = 8;
		m_pTerrainTexture->SaveToFile(selfTexPath.c_str(),IFF_JPG);
		//D3DXSaveTextureToFile(selfTexPath.c_str(),D3DXIFF_JPG,this->,&pale);
		this->LoadTerrainTexture();
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

	return true;
}

void CTerrainMesh::DepathGroupModel(index_t nodeIndex)
{
// 	this->m_DepathModelMap.clear();
// 	COperator *pOperator = CMapEditApp::GetInst()->GetOperator();
// 	typedef map<int, vector<string>> 	DepathObjectMap;
// 
// 	///找出复合图素
// 	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetTerrain();
// 	DWORD		  dwGridCntInTileRegion = pTerrain->GetGridCountInTileRegion();
// 	DWORD		  dwTileRegionCnt		= pTerrain->GetTileRegionCount();
// 	int			  nGridIndex = 0;
// 	int16		  nFirstTextureIndex = 0, nSecondTextureIndex = 0;
// 	string		  strName = "";
// 
// 
// 	STileRegion *pTileRegion = pTerrain->GetTileRegion(nodeIndex);
// 	for (DWORD i = 0; i < dwGridCntInTileRegion; ++i )
// 	{
// 		nGridIndex = pTileRegion->vGridIndices[i];
// 		SGrid &grid = pTerrain->GetGrid(nGridIndex);
// 
// 		if ( grid.vecCenterObjects.empty() )
// 			continue;
// 
// 		int nFaceIndex = 0;
// 		set<string>		setEffectName;
// 		size_t nModelIndex = 0;
// 
// 		for ( vector< CTObjectPtr >::iterator iter = grid.vecCenterObjects.begin(); iter != grid.vecCenterObjects.end(); ++iter, ++nFaceIndex)
// 		{
// 			CTObject * p = (*iter);
// 			CEditModelGroup* pModelGroup = p->GetModelGroup();
// 			if ( pModelGroup == NULL )
// 				continue;
// 
// 			if ( !pModelGroup->GetIsSubUnit() )
// 				continue;
// 
// 			pOperator->SetGridIndexHot(nGridIndex);
// 			string strAgpName = pModelGroup->GetAnimationGroupName();
// 			string strSkeName = pModelGroup->GetAnimationSkeName();
// 			string strEffectName, strEffectFileName;
// 			pModelGroup->GetEffectName(strEffectName);
// 			pModelGroup->GetEffectFileName(strEffectFileName);
// 
// 			float fLift = pModelGroup->GetLiftx();
// 			float fShiftX = pModelGroup->GetShiftX();
// 			float fShiftZ = pModelGroup->GetShiftZ();
// 
// 			float fLiftEx = pModelGroup->GetLiftEx();
// 			float fShiftXEx = pModelGroup->GetShiftXEx();
// 			float fShiftZEx = pModelGroup->GetShiftZEx();
// 
// 			float x = 0.0f, y = 0.0f, z = 0.0f;
// 			float fRotationX = 0.0f,fRotationY = 0.0f,fRotationZ = 0.0f,fscalex = 0.0f,fscaley = 0.0f,fscalez = 0.0f;
// 
// 			sqr::ECenter_Type eCenterType = pModelGroup->GetCenterType();
// 			byte rs = pModelGroup->GetRenderStyle();
// 			int atten = pModelGroup->GetAtten();
// 
// 			bool ShadowSecondPass = pModelGroup->GetShadowSecondPass();
// 			bool bRecieveShadow = pModelGroup->IsReceiveShadow();
// 			bool Shadow = pModelGroup->IsCastShadow();
// 			bool translucent = pModelGroup->GetIsTranslucent();
// 			bool ZTest = false;
// 			bool SelfZTest = false;
// 			bool IsCameraOriented = pModelGroup->GetIsCameraOriented();
// 			bool IsEffectBased = pModelGroup->GetIsEffectBased();
// 			bool bPlayAni = pModelGroup->GetIsPlayAnimation();
// 			bool bCameraCollision = pModelGroup->GetIsUseCameraCollison();
// 			bool bVisibleByCofigue = pModelGroup->GetIsVisibleByConfigure();
// 
// 			float r,g,b;
// 			pModelGroup->GetOverallAmbientRatio(r,g,b);
// 
// 			const vector<string>& modelnames = pModelGroup->GetModelFileNames();
// 			int nXGroupOffset = 0, nZGroupOffset = 0;
// 			size_t modelsize = modelnames.size();
// 			for ( size_t m = 0; m < modelsize; ++m )
// 			{
// 				const string& strArpName = modelnames[m];
// 				size_t npos = strArpName.rfind("\\");
// 				string newstrItemName = strArpName.substr(npos+1, strArpName.length() - npos);
// 				npos = newstrItemName.find(".");
// 				newstrItemName = newstrItemName.substr(0, npos);
// 
// 				CMapEditObject * pModel  = pModelGroup->GetSelfPresentation(m);
// 				CVector3f vOffset		 = pModel->GetModelOffset();
// 				CVector3f vOffsetOverall = pModel->GetModelOffsetOverall();
// 				CVector3f userOffsetEx	 = pModel->GetUserOffsetEx();
// 				pModel->GetImageOffsetToAdjustedCenterGrid(x,y,z,fRotationX,fRotationY,fRotationZ,fscalex,fscaley,fscalez);
// 
// 				CVector3f v = vOffset - vOffsetOverall;
// 
// 				nXGroupOffset = (int)((v.x / 64.0f) + 0.5f * sign(v.x));
// 				nZGroupOffset = (int)((v.z / 64.0f) + 0.5f * sign(v.z));
// 
// 				int nHotGridIndex = nGridIndex + nXGroupOffset + nZGroupOffset * m_dwWidth;
// 
// 				DepathModelInfoPtr pDepathModelInfo = new DepathModelInfo;
// 
// 				pDepathModelInfo->modelName = strArpName;
// 				pDepathModelInfo->modelNames = vector<string>(1, strArpName);
// 				pDepathModelInfo->strItemName = newstrItemName;
// 				pDepathModelInfo->agpName = strAgpName;
// 				pDepathModelInfo->skeName = strSkeName;
// 
// 				if (!strEffectName.empty() && setEffectName.end() == setEffectName.find(strEffectName))
// 				{
// 					setEffectName.insert(strEffectName);
// 					pDepathModelInfo->effectName = strEffectName;
// 				}
// 
// 				pDepathModelInfo->effectFileName = strEffectFileName;
// 				pDepathModelInfo->bShadow = Shadow;
// 				pDepathModelInfo->bSecondShadow = ShadowSecondPass;
// 				pDepathModelInfo->bRecieveShadow = bRecieveShadow;
// 				pDepathModelInfo->bZTest = ZTest;
// 				pDepathModelInfo->bSelfZTest = SelfZTest;
// 				pDepathModelInfo->bTranslucent = translucent;
// 				pDepathModelInfo->byRenderStyle = rs;
// 				pDepathModelInfo->bPlayAnimate = bPlayAni;
// 				pDepathModelInfo->bVisibleByConfigure = bVisibleByCofigue;
// 				pDepathModelInfo->bUseCameraCollision = bCameraCollision;
// 
// 				pDepathModelInfo->fRotationX = fRotationX;
// 				pDepathModelInfo->fRotationY = fRotationY;
// 				pDepathModelInfo->fRotationZ = fRotationZ;
// 				pDepathModelInfo->fScaleX = fscalex;
// 				pDepathModelInfo->fScaleY = fscaley;
// 				pDepathModelInfo->fScaleZ = fscalez;
// 
// 				pDepathModelInfo->fX = x;
// 				pDepathModelInfo->fY = y;
// 				pDepathModelInfo->fZ = z;
// 
// 				pDepathModelInfo->r = r;
// 				pDepathModelInfo->g = g;
// 				pDepathModelInfo->b = b;
// 
// 				pDepathModelInfo->nOriginGridIndex = nGridIndex;
// 
// 				///editor model info
// 				pDepathModelInfo->vUserOffsetEx  = userOffsetEx;
// 				pDepathModelInfo->bBakingColor	 = pModel->GetBakingColorIsNotGreaterShadow();
// 				pDepathModelInfo->nMirrorType	 = pModel->GetMirrorType();
// 				pDepathModelInfo->bTransState	 = pModel->GetModelTransState();
// 
// 				this->m_DepathModelMap[nHotGridIndex].push_back(pDepathModelInfo);
// 				size_t size = this->m_DepathModelMap[nHotGridIndex].size();
// 				++pTileRegion->nMultiModelCnt;
// 			}
// 		}
// 	}

}

void CTerrainMesh::DepathMapGroupModel()
{
	this->m_DepathModelMap.clear();
	COperator *pOperator = CMapEditApp::GetInst()->GetOperator();
	typedef map<int, vector<string>> 	DepathObjectMap;

	///找出复合图素
	for (int i = 0; i < this->GetGridCount(); ++i )
	{
		SGrid *pGrid = &(this->GetGrid(i));
		int nGridIndex = pGrid->dwGridIndex;

		if ( pGrid->vecCenterObjects.empty() )
			continue;

		int nFaceIndex = 0;
		set<string>		setEffectName;
		size_t nModelIndex = 0;

		for ( vector< CTObjectPtr >::iterator iter = pGrid->vecCenterObjects.begin();
			iter != pGrid->vecCenterObjects.end(); ++iter, ++nFaceIndex)
		{

			CTObject * p = (*iter);
			CEditModelGroup* pModelGroup = p->GetModelGroup();
			if ( pModelGroup == NULL )
				continue;
			if ( !pModelGroup->GetIsSubUnit() )
				continue;

			pOperator->SetGridIndexHot(nGridIndex);
			string strAgpName = pModelGroup->GetAnimationGroupName();
			string strSkeName = pModelGroup->GetAnimationSkeName();
			string strEffectName, strEffectFileName;
			pModelGroup->GetEffectName(strEffectName);
			pModelGroup->GetEffectFileName(strEffectFileName);

			float fLift = pModelGroup->GetLiftx();
			float fShiftX = pModelGroup->GetShiftX();
			float fShiftZ = pModelGroup->GetShiftZ();

			float fLiftEx = pModelGroup->GetLiftEx();
			float fShiftXEx = pModelGroup->GetShiftXEx();
			float fShiftZEx = pModelGroup->GetShiftZEx();

			float x = 0.0f, y = 0.0f, z = 0.0f;
			float fRotationX = 0.0f,fRotationY = 0.0f,fRotationZ = 0.0f,fscalex = 0.0f,fscaley = 0.0f,fscalez = 0.0f;

			sqr::ECenter_Type eCenterType = pModelGroup->GetCenterType();
			byte rs = pModelGroup->GetRenderStyle();
			int atten = pModelGroup->GetAtten();

			bool ShadowSecondPass = pModelGroup->GetShadowSecondPass();
			bool bRecieveShadow = pModelGroup->IsReceiveShadow();
			bool Shadow = pModelGroup->IsCastShadow();
			bool translucent = pModelGroup->GetIsTranslucent();
			bool ZTest = false;
			bool SelfZTest = false;
			bool IsCameraOriented = pModelGroup->GetIsCameraOriented();
			bool IsEffectBased = pModelGroup->GetIsEffectBased();
			bool bPlayAni = pModelGroup->GetIsPlayAnimation();
			bool bCameraCollision = pModelGroup->GetIsUseCameraCollison();
			bool bVisibleByCofigue = pModelGroup->GetIsVisibleByConfigure();

			byte r,g,b;
			pModelGroup->GetOverallAmbientRatio(r,g,b);

			const vector<string>& modelnames = pModelGroup->GetModelFileNames();
			int nXGroupOffset = 0, nZGroupOffset = 0;
			size_t size = modelnames.size();
			for ( size_t m = 0; m < size; ++m )
			{
				const string& strArpName = modelnames[m];
				size_t npos = strArpName.rfind("\\");
				string newstrItemName = strArpName.substr(npos+1, strArpName.length() - npos);
				npos = newstrItemName.find(".");
				newstrItemName = newstrItemName.substr(0, npos);

				CMapEditObject * pModel = pModelGroup->GetSelfPresentation(m);
				CVector3f vOffset = pModel->GetModelOffset();
				CVector3f vOffsetOverall = pModel->GetModelOffsetOverall();
				pModel->GetImageOffsetToAdjustedCenterGrid(x,y,z,fRotationX,fRotationY,fRotationZ,fscalex,fscaley,fscalez);

				CVector3f v = vOffset - vOffsetOverall;

				nXGroupOffset = (int)((v.x / 64.0f) + 0.5f * sign(v.x));
				nZGroupOffset = (int)((v.z / 64.0f) + 0.5f * sign(v.z));

				int nHotGridIndex = nGridIndex + nXGroupOffset + nZGroupOffset * m_dwWidth;

				DepathModelInfoPtr pDepathModelInfo = new DepathModelInfo;

				pDepathModelInfo->modelName = strArpName;
				pDepathModelInfo->modelNames = vector<string>(1, strArpName);
				pDepathModelInfo->strItemName = newstrItemName;
				pDepathModelInfo->agpName = strAgpName;
				pDepathModelInfo->skeName = strSkeName;

				if (!strEffectName.empty() && setEffectName.end() == setEffectName.find(strEffectName))
				{
					setEffectName.insert(strEffectName);
					pDepathModelInfo->effectName = strEffectName;
				}

				pDepathModelInfo->effectFileName = strEffectFileName;
				pDepathModelInfo->bShadow = Shadow;
				pDepathModelInfo->bSecondShadow = ShadowSecondPass;
				pDepathModelInfo->bRecieveShadow = bRecieveShadow;
				pDepathModelInfo->bZTest = ZTest;
				pDepathModelInfo->bSelfZTest = SelfZTest;
				pDepathModelInfo->bTranslucent = translucent;
				pDepathModelInfo->byRenderStyle = rs;
				pDepathModelInfo->bPlayAnimate = bPlayAni;
				pDepathModelInfo->bVisibleByConfigure = bVisibleByCofigue;
				pDepathModelInfo->bUseCameraCollision = bCameraCollision;

				pDepathModelInfo->fRotationX = fRotationX;
				pDepathModelInfo->fRotationY = fRotationY;
				pDepathModelInfo->fRotationZ = fRotationZ;
				pDepathModelInfo->fScaleX = fscalex;
				pDepathModelInfo->fScaleY = fscaley;
				pDepathModelInfo->fScaleZ = fscalez;

				pDepathModelInfo->fX = x;
				pDepathModelInfo->fY = y;
				pDepathModelInfo->fZ = z;

				pDepathModelInfo->r = r;
				pDepathModelInfo->g = g;
				pDepathModelInfo->b = b;

				pDepathModelInfo->nOriginGridIndex = nGridIndex;
				pDepathModelInfo->nGridModelIndex  = nModelIndex;			 ///该图素在属于该格子（nOriginGridIndex）中的第几个modelgroup
				pDepathModelInfo->nGroupModelIndex = m;///该图素在复合图素中是第几个位置

				if( nHotGridIndex < 0 )
					MessageBox(NULL,"格子索引小于0，请检查","提示",MB_OK);

				this->m_DepathModelMap[nHotGridIndex].push_back(pDepathModelInfo);
				//DebugOut("hot grid %d, %s\n", nHotGridIndex, strArpName.c_str());
			}

			++nModelIndex;
		}
	}
}

///有个问题是：主路和辅路用的纹理是否存在一样的情况
void CTerrainMesh::CalTerrainRoadWeight()
{
	DWORD terrainCnt = 0, alpha = 0;
	int weight = 0;
	string texfirstname = "", texsecondname = "";
	terrainCnt = this->GetGridCount();
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
	map<string, int>::iterator iter;

	for (DWORD i = 0; i < terrainCnt; ++i )
	{
		SGrid &grid = pTerrain->GetGrid(i);

		texfirstname = grid.nodeInfo.strTextureNames[0];
		texsecondname = grid.nodeInfo.strTextureNames[1];

		SVertex &vertex0 = pTerrain->GetVertex(grid.dwVertexIndex[0]);
		SVertex &vertex1 = pTerrain->GetVertex(grid.dwVertexIndex[3]);
		SVertex &vertex2 = pTerrain->GetVertex(grid.dwVertexIndex[1]);
		SVertex &vertex3 = pTerrain->GetVertex(grid.dwVertexIndex[2]);
		alpha = ((vertex0.oColor >> 24) + (vertex1.oColor >> 24) + (vertex2.oColor >> 24) + (vertex3.oColor >> 24)) / 4;

		///计算辅路权重
		///第一层就是道路贴图
		iter = pTerrain->m_MainRoadWeight.find(texfirstname);
		if ( iter != pTerrain->m_MainRoadWeight.end() )
		{
			weight = iter->second;

			///只有一层贴图
			if ( texsecondname == "" )
			{
				grid.nGridSeekRoadWeight = weight;
			}
			else
			{
				iter = pTerrain->m_MainRoadWeight.find(texsecondname);
				///2层都是道路贴图
				if ( iter != pTerrain->m_MainRoadWeight.end() )
				{
					///如果第一层占的比重大，就用第一层的贴图权重；否则用第2层的贴图权重
					if ( alpha < 127 )
					{
						grid.nGridSeekRoadWeight = weight;
					}
					else
					{
						grid.nGridSeekRoadWeight = iter->second;
					}
				}
				///2层不是道路贴图
				else
				{
					///如果第一层道路贴图的比重大于第2层非道路贴图，仍设为第一层道路贴图权重
					if ( alpha < 127 )
					{
						grid.nGridSeekRoadWeight = weight;
					}
				}
			}
		}
		else
		{
			if ( texsecondname != "" )
			{
				iter = pTerrain->m_MainRoadWeight.find(texsecondname);
				///第一层不是权重贴图，但第2层是权重贴图
				if ( iter != pTerrain->m_MainRoadWeight.end() )
				{
					if ( alpha >= 127 )
					{
						grid.nGridSeekRoadWeight = weight;
					}
				}
			}

		}

	}
}

void CTerrainMesh::WipeOffUnUseGridTextureInfo()
{
	DWORD terrainCnt = 0, alpha0 = 0, alpha1 = 0, alpha2 = 0, alpha3 = 0;
	int weight = 0;
	string texfirstname = "", texsecondname = "";
	terrainCnt = this->GetGridCount();
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

	for (DWORD i = 0; i < terrainCnt; ++i )
	{
		SGrid &grid = pTerrain->GetGrid(i);

		texfirstname = grid.nodeInfo.strTextureNames[0];
		texsecondname = grid.nodeInfo.strTextureNames[1];

		if ( texsecondname != "" )
		{
			SVertex &vertex0 = pTerrain->GetVertex(grid.dwVertexIndex[0]);
			SVertex &vertex1 = pTerrain->GetVertex(grid.dwVertexIndex[3]);
			SVertex &vertex2 = pTerrain->GetVertex(grid.dwVertexIndex[1]);
			SVertex &vertex3 = pTerrain->GetVertex(grid.dwVertexIndex[2]);
			alpha0 = vertex0.oColor >> 24;
			alpha1 = vertex1.oColor >> 24;
			alpha2 = vertex2.oColor >> 24;
			alpha3 = vertex3.oColor >> 24;

			///如果grid的四个点个alpha都为0，表示第2层纹理无用
			if ( alpha0 == 0 && alpha1 == 0 && alpha2 == 0 && alpha3 == 0 )
			{
				grid.nodeInfo.strTextureNames[1] = "";
			}
			else
			{
				///如果grid的四个点个alpha都为1，表示第1层纹理完全被遮盖；可以将第2层纹理直接赋给第1层，第2层清空
				if ( alpha0 == 1 && alpha1 == 1 && alpha2 == 1 && alpha3 == 1 )
				{
					grid.nodeInfo.strTextureNames[0] = texsecondname;
					grid.nodeInfo.strTextureNames[1] = "";
				}
			}
		}
	}
}

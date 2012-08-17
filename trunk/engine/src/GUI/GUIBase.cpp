#include "stdafx.h"
#include "GUIBase.h"
#include "SQRWnd.h"
#include "CPkgFile.h"
#include "BaseHelper.h"
#include "CTexture.h"
#include "CModel.h"
#include "SQRGUIManager.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"
#include "CSkeletalsMgr.h"
// ----------------------------------------------------------------------------------------
// 常量定义
// ----------------------------------------------------------------------------------------
namespace sqr
{
const float FLOAT_MAX_VALUE = 3.402823466e+38F;
const float FLOAT_MIN_VALUE = 1.175494351e-38F;
const float FLOAT_EPSILON   = 1.192092896e-07F;
}

const uint32 BLOCK_POOL::MAX_BLOCK_NUM = DEF_MAX_BLOCK_NUM;

//////////////////////////////////////////////////////////////////////////
// WND_IMAGE_LIST
//////////////////////////////////////////////////////////////////////////


WND_IMAGE_LIST::WND_IMAGE_LIST( const WND_IMAGE_LIST& ImageList )
{
	m_pImageList = new VEC_IMAGE;
	m_IsAsynEnable = false;
	*this = ImageList;
}

WND_IMAGE_LIST::WND_IMAGE_LIST()
{
	m_pImageList = new VEC_IMAGE;
	m_IsAsynEnable = false;
}

WND_IMAGE_LIST::~WND_IMAGE_LIST()
{
	Release();
	delete m_pImageList;
}

void WND_IMAGE_LIST::Release()
{
	for ( int32 i = 0; i < GetImageCount(); i++ )
		SafeRelease( GetImage(i).pTexture );
	m_pImageList->Image.clear();
}


int32 WND_IMAGE_LIST::AddImageFromImageList( const WND_IMAGE_LIST & ImageList, int32 nIndexBegin, int32 nIndexEnd )
{
	if ( this == &ImageList )
		return -1;
	UIVector<IMAGE>& DesImageList = m_pImageList->Image;
	UIVector<IMAGE>& SrcImageList = ImageList.m_pImageList->Image;
	nIndexEnd = (int32)min( ( (uint32)nIndexEnd ), SrcImageList.size() );
	nIndexBegin = max( nIndexBegin, 0 );
	for ( int32 i = nIndexBegin; i < nIndexEnd; i++ )
	{
		DesImageList.push_back( SrcImageList[i] );
		if ( SrcImageList[i].pTexture )
			SrcImageList[i].pTexture->AddRef();
	}
	return DesImageList.size();
}

void WND_IMAGE_LIST::SetGUIAsynEnable(bool enable)
{
	m_IsAsynEnable = enable;
}

bool WND_IMAGE_LIST::IsImageLoadValid()
{
	UIVector<IMAGE>::iterator iter = m_pImageList->Image.begin();
	while(iter!=m_pImageList->Image.end())
	{
		if(!(*iter).pTexture)
			return true;
		if(!(*iter).pTexture->IsValid())
			return false;
		iter++;
	}
	return true;
}

int32 WND_IMAGE_LIST::AddImage( IGraphic* pGraphic, int32 nImageIndex, const char* szTextName,
                                const CFRect* prt, const CFPos& posWnd, uint32 uColor, uint32 uLockFlag )
{
	UIString strTexName;
	if ( szTextName && szTextName[0] != 0)
	{
		strTexName = szTextName;
	}

	UIVector<IMAGE>& ImageList = m_pImageList->Image;

	IMAGE Image;
	ITexture* pTexture = NULL;

	if ( szTextName && szTextName[0] )
	{
		bool bEnableASynRead = CPkgFile::IsEnableAsyncRead();
		CPkgFile::EnableAsyncRead( m_IsAsynEnable );
		HRESULT hr = pGraphic->CreateTexture( GUI_PATH_ALIAS_GUITEX.c_str(), strTexName.c_str(), &pTexture ); // szTextName
		if(FAILED(hr))
			OutputDebugString(strTexName.c_str());
		CPkgFile::EnableAsyncRead( bEnableASynRead );
		if ( !pTexture )
		{
			return -1;
		}
	}

	Image.pTexture			= pTexture;
	Image.texName			= strTexName;
	Image.rtTexture			= prt ? *prt : ( pTexture ? CFRect( 0, 0, (float)pTexture->GetWidth(), (float)pTexture->GetHeight() ) : CFRect( 0, 0, 0, 0 ) );
	Image.posWnd			= posWnd;
	Image.dwColor			= uColor;
	Image.dwLockFlag		= uLockFlag;
	Image.bWidthNegtvDir	= Image.rtTexture.Width() < 0;
	Image.bHeightNegtvDir	= Image.rtTexture.Height() < 0;

	nImageIndex = (int32)( min( uint32( nImageIndex ), ImageList.size() ) );
	ImageList.insert( ImageList.begin() + nImageIndex, Image );

	return nImageIndex;
}

int32 WND_IMAGE_LIST::AddImageWin( IGraphic* pGraphic, int32 nImageIndex, const char* szTextName,
							   const CFRect* prt, const CFPos& posWnd, uint32 uColor, uint32 uLockFlag )
{
	UIString strTexName;
	if ( szTextName && szTextName[0] != 0)
	{
		strTexName = szTextName;
	}

	UIVector<IMAGE>& ImageList = m_pImageList->Image;

	IMAGE Image;
	ITexture* pTexture = NULL;
#ifdef TEXTURE_STINT_MODE
	Image.pTexture			= NULL;
#else

	if ( szTextName && szTextName[0] )
	{
		bool bEnableASynRead = CPkgFile::IsEnableAsyncRead();
		CPkgFile::EnableAsyncRead( m_IsAsynEnable );
		HRESULT hr = pGraphic->CreateTexture( GUI_PATH_ALIAS_GUITEX.c_str(), strTexName.c_str(), &pTexture ); // szTextName
		if(FAILED(hr))
			OutputDebugString(strTexName.c_str());
		CPkgFile::EnableAsyncRead( bEnableASynRead );
		if ( !pTexture )
		{
			return -1;
		}
	}

	Image.pTexture			= pTexture;
#endif

	Image.texName			= strTexName;
	Image.rtTexture			= prt ? *prt : ( pTexture ? CFRect( 0, 0, (float)pTexture->GetWidth(), (float)pTexture->GetHeight() ) : CFRect( 0, 0, 0, 0 ) );
	Image.posWnd			= posWnd;
	Image.dwColor			= uColor;
	Image.dwLockFlag		= uLockFlag;
	Image.bWidthNegtvDir	= Image.rtTexture.Width() < 0;
	Image.bHeightNegtvDir	= Image.rtTexture.Height() < 0;

	nImageIndex = (int32)( min( uint32( nImageIndex ), ImageList.size() ) );
	ImageList.insert( ImageList.begin() + nImageIndex, Image );

	return nImageIndex;
}

int32 WND_IMAGE_LIST::GetImageCount() const
{
	return (int32)m_pImageList->Image.size();
}

IMAGE& WND_IMAGE_LIST::GetImage( int32 nImageIndex )const
{
	if (nImageIndex < 0 || nImageIndex >= static_cast<int32>(m_pImageList->Image.size()))
		GenErr("WND_IMAGE_LIST error","GetImage Index越界");

	return m_pImageList->Image[nImageIndex];
}

void WND_IMAGE_LIST::GetImage(int32 nImageIndex, IMAGE& image)
{
	image = GetImage(nImageIndex);
}

void WND_IMAGE_LIST::DeleteImage( int32 nImageIndex )
{
	if (nImageIndex <= -1)
	{
		ClearImage();
	}
	else if ( nImageIndex < GetImageCount() )
	{
		SafeRelease( GetImage( nImageIndex ).pTexture );
		m_pImageList->Image.erase( ( ( VEC_IMAGE* )m_pImageList )->Image.begin() + nImageIndex );
	}
}

void WND_IMAGE_LIST::ClearImage()
{
	UIVector<IMAGE>& vec=m_pImageList->Image;
	for (size_t i=0;i<vec.size();++i)
		SafeRelease( vec[i].pTexture );
	vec.clear();
}

int32 WND_IMAGE_LIST::DetectImage( const CFPos& CursorInWnd )
{
	UIVector<IMAGE>& ImageList = m_pImageList->Image;
	for ( int32 i = ImageList.size() - 1; i >= 0; i-- )
	{
		IMAGE& Image = ImageList[i];
		if ( CursorInWnd.x > Image.posWnd.x && CursorInWnd.x < Image.posWnd.x + abs( Image.rtTexture.Width() ) &&
		        CursorInWnd.y > Image.posWnd.y && CursorInWnd.y < Image.posWnd.y + abs( Image.rtTexture.Height() ) )
			return i;
	}
	return -1;
}

void WND_IMAGE_LIST::ReleaseTex()
{
	UIVector<IMAGE>& vec=m_pImageList->Image;

	for ( size_t i = 0; i < vec.size(); ++i )
	{
		if ( vec[i].pTexture )
		{
			SafeRelease( vec[i].pTexture );
		}
	}
}

void WND_IMAGE_LIST::CreatTex( IGraphic* pGraphic, GUITextureHelper* texhelp )
{
	UIVector<IMAGE>& Image=m_pImageList->Image;

	for ( size_t i = 0; i < Image.size(); ++i )
	{
		if (Image[i].pTexture)
			continue;

		ITexture* pTexture = NULL;
		if ( Image[i].texName.length() > 0 && Image[i].texName[0] )
		{
			bool bEnableASynRead = CPkgFile::IsEnableAsyncRead();
			if (texhelp)
				CPkgFile::EnableAsyncRead( texhelp->IsAsynLoad() );
			else
				CPkgFile::EnableAsyncRead( m_IsAsynEnable );
			HRESULT hr = pGraphic->CreateTexture( GUI_PATH_ALIAS_GUITEX.c_str(), Image[i].texName.c_str(), &pTexture ); // szTextName
			if(FAILED(hr))
				OutputDebugString(Image[i].texName.c_str());
			CPkgFile::EnableAsyncRead( bEnableASynRead );
			if ( !pTexture )
				continue;
			if (texhelp)
				texhelp->InsertTexture(pTexture);
		}

		Image[i].pTexture			= pTexture;
		if (Image[i].rtTexture.IsEmpty())
			Image[i].rtTexture			= ( pTexture ? CFRect( 0, 0, (float)pTexture->GetWidth(), (float)pTexture->GetHeight() ) : CFRect( 0, 0, 0, 0 ) );

	}
}

void WND_IMAGE_LIST::Scale( float xDel, float yDel )
{
	for ( int32 i = 0; i < GetImageCount(); i++ )
	{
		IMAGE& Image = GetImage( i );
		if ( Image.dwLockFlag&LOCK_RIGHT )
		{
			if ( Image.dwLockFlag&LOCK_LEFT )
				Image.rtTexture.right += Image.bWidthNegtvDir ? -xDel : xDel;
			else
				Image.posWnd.x = Image.posWnd.x + xDel;
		}

		if ( Image.dwLockFlag&LOCK_BOTTOM )
		{
			if ( Image.dwLockFlag&LOCK_TOP )
				Image.rtTexture.bottom += Image.bHeightNegtvDir ? -yDel : yDel;
			else
				Image.posWnd.y = Image.posWnd.y + yDel;
		}
	}
}

const WND_IMAGE_LIST& WND_IMAGE_LIST::operator = ( const WND_IMAGE_LIST& ImageList )
{
	if ( this == &ImageList )
		return *this;

	Release();
	UIVector<IMAGE>& DesImageList = m_pImageList->Image;
	UIVector<IMAGE>& SrcImageList = ImageList.m_pImageList->Image;
	for ( int32 i = 0; i < (int32)SrcImageList.size(); i++ )
	{
		DesImageList.push_back( SrcImageList[i] );
		if ( SrcImageList[i].pTexture )
			SrcImageList[i].pTexture->AddRef();
	}
	return *this;
}


//////////////////////////////////////////////////////////////////////////
// WND_IMAGE_LIST
//////////////////////////////////////////////////////////////////////////
//struct WND_RENDER_OBJ_STRUCT
//{
//	WND_RENDER_OBJ_STRUCT() : m_pRenderObj(NULL), m_pGraphic(NULL), m_pFxMgr(NULL), m_vecDir( 0, 1 ) {}
//	~WND_RENDER_OBJ_STRUCT()
//	{
//		Clear();
//	}
//
//	struct ModelInfo
//	{
//		UIString sArpFile;
//		UIString sClass;
//		uint32 uRS;
//	};
//
//	struct FxInfo
//	{
//		UIString sFxFile;
//		UIString sFxName;
//	};
//
//	UIVector<ModelInfo>	m_vecModelInfo;
//	UIVector<FxInfo>		m_vecFxInfo;
//	UIString				m_sAraFile;
//	UIString				m_sAgpFile;
//	IGraphic*			m_pGraphic;
//	IEffectManager*		m_pFxMgr;
//	CRenderable*		m_pRenderObj;
//	//CAnimationsRes*		m_pAniGroup;
//	CSkeletalFrame*		m_pSkeletalFrame;
//	CAnimationGroup*	m_pAnimateGroup;
//	float				m_fScale;
//	CVector3f			m_vecOff;
//	CVector2f			m_vecDir;
//	IDNAME				m_sAniName;
//	FramePair			m_FramePair;
//	bool				m_bLoop;
//
//	void Clear()
//	{
//		m_vecModelInfo.clear();
//		m_vecFxInfo.clear();
//		m_sAraFile.clear();
//		m_sAgpFile.clear();
//		m_pGraphic = NULL;
//		m_pFxMgr = NULL;
//		//m_pAniGroup = NULL;
//		m_pSkeletalFrame = NULL;
//		m_pAnimateGroup = NULL;
//		SafeRelease( m_pRenderObj );
//	}
//
//	bool CheckAniValid()
//	{
//		if( !m_pSkeletalFrame&&!m_pAnimateGroup)
//			return true;
//
//		if ( m_pSkeletalFrame && m_pSkeletalFrame->IsValid() &&m_pAnimateGroup&&m_pAnimateGroup->IsValid()&& m_pRenderObj && m_pRenderObj->GetType() == ROT_MODEL )
//		{
//			((CModel*)m_pRenderObj)->SetSkeletalFrame( m_pSkeletalFrame);
//			((CModel*)m_pRenderObj)->SetAnimateGroup( m_pAnimateGroup );
//			((CModel*)m_pRenderObj)->SetNextAnimation( m_sAniName, m_FramePair, m_bLoop, 0 );
//			//SafeRelease( m_pSkeletalFrame );
//			SafeRelease( m_pAnimateGroup );
//			return true;
//		}
//
//		return false;
//	}
//
//	void AddModelInfo( const ModelInfo& Info )
//	{
//		if ( !m_pGraphic || Info.sArpFile.empty() || m_pRenderObj && m_pRenderObj->GetType() != ROT_MODEL )
//			return;
//
//		CModel* pModel = m_pRenderObj ? (CModel*)m_pRenderObj : NULL;
//		if ( !pModel )
//		{
//			pModel = new CModel;
//			pModel->AddRef();
//		}
//
//		bool bEnableASynRead = CPkgFile::IsEnableAsyncRead();
//		CPkgFile::EnableAsyncRead( false );
//		CPieceGroup* pPieceGroup = NULL;
//		if ( SUCCEEDED( m_pGraphic->CreatePieceGroup( Info.sArpFile.c_str(), &pPieceGroup ) ) )
//		{
//			if ( !Info.sClass.empty() )
//				pModel->AddMeshPiece( pPieceGroup, Info.sClass.c_str(), Info.uRS );
//			else
//				for ( size_t i = 0; i < pPieceGroup->GetPieceClassNum(); i++ )
//					pModel->AddMeshPiece( pPieceGroup, pPieceGroup->GetPieceClassName(i), Info.uRS );
//			m_vecModelInfo.push_back( Info );
//		}
//		CPkgFile::EnableAsyncRead( bEnableASynRead );
//		m_pRenderObj = pModel;
//		SafeRelease( pPieceGroup );
//	}
//
//	void SetAniInfo( UIString sAraFile )
//	{
//		if ( !m_pGraphic || sAraFile.empty() || !m_pRenderObj || m_pRenderObj->GetType() != ROT_MODEL )
//			return;
//
//		if ( SUCCEEDED( CSkeletalsMgr::GetInst()->GetSkeInPool(sAraFile.c_str(),&m_pSkeletalFrame) ))
//			m_sAraFile = sAraFile;
//
//		UIString sAgpFile = sAraFile.substr( 0, sAraFile.size() - 4 ) + ".agp";	
//		
//		if ( SUCCEEDED( m_pGraphic->CreateAnimationGroup( sAgpFile.c_str(), &m_pAnimateGroup ) ) )
//			m_sAgpFile = sAgpFile;
//	}
//
//	void AddFxInfo( const FxInfo& Info )
//	{
//		if ( !m_pFxMgr || Info.sFxFile.empty() || Info.sFxName.empty() )
//			return;
//		IEffectGroup* pEffectGroup;
//
//		bool bEnableASynRead = CPkgFile::IsEnableAsyncRead();
//		CPkgFile::EnableAsyncRead( true );
//		if ( SUCCEEDED( m_pFxMgr->CreateEffectGroup( Info.sFxFile.c_str(), &pEffectGroup ) ) )
//		{
//			UIVector<UIString> sFxGroup;
//			if ( !Info.sFxName.empty() )
//				sFxGroup.push_back( Info.sFxName );
//			else
//				for ( uint32 i = 0; i < pEffectGroup->GetEffectNum(); i++ )
//					sFxGroup.push_back( pEffectGroup->GetEffectName(i) );
//
//			for ( int32 i = 0; i < (int32)sFxGroup.size(); i++ )
//			{
//				IEffect* pEffect;
//				if ( SUCCEEDED( m_pFxMgr->CreateEffect( pEffectGroup, sFxGroup[i].c_str(), &pEffect ) ) )
//				{
//					if ( m_pRenderObj )
//					{
//						m_pRenderObj->AddLinkObj( pEffect, ( Info.sFxFile + sFxGroup[i] ).c_str(), false );
//						SafeRelease( pEffect );
//					}
//					else
//						m_pRenderObj = pEffect;
//				}
//			}
//			m_vecFxInfo.push_back( Info );
//		}
//		CPkgFile::EnableAsyncRead( bEnableASynRead );
//		SafeRelease( pEffectGroup );
//	}
//
//	const WND_RENDER_OBJ_STRUCT& operator= ( const WND_RENDER_OBJ_STRUCT& WndObj )
//	{
//		Clear();
//		m_pGraphic	= WndObj.m_pGraphic;
//		m_pFxMgr	= WndObj.m_pFxMgr;
//		m_fScale	= WndObj.m_fScale;
//		m_vecOff	= WndObj.m_vecOff;
//		for ( size_t i = 0; i < WndObj.m_vecModelInfo.size(); i++ )
//			AddModelInfo( WndObj.m_vecModelInfo[i] );
//		SetAniInfo( WndObj.m_sAraFile );
//		for ( size_t i = 0; i < WndObj.m_vecFxInfo.size(); i++ )
//			AddFxInfo( WndObj.m_vecFxInfo[i] );
//		return *this;
//	}
//};
//
//WND_RENDER_OBJ::WND_RENDER_OBJ( const WND_RENDER_OBJ& ImageList )
//{
//	m_hRenderObj = new WND_RENDER_OBJ_STRUCT;
//	*this = ImageList;
//}
//
//WND_RENDER_OBJ::WND_RENDER_OBJ() : m_hRenderObj( NULL )
//{
//	m_hRenderObj = new WND_RENDER_OBJ_STRUCT;
//}
//
//WND_RENDER_OBJ::~WND_RENDER_OBJ()
//{
//	Release();
//	delete m_hRenderObj;
//}
//
//void WND_RENDER_OBJ::Release()
//{
//	((WND_RENDER_OBJ_STRUCT*)m_hRenderObj)->Clear();
//}
//
//bool WND_RENDER_OBJ::IsEmpty() const
//{
//	return !((WND_RENDER_OBJ_STRUCT*)m_hRenderObj)->m_pRenderObj;
//}
//
//const CVector2f& WND_RENDER_OBJ::GetDir()const
//{
//	return ((WND_RENDER_OBJ_STRUCT*)m_hRenderObj)->m_vecDir;
//}
//
//void WND_RENDER_OBJ::GetDir(CVector2f& vecDir)
//{
//	vecDir = GetDir();
//}
//
//void WND_RENDER_OBJ::SetDir( const CVector2f& vecDir )
//{
//	((WND_RENDER_OBJ_STRUCT*)m_hRenderObj)->m_vecDir = vecDir;
//	((WND_RENDER_OBJ_STRUCT*)m_hRenderObj)->m_vecDir.Normalize();
//}
//
//float WND_RENDER_OBJ::GetScale() const
//{
//	return ((WND_RENDER_OBJ_STRUCT*)m_hRenderObj)->m_fScale;
//}
//
//void WND_RENDER_OBJ::SetScale( float fScale )
//{
//	((WND_RENDER_OBJ_STRUCT*)m_hRenderObj)->m_fScale = fScale;
//}
//
//CVector3f WND_RENDER_OBJ::GetOff() const
//{
//	return ((WND_RENDER_OBJ_STRUCT*)m_hRenderObj)->m_vecOff;
//}
//
//void WND_RENDER_OBJ::GetOff( CVector3f& vecOff )
//{
//	vecOff = GetOff();
//}
//
//void WND_RENDER_OBJ::SetOff( const CVector3f& vecOff )
//{
//	((WND_RENDER_OBJ_STRUCT*)m_hRenderObj)->m_vecOff = vecOff;
//}
//
//void WND_RENDER_OBJ::SetNextAnimation( const char* AniName, const FramePair& FmPair, bool bLoop, int32 DelayTime )
//{
//	if ( !IsEmpty() )
//	{
//		WND_RENDER_OBJ_STRUCT& Obj = *((WND_RENDER_OBJ_STRUCT*)m_hRenderObj);
//
//		Obj.m_sAniName = AniName;
//		Obj.m_FramePair = FmPair;
//		Obj.m_bLoop = bLoop;
//		((CBaseModel*)Obj.m_pRenderObj)->SetNextAnimation( AniName, FmPair, bLoop, DelayTime );
//	}
//}
//
//void WND_RENDER_OBJ::AddRenderObj( SQRWnd* pAnyWnd, const char* sArpFile, const char* sClass, uint32 uRS,
//                                   const char* sAraFile, const char* sFxFile, const char* sFxName )
//{
//	WND_RENDER_OBJ_STRUCT& Obj = *((WND_RENDER_OBJ_STRUCT*)m_hRenderObj);
//	Obj.m_pGraphic	= pAnyWnd->GetGraphic();
//	Obj.m_pFxMgr	= pAnyWnd->GetEffectManager();
//
//	WND_RENDER_OBJ_STRUCT::ModelInfo MI = { sArpFile, sClass, uRS };
//	Obj.AddModelInfo( MI );
//
//	Obj.SetAniInfo( sAraFile );
//
//	WND_RENDER_OBJ_STRUCT::FxInfo FI = { sFxFile, sFxName };
//	Obj.AddFxInfo( FI );
//}
//
//const WND_RENDER_OBJ& WND_RENDER_OBJ::operator = ( const WND_RENDER_OBJ& RenderObj )
//{
//	if ( this == &RenderObj )
//		return *this;
//
//	*((WND_RENDER_OBJ_STRUCT*)m_hRenderObj) = *((WND_RENDER_OBJ_STRUCT*)RenderObj.m_hRenderObj);
//
//	return *this;
//}
//
//AniResult WND_RENDER_OBJ::Render( const CMatrix& matWorld, uint32 uCurTime ) const
//{
//	AniResult AniRet;
//
//	if ( !IsEmpty() && ((WND_RENDER_OBJ_STRUCT*)m_hRenderObj)->CheckAniValid() )
//	{
//		CMatrix matRotate;
//		CMatrix matScale;
//		matRotate._11 =  GetDir().y;
//		matRotate._13 = -GetDir().x;
//		matRotate._31 = -matRotate._13;
//		matRotate._33 =  matRotate._11;
//		matScale._11 = matScale._22 = matScale._33 = GetScale();
//
//		matScale = matScale*matRotate;
//		*(CVector3f*)&matScale._41 = GetOff();
//		AniRet = ((WND_RENDER_OBJ_STRUCT*)m_hRenderObj)->m_pRenderObj->Render( matScale*matWorld, uCurTime );
//	}
//
//	return AniRet;
//}

GUI_EVENT_FONT* GUI_FONT::GetEventFont(EventStateMask esm)
{
	if (esm >=0 && esm < IP_COUNT)
	{
		EvtFontVector::iterator itr = m_EventFont.find(esm);
		if (itr == m_EventFont.end())
			return NULL;
		return &(itr->second);
	}
	return NULL;
}

StyleStruct::StyleStruct()
		: pTexture(NULL)
		, m_dwColor(0xFFFFFFFF)
{
}

void StyleStruct::Release(void)
{
	SafeRelease(pTexture);
	m_vecPool.clear();

	StyleStruct::FontVector::iterator vit,evit = m_vecFont.end();
	for (vit = m_vecFont.begin(); vit!= evit; ++vit)
		GfkSafeDelete(vit->second);
	m_vecFont.clear();
}

BLOCK_POOL::BLOCK_POOL()
{
	m_vecRect.resize(MAX_BLOCK_NUM);
}

BLOCK_POOL::~BLOCK_POOL()
{
	for ( int i = 0; i<MAX_BLOCK_NUM; ++i )
		GfkSafeDelete(m_vecRect[i]);
}

GUI_BLOCK& BLOCK_POOL::InitBlock(UINT i)
{
	--i;
	if (NULL == m_vecRect[i])
		m_vecRect[i] = new GUI_BLOCK;
	return *m_vecRect[i];
}

IHook::IHook()
{
}
IHook::~IHook()
{
	SQRGUIManager::UnRegisterHook(this);
}

IAccelerator::IAccelerator()
{
}
IAccelerator::~IAccelerator()
{
	SQRGUIManager::UnRegisterAccelerator(this);
}


bool IHook::OnHook( SQRWnd* pWnd, uint32 message, WPARAM wParam, LPARAM lParam )
{
	CALL_CLASS_CALLBACK_4_RET(bool, pWnd, message, wParam, lParam)
		return false;
};

#include "stdafx.h"
#include "CSmallMap.h"
#include "CSyncVariantClient.h"
#include "SQRRootWnd.h"
#include "CCharacterDirector.h"
#include "CAppClient.h"
#include "CSceneManagerClient.h"
#include "CRelationMgrClient.h"
#include "CRenderSystemClient.h"
#include "CSmallMapCoder.h"

DYNAMIC_CREATE_WND( CSmallMap, SQRWnd )
const double CSmallMap::MapRotateRadian = MATH_PI_DIV_4;
const uint32 CSmallMap::st_uRegionScale = 4096;
const uint32 CSmallMap::st_uCenterId	= 1;
const uint32 CSmallMap::ms_uImageSize = 12;
UIVector<MapImageData>CSmallMap::ms_vecImageType;


MAPROLE::MAPROLE()
{
}

MAPROLE::~MAPROLE()
{
	for(map<CCoreObjectFollower*,WND_IMAGE_LIST*>::iterator it = m_aRoleInfo.begin(); it != m_aRoleInfo.end(); ++it )
	{
		AdvDelRef(it->first);
		SafeDelete (it->second);
	}
}

bool MAPROLE::DelItem( CCoreObjectFollower* Obj )
{
	map<CCoreObjectFollower*,WND_IMAGE_LIST*>::iterator it = m_aRoleInfo.find(Obj);
	if(it != m_aRoleInfo.end())
	{
		AdvDelRef(it->first);
		SafeDelete (it->second);
		m_aRoleInfo.erase(it);
		return true;
	}		
	return false;
}
void MapImageData::InitMapImageData(const string& imageStrInfo, int32 imageHeight, int32 imageWidth)
{
	this->imageStrInfo = imageStrInfo.c_str();
	this->imageHeight  = imageHeight;
	this->imageWidth   = imageWidth;
}

CSmallMapTex::CSmallMapTex(CElementManager* pParent , URegionID id) 
: CElementNode(pParent,id),m_MapText(NULL) 
{
	m_uX = id.X;
	m_uY = id.Z;
}

CSmallMapTex::~CSmallMapTex()
{
	SafeRelease(m_MapText);
}

CSmallMap::CSmallMap()
: m_rcPiece()
, m_fScale(0.6f)
, m_fCWRotRad( (float)3.1415926f / 4.0f )
, m_nWidthInPixel(0)
, m_nHeightInPixel(0)
, m_pWnd(NULL)
, m_MainPlayerDirection(0,0)
, m_MainPlayerPosition(0,0)
, m_pBackTexture(NULL)
, m_pMainPlayerWnd(NULL)
,m_pCenterBtn(NULL)
{
	//memset(m_MapTexture,NULL,sizeof(m_MapTexture));
	ms_vecImageType.resize(ms_uImageSize);
	InitNpcAndPlayerImageData();
	CAppClient::Inst()->RegisterTick(this, 100);
	m_pCoderEx = new CSmallMapCoder;
	SetCoder( m_pCoderEx );
}

CSmallMap::~CSmallMap(void)
{
	Destroy();
	ClearUp();
	SafeRelease(m_pBackTexture);
	CAppClient::Inst()->UnRegisterTick(this);
	//CClientRenderScene::GetInst()->GetDataScene()->DelElementManager(this);
}

void CSmallMap::ClearUp()
{
	//for(uint i = 0;i<3;++i)
	//	for(uint j=0;j<3;++j)
	//		SafeRelease(m_MapTexture[i][j]);
	ClearAllNpcAndPlayerInfo();
}

void CSmallMap::SetScale(float fScale)
{ 
	if(fabs(m_fScale - fScale) < 0.0001)
		return;

	m_fScale = fScale;
	m_fBlockWidth = float(m_pBackTexture->GetWidth()) * m_fScale;
	m_fBlockHeight = float(m_pBackTexture->GetHeight()) * m_fScale;
}

int32 CSmallMap::DestroyWnd()
{
	ClearUp();
	AdvDelRef(m_pWnd);
	return SQRWnd::DestroyWnd();
}

void CSmallMap::InitNpcAndPlayerImageData()
{
	MapImageData mapImageEnemyNpc;
	mapImageEnemyNpc.InitMapImageData("guitex/tex/ybtx/gui/2ndEdition/Icon/4.tex", 20, 20);
	MapImageData mapImageFriendNpc;
	mapImageFriendNpc.InitMapImageData("guitex/tex/ybtx/gui/2ndEdition/Icon/3.tex", 20, 20);
	MapImageData mapImageTeamMate;
	mapImageTeamMate.InitMapImageData("guitex/tex/ybtx/gui/2ndEdition/Icon/0.tex",20, 20);
	MapImageData mapImageEnemyPlayer;
	mapImageEnemyPlayer.InitMapImageData("guitex/tex/ybtx/gui/2ndEdition/Icon/2.tex",20, 20);
	MapImageData mapImageFriendPlayer;
	mapImageFriendPlayer.InitMapImageData("guitex/tex/ybtx/gui/2ndEdition/Icon/1.tex",20, 20);
	MapImageData mapImageTakeQuestNpc;
	mapImageTakeQuestNpc.InitMapImageData("guitex/tex/ybtx/gui/2ndEdition/Icon/5.tex",20, 20);
	MapImageData mapImageFinishQuestNpc;
	mapImageFinishQuestNpc.InitMapImageData("guitex/tex/ybtx/gui/2ndEdition/Icon/6.tex",20, 20);
	MapImageData mapImageNpcShopNpc;
	mapImageNpcShopNpc.InitMapImageData("guitex/tex/ybtx/gui/2ndEdition/Icon/7.tex",20, 20);
	MapImageData mapImageCSMNpc;
	mapImageCSMNpc.InitMapImageData("guitex/tex/ybtx/gui/2ndEdition/Icon/8.tex",20, 20);//寄售交易所
	MapImageData mapImageCOFCNpc;
	mapImageCOFCNpc.InitMapImageData("guitex/tex/ybtx/gui/2ndEdition/Icon/9.tex",20, 20);//商会
	MapImageData mapImageTongNpc;
	mapImageTongNpc.InitMapImageData("guitex/tex/ybtx/gui/2ndEdition/Icon/10.tex",20, 20);//帮会
	MapImageData mapImageWanFaNpc;
	mapImageWanFaNpc.InitMapImageData("guitex/tex/ybtx/gui/2ndEdition/Icon/11.tex",20, 20);//玩法
	ms_vecImageType[0] = mapImageEnemyNpc;
	ms_vecImageType[1] = mapImageFriendNpc;
	ms_vecImageType[2] = mapImageTeamMate;
	ms_vecImageType[3] = mapImageEnemyPlayer;
	ms_vecImageType[4] = mapImageFriendPlayer;
	ms_vecImageType[5] = mapImageTakeQuestNpc;
	ms_vecImageType[6] = mapImageFinishQuestNpc;
	ms_vecImageType[7] = mapImageNpcShopNpc;
	ms_vecImageType[8] = mapImageCSMNpc;
	ms_vecImageType[9] = mapImageCOFCNpc;
	ms_vecImageType[10] = mapImageTongNpc;
	ms_vecImageType[11] = mapImageWanFaNpc;
}

void CSmallMap::InitData( uint8 uHorBlocks, uint8 uVerBolcks, uint8 uPiecesPerBlock, uint8 uPiecesSize, bool bBigMap )
{
	uPiecesSize = uint8( (uPiecesSize * m_fScale/4)*4 );
	m_rcPiece.SetValue(0, 0, 0, 0);

	const int32 nBlockSize	= uPiecesSize * uPiecesPerBlock;
}

void CSmallMap::DrawSmallMap()
{
	CVector2f center( m_fCenterX + m_fBlockWidth/2 - float(m_MainPlayerPosition.x - m_XRegion*st_uRegionScale)*m_fBlockWidth /st_uRegionScale 
					, m_fCenterY - m_fBlockHeight/2 + float(m_MainPlayerPosition.y - m_YRegion*st_uRegionScale)*m_fBlockHeight /st_uRegionScale );
	IGraphic* pGraphic = GetGraphic();

	VerText2D* pVB;
	float MinUV = 0.5f / m_pBackTexture->GetWidth();
	float MaxUV = 1.0f - MinUV;



	ElementPool::iterator it = m_ElePool.begin();
	ElementPool::iterator ite = m_ElePool.end();
	uint w,h;
	CSmallMapTex* pNode;
	for (;it!=ite;++it)
	{
		pNode = (CSmallMapTex*)it->second;
		if ( pNode->IsValid() )
		{

			w = pNode->GetXRegion() - m_XRegion + 1;
			h = pNode->GetYRegion() - m_YRegion + 1;

			if(	w < 0 || h < 0 ||
				center.x + m_fBlockWidth	* ( w - 0.5f )	< 0 || 
				center.x + m_fBlockWidth	* ( w - 1.5f )	> m_pBackTexture->GetWidth() ||
				center.y + m_fBlockHeight	* ( 1.5f - h )	< 0 ||
				center.y + m_fBlockHeight	* ( 0.5f - h )	> m_pBackTexture->GetHeight() )
				continue;

			RenderParam* pRP;
			pGraphic->GetRenderStack( RS_GUI , pNode->GetTexture() , NULL , PT_TRIANGLESTRIP 
				, 4 , 2 , VerText2D::Format , sizeof(VerText2D) , (void**)&pVB , NULL, (void**)&pRP );

			pVB[0] = VerText2D( center.x + m_fBlockWidth * ( w - 1.5f ) , center.y + m_fBlockHeight * ( 0.5f - h )	, 1.0f, MinUV,MinUV );
			pVB[1] = VerText2D( center.x + m_fBlockWidth * ( w - 0.5f ) , center.y + m_fBlockHeight * ( 0.5f - h )	, 1.0f, MaxUV,MinUV );
			pVB[2] = VerText2D( center.x + m_fBlockWidth * ( w - 1.5f ) , center.y + m_fBlockHeight * ( 1.5f - h )	, 1.0f, MinUV,MaxUV );
			pVB[3] = VerText2D( center.x + m_fBlockWidth * ( w - 0.5f ) , center.y + m_fBlockHeight * ( 1.5f - h )	, 1.0f, MaxUV,MaxUV );		
			pRP->m_RS.m_ZTestEnable = false;
		}
	}
	DrawMainPlayer();
}

/// @brief 绘制主角
/// TODO: 该方法需要整理，计算流程还不是很清楚
void CSmallMap::DrawMainPlayer()
{
	int codestate = 0;
	SQR_TRY	
	{
		if(/*m_bDrawPlayerInBigMap == false || */m_aRoleInfo[eMR_Player].Size() == 0)
			return;

		IMAGE& img = m_aRoleInfo[eMR_Player][NULL].GetImage(0);
		IGraphic* pGraphic = GetGraphic();
		codestate = 1;
		if ( img.pTexture == NULL)
		{
			m_aRoleInfo[eMR_Player][NULL].CreatTex( pGraphic, NULL );
			img = m_aRoleInfo[eMR_Player][NULL].GetImage(0);
		}
		codestate = 2;

		const CFRect rtItem((float)img.posWnd.x, (float)img.posWnd.y, img.posWnd.x + img.rtTexture.Width(), img.posWnd.y + img.rtTexture.Height());
		const CVector2f posCenter(m_fCenterX, m_fCenterY);

		CVector2f xDir, yDir;
		xDir.x = ( m_MainPlayerDirection.x - m_MainPlayerDirection.y )*   0.707f;
		xDir.y = ( m_MainPlayerDirection.y + m_MainPlayerDirection.x )*( -0.707f );
		yDir.x = ( m_MainPlayerDirection.x + m_MainPlayerDirection.y )*   0.707f;
		yDir.y = ( m_MainPlayerDirection.y - m_MainPlayerDirection.x )*( -0.707f );
		xDir.Normalize();
		yDir.Normalize();
		xDir = xDir.Rotate(MapRotateRadian, true);
		yDir = yDir.Rotate(MapRotateRadian, true);
		xDir *= rtItem.Width() * 0.5f;
		yDir *= rtItem.Height() * 0.5f;

		// 目标绘制区域
		CVector2f vPos[4] = {
			posCenter + xDir - yDir,
			posCenter + xDir + yDir,
			posCenter - xDir - yDir,
			posCenter - xDir + yDir
		};
		codestate = 3;
		CFRect rtTexture = img.rtTexture;

		// 	// 源贴图区域
		// 	CVector2f vTexture[4] = {
		// 		CVector2f(, rtTexture.bottom),
		// 		CVector2f(,),
		// 		CVector2f(rtTexture.left, ),
		// 		CVector2f(rtTexture.left, rtTexture.top)
		// 	};
		codestate = 4;
		VerText2D* pVB;
		RenderParam* pRP;
		GetGraphic()->GetRenderStack( RS_GUI , img.pTexture
			, NULL , PT_TRIANGLESTRIP , 4 , 2 , VerText2D::Format , sizeof(VerText2D) , (void**)&pVB , NULL, (void**)&pRP );
		codestate = 5;
		pVB[0] = VerText2D( vPos[0].x, vPos[0].y, 1.0f, rtTexture.right / img.pTexture->GetWidth() , rtTexture.bottom	/ img.pTexture->GetHeight() );
		pVB[1] = VerText2D( vPos[1].x, vPos[1].y, 1.0f, rtTexture.right / img.pTexture->GetWidth() , rtTexture.top		/ img.pTexture->GetHeight() );
		pVB[2] = VerText2D( vPos[2].x, vPos[2].y, 1.0f, rtTexture.left  / img.pTexture->GetWidth() , rtTexture.bottom	/ img.pTexture->GetHeight() );
		pVB[3] = VerText2D( vPos[3].x, vPos[3].y, 1.0f, rtTexture.left  / img.pTexture->GetWidth() , rtTexture.top		/ img.pTexture->GetHeight() );
		pRP->m_RS.m_ZTestEnable = false;
		codestate = 6;
	}
	SQR_CATCH(exp)
	{
		stringstream errmsg;
		errmsg	<< "     error drop line: " << codestate  << "\n";
		exp.AppendMsg(errmsg.str());
		GfkLogExpOnce(exp);
	}
	SQR_TRY_END;
}

void CSmallMap::DrawWndBackground()
{
	if(m_pBackTexture==NULL)
		return;

	m_pBackTexture->Begin(true);
	{
		GetGraphic()->SwapBegin();
		DrawSmallMap();
		GetGraphic()->SwapEnd();
	}
	m_pBackTexture->End();

	CColor uColor = CColor::White;
	CVector2f vStart( (float)GetWndHandle()->m_xScreen, (float)GetWndHandle()->m_yScreen );
	CVector2f rtScreen[4] = { vStart
		, vStart + CVector2f((float)GetWndHandle()->m_Width,0.0f)
		, vStart + CVector2f(0.0f,(float)GetWndHandle()->m_Height)
		, vStart + CVector2f((float)GetWndHandle()->m_Width,(float)GetWndHandle()->m_Height)};
	
	float Zoom = GetRootWnd()->GetZoomSize();
	IMAGE& image =  GetWndHandle()->m_Enable.GetImage(0);
	CFRect rtImage((float)image.posWnd.x, (float)image.posWnd.y, image.posWnd.x + abs(image.rtTexture.Width()), image.posWnd.y + abs(image.rtTexture.Height())); 
	rtImage.top		*= Zoom;
	rtImage.left	*= Zoom;
	rtImage.right	*= Zoom;
	rtImage.bottom	*= Zoom;
	//CFRect rtDraw = m_wndRect.Intersection(rtImage).Offset(0.0f,0.0f);
	CFRect rtText(
		 image.rtTexture.left,
		 image.rtTexture.top,
		( rtImage.right  - rtImage.left )*image.rtTexture.Width() /rtImage.Width()  + image.rtTexture.left,
		( rtImage.bottom -  rtImage.top )*image.rtTexture.Height()/rtImage.Height() + image.rtTexture.top);

	uColor.A = uint8( uColor.A*GetWndHandle()->m_fTransparent );
	DrawRect( rtScreen, uColor,image.pTexture,m_pBackTexture,&rtText,m_BackTexUV, RS_GUI_ALPHA, GetWndHandle()->m_fZ );	
	
}

void CSmallMap::SetPlayerTex()
{
	m_aRoleInfo[eMR_Player].PushItem(NULL);
	SetRoleTex( &(m_aRoleInfo[eMR_Player][NULL]), eMR_Player );
}

void CSmallMap::SetRoleTex( WND_IMAGE_LIST* pItem, EMapRole eRole )
{
	string ResName[eMR_Count+1] = 
	{
		"ChkBtnGrain01",
		"ChkBtnGrain02",
		"ChkBtnGrain03",
		"ChkBtnGrain04",
		"ChkBtnGrain05",
		"WndPlayer"
	};

	IMAGE_PARAM IP ( SM_BS_BK, IP_ENABLE );
	*pItem = *(m_pWnd->GetDlgChild(ResName[eRole].c_str())->GetWndBkImage(IP));

	IGraphic* pGraphic = GetGraphic();
	pItem->CreatTex( pGraphic, NULL );

	IMAGE& img = pItem->GetImage(0);
	img.posWnd = CFPos(0,0);
}

void CSmallMap::ReceiveRoleData( CCoreObjectFollower* Obj, uint8 nType )
{
	// 得到纹理
	if( m_aRoleInfo[nType].PushItem(Obj) )
		SetRoleTex( &(m_aRoleInfo[nType][Obj]), (EMapRole)nType );
	AdvAddRef(Obj);
}

bool CSmallMap::RemoveRoleData( CCoreObjectFollower* Obj )
{
	for( int i = 0; i < eMR_Player; i++ )
	{
		if(m_aRoleInfo[i].DelItem(Obj))
			return true;
	}
	return false;
}

void CSmallMap::SetPlayerPos( const CVector3f& pos )
{
	//重新计算玩家在窗口的位置
	IMAGE& img = m_aRoleInfo[eMR_Player][NULL].GetImage(0);
	img.posWnd.SetValue( (GetWndWidth()/2 - img.rtTexture.Width()/2),
		(GetWndHeight()/2 - img.rtTexture.Height()/2) );
}
void CSmallMap::SetMainPlayerPixelPos(float x, float y)
{
	if( m_MainPlayerPosition.x == x && m_MainPlayerPosition.y == y )
		return;
	m_MainPlayerPosition.x = x;
	m_MainPlayerPosition.y = y;

	m_XRegion = limit2( uint32(x / st_uRegionScale) , uint32(0), m_MaxWidth-1 );
	m_YRegion = limit2( uint32(y / st_uRegionScale) , uint32(0), m_MaxHight-1 );
	return;
	//DWORD	flag = CGridDirectionTool::GET_DIRECTION(XRegion-m_XRegion,YRegion-m_YRegion);
	//if( m_XRegion == 0xffffffff && m_YRegion == 0xffffffff )
	//	flag = 0;

	//GetGraphic()->EnableAsynRead(true);
	//string FileName = "";
	//uint32 nRgnIndex = 0;

	//switch(flag)
	//{
	//case eGT_NXNZ:
	//	{
	//		SafeRelease(m_MapTexture[0][2]);
	//		SafeRelease(m_MapTexture[1][2]);
	//		SafeRelease(m_MapTexture[2][2]);
	//		SafeRelease(m_MapTexture[2][1]);
	//		SafeRelease(m_MapTexture[2][0]);
	//		m_MapTexture[2][1] = m_MapTexture[1][0];
	//		m_MapTexture[1][2] = m_MapTexture[0][1];
	//		m_MapTexture[2][2] = m_MapTexture[1][1];
	//		m_MapTexture[1][1] = m_MapTexture[0][0];

	//		format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion,YRegion);
	//		GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[0][0]) );
	//		
	//		nRgnIndex = YRegion + 1;
	//		if( nRgnIndex < m_MaxHight)
	//		{
	//			format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion,nRgnIndex);
	//			GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[0][1]) );
	//		}
	//		
	//		nRgnIndex = XRegion + 1;
	//		if( nRgnIndex  <m_MaxWidth)
	//		{
	//			format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),nRgnIndex,YRegion);
	//			GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[1][0]) );
	//		}
	//		
	//		m_MapTexture[0][2] = NULL;
	//		nRgnIndex = YRegion + 2;
	//		if( nRgnIndex < m_MaxHight )
	//		{
	//			format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion,nRgnIndex);
	//			GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[0][2]) );
	//		}

	//		m_MapTexture[2][0] = NULL;
	//		nRgnIndex = XRegion + 2;
	//		if( nRgnIndex < m_MaxWidth )
	//		{
	//			format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),nRgnIndex,YRegion);
	//			GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[2][0]) );
	//		}
	//	}
	//	break;
	//case eGT_NX:
	//	for(int h = 0; h<3; ++h)
	//	{
	//		SafeRelease(m_MapTexture[2][h]);
	//		m_MapTexture[2][h] = m_MapTexture[1][h];
	//		m_MapTexture[1][h] = m_MapTexture[0][h];
	//		m_MapTexture[0][h] = NULL;
	//		if(YRegion + h>0 && YRegion + h - 1<m_MaxHight &&XRegion>0)
	//		{
	//			format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion - 1,YRegion + h - 1);
	//			GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[0][h]) );
	//		}
	//	}
	//	break;
	//case eGT_NXZ:
	//	{
	//		SafeRelease(m_MapTexture[2][2]);
	//		SafeRelease(m_MapTexture[2][1]);
	//		SafeRelease(m_MapTexture[2][0]);
	//		SafeRelease(m_MapTexture[0][1]);
	//		SafeRelease(m_MapTexture[0][0]);
	//		m_MapTexture[2][0] = m_MapTexture[1][1];
	//		m_MapTexture[2][1] = m_MapTexture[1][2];
	//		m_MapTexture[1][0] = m_MapTexture[0][1];
	//		m_MapTexture[1][1] = m_MapTexture[0][2];

	//		format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion,YRegion);
	//		GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[0][2]) );
	//		format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion+1,YRegion);
	//		GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[1][2]) );		
	//		
	//		m_MapTexture[0][1] = NULL;
	//		m_MapTexture[0][0] = NULL;
	//		m_MapTexture[2][2] = NULL;
	//		
	//		if(YRegion>0)
	//		{				
	//			format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion,YRegion-1);
	//			GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[0][1]) );
	//			
	//			if(YRegion > 1)
	//			{
	//				format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion,YRegion - 2);
	//				GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[0][0]) );
	//			}
	//		}

	//		if(XRegion + 2<m_MaxWidth)
	//		{
	//			format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion + 2,YRegion);
	//			GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[2][2]) );
	//		}
	//	}
	//	break;
	//case eGT_NZ:

	//	for(int w = 0; w<3; ++w)
	//	{
	//		SafeRelease(m_MapTexture[w][2]);
	//		m_MapTexture[w][2] = m_MapTexture[w][1];
	//		m_MapTexture[w][1] = m_MapTexture[w][0];
	//		m_MapTexture[w][0] = NULL;
	//		if(XRegion + w>0 && XRegion + w - 1<m_MaxWidth && YRegion > 0)
	//		{
	//			format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion + w - 1 ,YRegion - 1);
	//			GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[w][0]) );
	//		}
	//	} 
	//	break;
	//case eGT_O:
	//	break;
	//case eGT_Z:
	//	for(int w = 0; w<3; ++w)
	//	{
	//		SafeRelease(m_MapTexture[w][0]);
	//		m_MapTexture[w][0] = m_MapTexture[w][1];
	//		m_MapTexture[w][1] = m_MapTexture[w][2];
	//		m_MapTexture[w][2] = NULL;
	//		if(XRegion + w >0 && XRegion + w - 1<m_MaxWidth && YRegion + 1 < m_MaxHight)
	//		{
	//			format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion + w - 1 ,YRegion+1);
	//			GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[w][2]) );
	//		}
	//	} 
	//	break;

	//case eGT_XNZ:
	//	{
	//		SafeRelease(m_MapTexture[0][0]);
	//		SafeRelease(m_MapTexture[0][1]);
	//		SafeRelease(m_MapTexture[0][2]);
	//		SafeRelease(m_MapTexture[1][2]);
	//		SafeRelease(m_MapTexture[2][2]);
	//		m_MapTexture[0][1] = m_MapTexture[1][0];
	//		m_MapTexture[0][2] = m_MapTexture[1][1];
	//		m_MapTexture[1][2] = m_MapTexture[2][1];
	//		m_MapTexture[1][1] = m_MapTexture[2][0];			

	//		format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion,YRegion);
	//		GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[2][0]) );

	//		nRgnIndex = YRegion + 1;
	//		if( nRgnIndex < m_MaxHight )
	//		{
	//			format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion,nRgnIndex);
	//			GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[2][1]) );
	//		}

	//		m_MapTexture[1][0] = NULL;
	//		m_MapTexture[2][2] = NULL;
	//		m_MapTexture[0][0] = NULL;

	//		if(XRegion>0)
	//		{
	//			format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion - 1,YRegion);
	//			GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[1][0]) );
	//			if(XRegion > 1 )
	//			{
	//				format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion - 2,YRegion);
	//				GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[0][0]) );
	//			}
	//		}

	//		nRgnIndex = YRegion + 2;
	//		if( nRgnIndex < m_MaxHight )
	//		{
	//			format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion,nRgnIndex);
	//			GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[2][2]) );
	//		}
	//	}
	//	break;
	//case eGT_X:
	//	for(int h = 0; h<3; ++h)
	//	{
	//		SafeRelease(m_MapTexture[0][h]);
	//		m_MapTexture[0][h] = m_MapTexture[1][h];
	//		m_MapTexture[1][h] = m_MapTexture[2][h];
	//		m_MapTexture[2][h] = NULL;
	//		if(YRegion + h>0 && YRegion + h - 1<m_MaxHight && XRegion + 1 < m_MaxWidth)
	//		{			
	//			format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion + 1,YRegion + h - 1);
	//			GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[2][h]) );
	//			m_Test[2][h] = CVector2I(XRegion + 1,YRegion+h-1);
	//		}
	//	}
	//	break;
	//case eGT_XZ:
	//	{
	//		SafeRelease(m_MapTexture[0][2]);
	//		SafeRelease(m_MapTexture[0][1]);
	//		SafeRelease(m_MapTexture[0][0]);
	//		SafeRelease(m_MapTexture[1][0]);
	//		SafeRelease(m_MapTexture[2][0]);
	//		m_MapTexture[0][0] = m_MapTexture[1][1];
	//		m_MapTexture[0][1] = m_MapTexture[1][2];
	//		m_MapTexture[1][0] = m_MapTexture[2][1];
	//		m_MapTexture[1][1] = m_MapTexture[2][2];

	//		format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion,YRegion);
	//		GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[2][2]) );
	//		
	//		m_MapTexture[0][2] = NULL;
	//		m_MapTexture[1][2] = NULL;
	//		m_MapTexture[2][1] = NULL;
	//		m_MapTexture[2][0] = NULL;

	//		if(YRegion > 0)
	//		{
	//			format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion,YRegion-1);
	//			GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[2][1]) );
	//		
	//			if(YRegion > 1)
	//			{
	//				format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion,YRegion - 2);
	//				GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[0][2]) );
	//			}
	//		}	

	//		if( XRegion > 0 )
	//		{
	//			format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion-1,YRegion);
	//			GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[1][2]) );
	//			if(XRegion > 1)
	//			{
	//				format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion - 2,YRegion);
	//				GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[2][0]) );
	//			}
	//		}


	//	}
	//	break;
	//default:
	//	GetGraphic()->EnableAsynRead(false);
	//	ClearUp();
	//	for(int w = -1; w < 2; ++w)
	//		for(int h = -1; h < 2; ++h)
	//		{
	//			if(XRegion+w<0 || YRegion+h<0 || XRegion+w>=m_MaxWidth || YRegion+h>=m_MaxHight)
	//				continue;

	//			format(FileName,"%s/MinMap/M_%u_%u.dds",m_SceneName.c_str(),XRegion+w,YRegion+h);
	//			GetGraphic()->CreateTexture( FileName.c_str(), &(m_MapTexture[w+1][h+1]) );
	//			m_Test[w+1][h+1] = CVector2I(XRegion+w,YRegion+h);
	//		}

	//	break;
	//}

}

CElementNode* CSmallMap::_CreateNode( URegionID ID )
{
	return new CSmallMapTex( this,ID );
}

void CSmallMap::SetSceneData( uint32 nWidthInPixel, uint32 nHeightInPixel, float fCWRotDeg, const TCHAR* szMapFile, uint8 uSceneID )
{
	m_nWidthInPixel  = nWidthInPixel;		//按像素算的，每个格子64个像素
	m_nHeightInPixel = nHeightInPixel;	//按像素算的，每个格子64个像素
	m_fCWRotRad     = float(fCWRotDeg * MATH_PI / 180.0f);
	m_MaxWidth  = nWidthInPixel/st_uRegionScale;
	m_MaxHight = nHeightInPixel/st_uRegionScale;
	InitMapData( szMapFile,uSceneID );
	m_pCoderEx->Clear();
	CClientRenderScene::GetInst()->GetDataScene()->AddElementManager(this);
}

void CSmallMap::InitMapData( const TCHAR* szMapFile, uint8 uSceneID )
{
	SQR_TRY
	{
		ClearUp();
		m_SceneName = szMapFile;

		InitData( 4, 4, 2, 32 );
		m_wndRect = CFRect(0, 0, (float)GetWndWidth(), (float)GetWndHeight());

		m_dest[0] = CVector2f(m_wndRect.left, m_wndRect.top);
		m_dest[1] =	CVector2f(m_wndRect.right, m_wndRect.top);
		m_dest[2] = CVector2f(m_wndRect.left, m_wndRect.bottom);
		m_dest[3] = CVector2f(m_wndRect.right, m_wndRect.bottom);

		SafeRelease(m_pBackTexture);
		m_pBackTexture = GetGraphic()->CreateRenderTarget();
		m_pBackTexture->SetWidth(uint32(GetWndWidth()*1.5f));
		m_pBackTexture->SetHeight(uint32(GetWndHeight()*1.5f));
		m_pBackTexture->SetUseDepthBuffer(false);
		m_pBackTexture->Create();

		m_BackTexUV[2] = CVector2f(	0.0f , float(m_pBackTexture->GetHeight()/2));
		m_BackTexUV[0] = CVector2f( float(m_pBackTexture->GetWidth()/2) , 0.0f );
		m_BackTexUV[3] = CVector2f( float(m_pBackTexture->GetWidth()/2) , float(m_pBackTexture->GetHeight()) );
		m_BackTexUV[1] = CVector2f( float(m_pBackTexture->GetWidth())   , float(m_pBackTexture->GetHeight()/2));

		m_fDeltaX =  float(m_pBackTexture->GetWidth() )/3.0f;
		m_fDeltaY =  float(m_pBackTexture->GetHeight())/3.0f;
		m_fCenterX = float(m_pBackTexture->GetWidth() )/2.0f;
		m_fCenterY = float(m_pBackTexture->GetHeight())/2.0f;
		m_fBlockWidth = float(m_pBackTexture->GetWidth());
		m_fBlockHeight = float(m_pBackTexture->GetHeight());

		m_XRegion = 0xffffffff;
		m_YRegion = 0xffffffff;
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("CSmallMap::InitMapData");
		LogExp(exp);
	}
	SQR_TRY_END;
	
}

void CSmallMap::SetSmallMapWnd(SQRWnd * pWnd)
{
	AdvDelRef(m_pWnd);
	m_pWnd = pWnd;
	AdvAddRef(m_pWnd);
	m_pMainPlayerWnd = m_pWnd->GetDlgChild("WndPlayer");
	m_pCenterBtn =  m_pWnd->GetDlgChild("CenterBtn");
}



void CSmallMap::SetPlayerDir(float x, float y)
{
	m_MainPlayerDirection = CVector2f(x, y); 	
}

//清空所有保存的玩家和npc的信息
void CSmallMap::ClearAllNpcAndPlayerInfo()
{
	while(!m_mapFollowerMapBtn.empty())
	{

		delete m_mapFollowerMapBtn.begin()->second.first;
		m_mapFollowerMapBtn.erase(m_mapFollowerMapBtn.begin());		
	}
}

//根据followerID清空保存的玩家或npc信息
void CSmallMap::ClearNpcOrPlayerInfoByID(uint32 followerID)
{
	MapFollowerBtn::const_iterator iter = m_mapFollowerMapBtn.find(followerID);
	if (iter != m_mapFollowerMapBtn.end())
	{
		delete iter->second.first;
		m_mapFollowerMapBtn.erase(followerID);
	}
}

//得到小地图中显示主角的btn，在游戏屏幕中的位置（btn的中心x坐标和y坐标）
void CSmallMap::GetMainPlayerBtnPos(int32& xPos, int32& yPos)
{
	CFRect rect;
	m_pCenterBtn->GetWndRect( rect );
	CFRect rectRoot;

	xPos = (int32)( rect.left + ( rect.right - rect.left) / 2 );
	yPos = (int32)(rect.top + ( rect.bottom - rect.top ) / 2 );

}

//显示主角视野范围内的玩家和npc，参数：在场经常的xpos，ypos，npc或玩家的名称，贴图存放的位置
//不用种类的npc或玩家在小地图中显示的贴图不同
bool CSmallMap::IsNeedUpdatePlayerPos(SQRButton* pBtn ,const CFPos &Pos)
{
	float xPosRotate = ((Pos.x - Pos.y) - (m_MainPlayerPosition.x - m_MainPlayerPosition.y)) * 0.707f / 10 * m_fScale; //10为缩放比例
	float yPosRotate = ((Pos.x + Pos.y) - (m_MainPlayerPosition.x + m_MainPlayerPosition.y)) * 0.707f / 10 * m_fScale;
	int32 deltax = int32( xPosRotate );
	int32 deltay = int32( yPosRotate );
	double distance = sqrt(xPosRotate * xPosRotate + yPosRotate * yPosRotate);
	double staticWndWidth = (double)GetWndWidth()/2;
	if (distance < staticWndWidth)//在小地图圆形范围内的btn才显示，否则不显示'	
	{
		return true;
	}
	return false;
}

void CSmallMap::AddEntity(uint32 uGameEntityID,const char* szCharName, EMapRoleType eRoleType)
{
	CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(uGameEntityID);
	if (pCharacter&&pCharacter->GetGameEntityType()==eGET_CharacterFollower&&pCharacter->GetAITypeID()!=ENpcType_Dummy)
	{
		CFPos pos = pCharacter->GetPixelPos();
		SQRButton* pBtn = CreateBtn(szCharName);
		if (ms_uImageSize>eRoleType)
		{
			if (eRoleType!=eMRT_None)
			{
				MapImageData& ImageData= ms_vecImageType[eRoleType];
				SetBtnImage(pBtn,ImageData.imageStrInfo.c_str());
			}
		}
		m_mapFollowerMapBtn.insert(make_pair(uGameEntityID,make_pair(pBtn,eRoleType)));
	}
}

void CSmallMap::ChangeEntityImageType(uint32 uGameEntityID,EMapRoleType eRoleType)
{
	MapFollowerBtn::iterator it = m_mapFollowerMapBtn.find(uGameEntityID);
	if (it!=m_mapFollowerMapBtn.end())
	{
		it->second.second = eRoleType;
		if (ms_uImageSize>eRoleType)
		{
			if (eRoleType!=eMRT_None)
			{
				MapImageData& ImageData= ms_vecImageType[eRoleType];
				SetBtnImage(it->second.first,ImageData.imageStrInfo.c_str());
			}
		}
	}
}

void CSmallMap::SetMainPlayer(uint32 uMainPlayerID)
{
	m_uDirectorID=uMainPlayerID;
	CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(m_uDirectorID);
	CCharacterDirector* pPlayer=pCharacter?pCharacter->CastToCharacterDirector():NULL;
	if (pPlayer)
	{
		CFPos Pos;
		pPlayer->GetPixelPos(Pos);
		SetMainPlayerPixelPos(Pos.x,Pos.y);
	}
}

void CSmallMap::RemoveEntity(uint32 uObjID) 
{
	ClearNpcOrPlayerInfoByID(uObjID);
}


void CSmallMap::DrawFollower()
{
	CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(m_uDirectorID);
	CCharacterDirector* pPlayer=pCharacter?pCharacter->CastToCharacterDirector():NULL;
	if (pPlayer==NULL)
	{
		ClearAllNpcAndPlayerInfo();
		return;
	}
	

	uint32 uPlayerTeamID = pPlayer->GetViewVariant()->GetMember("Attribute")->GetMember("TeamID")->GetNumber<uint32>();
	MapFollowerBtn::iterator it = m_mapFollowerMapBtn.begin();
	for (; it!=m_mapFollowerMapBtn.end(); ++it)
	{
		CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(it->first);
		if (pCharacter&&pCharacter->GetGameEntityType()==eGET_CharacterFollower)
		{
			CFPos pos = pCharacter->GetPixelPos();
			EMapRoleType eRoleType =it->second.second;
			if (eRoleType<=eMRT_FriendPlayer)
			{
				EMapRoleType eUpdateType = (EMapRoleType)GetRoleType(pCharacter);
				if (eRoleType!=eUpdateType&& eUpdateType>eMRT_None)
				{
					it->second.second = eUpdateType;
					MapImageData& ImageData= ms_vecImageType[eUpdateType];
					SetBtnImage(it->second.first,ImageData.imageStrInfo.c_str());
				}
			}
			SQRButton* pBtn = it->second.first;
			if((m_setShowRoleType.count(eRoleType)==1)&&IsNeedUpdatePlayerPos(pBtn,pos)&&pCharacter->CppIsAlive())
			{
				MapImageData& ImageData= ms_vecImageType[it->second.second];
				SetBtnRect(pBtn,pos,ImageData.imageWidth,ImageData.imageHeight);
				pBtn->ShowWnd(true);
			}
			else
			{
				pBtn->ShowWnd(false);
			}
		}
	}
}

uint32 CSmallMap::GetRoleType(CCharacterFollower* pRole)
{
	CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(m_uDirectorID);
	CCharacterDirector* pPlayer=pCharacter?pCharacter->CastToCharacterDirector():NULL;
	if (pPlayer==NULL)
	{
		return eMRT_None;
	}
	uint32 uPlayerTeamID = pPlayer->GetViewVariant()->GetMember("Attribute")->GetMember("TeamID")->GetNumber<uint32>();
	uint32 uType = pRole->GetViewVariant()->GetMember("Attribute")->GetMember("Type")->GetNumber<uint8>();
	bool bIsFriend = CRelationMgrClient::IsFriend(pPlayer->GetFighter(), pRole->GetFighter());
	bool bIsEnemy = CRelationMgrClient::IsEnemy(pPlayer->GetFighter(), pRole->GetFighter());
	if (uType==1)
	{
		if (bIsFriend)
		{
			uint32 uTeamID = pRole->GetViewVariant()->GetMember("Attribute")->GetMember("TeamID")->GetNumber<uint32>();
			if (uPlayerTeamID!=0&&uPlayerTeamID==uTeamID)
			{
				return eMRT_TeamMate;
			} 
			else
			{
				return eMRT_FriendPlayer;
			}
		}
		if (bIsEnemy)
		{
			return eMRT_EnemyPlayer;
		}
	} 
	else if(uType==2)
	{
		if (bIsFriend)
		{
			return eMRT_FriendNpc;
		}
		if (bIsEnemy)
		{
			return eMRT_EnemyNpc;
		}
	}
	return eMRT_None;
}

void CSmallMap::AddShowRoleType(uint32 uRoleType)
{
	m_setShowRoleType.insert(uRoleType);
}

void CSmallMap::RemoveShowRoleType(uint32 uRoleType)
{
	m_setShowRoleType.erase(uRoleType);
}

void CSmallMap::OnTick()
{
	CVector3f m_vPos;
	CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(m_uDirectorID);
	CCharacterDirector* pPlayer=pCharacter?pCharacter->CastToCharacterDirector():NULL;
	if (pPlayer==NULL)
	{
		ClearAllNpcAndPlayerInfo();
		return;
	}
	CRenderObject * renderObj = pPlayer->GetRenderObject();
	if (renderObj == NULL)
		return;
	renderObj->GetRootPosition(&m_vPos);
	SetPlayerPos(m_vPos);

	CDir dir;
	pPlayer->GetDirection (dir);
	CVector2f m_vDir;
	dir.Get(m_vDir);

	SetPlayerDir(m_vDir.x, m_vDir.y);
	
	//主角在场景中的像素位置
	CFPos posInSceneInPixel;
	pPlayer->GetPixelPos(posInSceneInPixel);
	SetMainPlayerPixelPos(posInSceneInPixel.x, posInSceneInPixel.y);
	DrawFollower();
}

SQRButton* CSmallMap::CreateBtn(const char* szName)
{
	SQRButton *pBtn = new SQRButton;
	WndCreateParam param;
	param.uStyle = WS_CHILD | WS_IGNOREWHEEL;
	param.width = 12;
	param.height = 12;
	param.pParentWnd = m_pWnd;
	param.nFontID = 3;
	param.font_size = 12;

	SQR_TRY
	{
		pBtn->Create(param);	//在小地图中创建代表玩家和npc的btn
	}
	SQR_CATCH(exp) {
		ostringstream strm;
		strm<<"Current Root Manager:"<< GetRootWnd()->GetWndHandle()->m_pGUI << ";Current Parent Manager:" << m_pWnd->GetWndHandle()->m_pGUI;
		GenErr(exp.ErrorTitle(),	strm.str());
	}
	SQR_TRY_END;
	
	pBtn->ShowWnd(false);
	pBtn->EnableWnd(false);
	pBtn->SetMouseOverDesc(szName);	//设置tooltips
	return pBtn;
}

void CSmallMap::SetBtnImage(SQRButton* pBtn, const char* szImageInfo)
{
	WND_IMAGE_LIST DefaultImage;
	CFPos rectPos(0.0f, 0.0f);
	DefaultImage.AddImage( pBtn->GetGraphic(), -1, szImageInfo, NULL, rectPos, 0xffffffff, 0 );	
	pBtn->SetWndBkImage( IMAGE_PARAM(SM_BS_BK, IP_DISABLE), DefaultImage );
	pBtn->SetWndBkImage( IMAGE_PARAM(SM_BS_BK, IP_MOUSEOVER), DefaultImage );
}

void CSmallMap::GetOtherObjBtnPos(const CFPos playerPos, int32 &XPos, int32 &YPos)
{
	int32 playerCenterXpos = 0;
	int32 playerCenterYpos = 0;
	GetMainPlayerBtnPos(playerCenterXpos, playerCenterYpos);

	int delX = 1;
	int delY = -1;
	float difK = 2;
	if (playerPos.x != m_MainPlayerPosition.x)
		difK = abs((playerPos.y - m_MainPlayerPosition.y) /(playerPos.x - m_MainPlayerPosition.x));
	if (m_MainPlayerPosition.x <= playerPos.x && m_MainPlayerPosition.y <=  playerPos.y)
	{
		if (difK > 1)
		{
			delX = -1;
		}
	}
	else if (m_MainPlayerPosition.x >= playerPos.x && m_MainPlayerPosition.y <=  playerPos.y)
	{
		if (difK > 1)
		{
			delX = -1;
		}
		else if (difK <= 1)
		{
			delX = -1;
			delY = 1;
		}
	}
	else if (m_MainPlayerPosition.x >=  playerPos.x && m_MainPlayerPosition.y  >= playerPos.y)
	{
		if (difK < 1)
		{
			delX = -1;
			delY = 1;
		}
		else if (difK >=1)
		{
			delX = 1;
			delY = 1;
		}
	}
	else if (m_MainPlayerPosition.x <=  playerPos.x && m_MainPlayerPosition.y  >= playerPos.y)
	{
		if (difK > 1)
		{
			delX = 1;
			delY = 1;
		}
	}

	double mainPlayerLen = sqrt((m_MainPlayerPosition.x) *(m_MainPlayerPosition.x)+ (m_MainPlayerPosition.y)*(m_MainPlayerPosition.y));
	//double mainPlayerXPos = mainPlayerLen * sin(0.707 - atan(m_MainPlayerPosition.y / m_MainPlayerPosition.x));
	//double mainPlayerYPos = mainPlayerLen * cos(0.707 - atan(m_MainPlayerPosition.y / m_MainPlayerPosition.x));
	double mainPlayerXPos =0;
	double mainPlayerYPos = 0;
	double playerXPos = 0;
	double playerYPos = 0;
	if (mainPlayerLen == 0)
	{
		mainPlayerXPos = m_MainPlayerPosition.x;
		mainPlayerYPos = m_MainPlayerPosition.y;
	}
	else
	{
		mainPlayerXPos = mainPlayerLen * sin(0.707 - asin(m_MainPlayerPosition.y / mainPlayerLen));
		mainPlayerYPos = mainPlayerLen * cos(0.707 - asin(m_MainPlayerPosition.y  / mainPlayerLen));
	}

	double playerLen = sqrt((playerPos.x)*(playerPos.x) + (playerPos.y)*(playerPos.y));
	if (playerLen == 0)
	{
		playerXPos = playerPos.x;
		playerYPos = playerPos.y;
	}
	else
	{
		playerXPos = playerLen * sin(0.707 - asin(playerPos.y / playerLen));
		playerYPos = playerLen * cos(0.707 - asin(playerPos.y / playerLen));
	}

	double difDis = (m_MainPlayerPosition.y - playerPos.y) * (m_MainPlayerPosition.y - playerPos.y) + (m_MainPlayerPosition.x - playerPos.x) *(m_MainPlayerPosition.x - playerPos.x);
	double deltaDis = sqrt(difDis)* 0.707f / 8 * m_fScale;
	//double deltaX = deltaDis / sqrt((mainPlayerYPos - playerYPos)*(mainPlayerYPos - playerYPos)/ ((mainPlayerXPos - playerXPos)*(mainPlayerXPos - playerXPos)) + 1);
	//double deltaY = deltaDis / sqrt((mainPlayerXPos - playerXPos)*(mainPlayerXPos - playerXPos)/((mainPlayerYPos- playerYPos)*(mainPlayerYPos- playerYPos))+ 1);	
	double difDisRotate = sqrt((mainPlayerYPos - playerYPos)*(mainPlayerYPos - playerYPos) + (mainPlayerXPos - playerXPos)*(mainPlayerXPos - playerXPos));
	double deltaX = 0;
	double deltaY = 0;
	
	if (difDisRotate != 0)
	{
		deltaX = abs((deltaDis / difDisRotate) *(mainPlayerXPos - playerXPos));
		deltaY = abs((deltaDis / difDisRotate) *(mainPlayerYPos - playerYPos));
	}

	XPos = int32(playerCenterXpos + delX * deltaX);
	YPos = int32(playerCenterYpos + delY *deltaY);
}


void CSmallMap::SetBtnRect(SQRButton* pBtn, const CFPos Pos,int32 ImageWidth, int32 ImageHeight)
{
	int32 showXPos = 0;		//在小地图中显示npc或玩家btn的x、y坐标
	int32 showYPos = 0;
	GetOtherObjBtnPos(Pos, showXPos, showYPos);

	RECT rect;
	rect.left = showXPos - ImageWidth/2;
	rect.right = showXPos + ImageWidth/2;
	rect.top = showYPos - ImageHeight/2;
	rect.bottom = showYPos + ImageHeight/2;
	pBtn->SetWndRect(rect);
}

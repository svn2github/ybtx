// MapView.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptMgr.h"
#include "NewSceneEditor.h"
#include "MapView.h"
#include <math.h>
#include "ObjTrapPropertiesDlg.h"
#include "NpcProPertiesDlg.h"
#include "CBufFile.h"
#include "CPkgFile.h"
#include "StateMgr.h"
#include "EnumMgr.h"
#include "PlaceMgr.h"
#include "MathAlgorithm.h"
#include "SEPathMgr.h"

// CMapView

IMPLEMENT_DYNAMIC(CMapView, CStatic)

extern const wstring PATH_ALIAS_SCENE_EDITOR;

void FillRect(CDC* pDC,const Pos& centernPos, int size,COLORREF color, COLORREF frameColor = -1);
float GetRadianDir(int dir);
int TransDir(int gameDir);
int GetGameDir(const Vector2& vecDir)//方向向量
{
	float length = vecDir.Length();
	float sina = vecDir.x / length; //y轴正向夹角(游戏角为与y轴正向夹角)_的正弦值 
	float cosa = vecDir.y / length;//y轴正向夹角的余弦值 
	double radian;

	radian = acos(cosa);
	int gameDir = (int)(radian / DIR_TO_RADIAN_RATE);
	if (sina < 0) //正角
	{
		gameDir = 255 - gameDir;
	}
	return TransDir(gameDir);
}



CMapView::CMapView():m_fScaling(1),m_iUpdateViewElementMark(eUES_NONE),m_lFrameCount(0)
					,m_pFocusElement(NULL),m_pFillRecord(NULL),m_pAreaContour(NULL),m_iPauseTimes(0)
{
	
}

CMapView::~CMapView()
{
	if(NULL != m_pFillRecord)
	{
		delete [] m_pFillRecord;
		m_pFillRecord = NULL;
	}
	if(NULL != m_pAreaContour)
	{
		delete [] m_pAreaContour;
		m_pAreaContour = NULL;
	}
	ClearCopy();
}


BEGIN_MESSAGE_MAP(CMapView, CStatic)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CMapView message handlers



void CMapView::OnMouseMove(UINT nFlags, CPoint point)
{
	FPos gridPos, preGridPos;
	ViewPosToGridPos(gridPos, Pos(point.x, point.y));
	ViewPosToGridPos(preGridPos, Pos(m_iCurCursorPosX, m_iCurCursorPosY));
	int xOffset = point.x - m_iCurCursorPosX;
	int yOffset = point.y - m_iCurCursorPosY;
	m_iCurCursorPosX = point.x;
	m_iCurCursorPosY = point.y;

	switch(CStateMgr::GetCurState())
	{
	case eMS_MOVE_MAP:
		MoveMap(xOffset, yOffset);
		break;
	case eMS_SELECT_ING:
		SetSelecting();
		break;
	case eMS_NORMAL:
		UpdateFocus();
		break;
	case eMS_PLACE_ELEMENT:
		UpdateFocus();
		break;
	case eMS_HAVE_FOCUS:
		UpdateFocus();
		break;
	case eMS_MOVE_ELEMENT:
	case eMS_MOVE_NEW_PATH_KEY_POS:
		OnMoveElement();
		break;
	case eMS_DRAW_AREA:
		if ((nFlags & MK_LBUTTON) == MK_LBUTTON)
		{
			if (m_curAreaColor != -1)
			{
				SetAreaImageColor(gridPos.x, gridPos.y, g_pMainDlg->GetPenSize() , m_curAreaColor);
			}
		}
		break;
	case eMS_ERASE_AREA:
		if ((nFlags & MK_LBUTTON) == MK_LBUTTON)
		{
			SetAreaImageColor(gridPos.x, gridPos.y, g_pMainDlg->GetPenSize() , -1);
		}
		break;
	case eMS_ROTATE_ELEMENT_ING:
		UpdateRotateElement();
		break;
	default:
	    break;
	}
	
	
	CStateMgr::Inst().DoEvent(eEVENT_MOUSE_MOVE_IN_VIEW);
	//g_pMainDlg->SetFocus();
	CStatic::OnMouseMove(nFlags, point);
}

void CMapView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
}

//xOffset : 左移负,右移正
//yOffset : 上移负,下移正
void CMapView::MoveMap(int xOffset, int yOffset)
{
	SetMapPos(m_iOffsetX + xOffset, m_iOffsetY + yOffset);
}

//xOffset : 左移负,右移正
//yOffset : 上移负,下移正
void CMapView::SetMapPos(int posX, int posY)
{
	if (!IsMapOk())
	{
		return;
	}
	if (m_nCurMapWidth + posX < m_nViewWidth)
	{
		posX = m_nViewWidth - m_nCurMapWidth;
	}

	if (m_nCurMapHeight + posY < m_nViewHeight)
	{
		posY = m_nViewHeight - m_nCurMapHeight;
	}
	if (posX > 0)
	{
		posX = 0;
	}
	if (posY > 0)
	{
		posY = 0;
	}
	m_iOffsetX = posX;
	m_iOffsetY = posY;
	SetUpdateMark(eUES_ALL);
}

void CMapView::MoveMapToCenter(float fGridX, float fGridy)
{
	int x = int(m_nViewWidth/2 - fGridX * m_nGridPixel * m_fScaling);
	int y =  int(m_nViewHeight/2 - (m_nMapHeight -(fGridy * m_nGridPixel)) * m_fScaling );
	SetMapPos(x, y);
}

void CMapView::MoveMapToCenter(const FPos& fPos)
{
	MoveMapToCenter(fPos.x, fPos.y);
}

void CMapView::MoveMapToSelectCenter()
{
	FPos fPos;
	GetVecElementCenter(fPos, m_vecSelectElement, eET_ALL);
	MoveMapToCenter(fPos);
}

void CMapView::ClearAllImage()
{
	m_ImageMap.Destroy();
	m_ImageArea.Destroy();
	m_ImageBlock.Destroy();
}

bool CMapView::IsMapOk()
{
	if (m_ImageMap.IsNull() || m_ImageBuffer.IsNull() || m_ImageArea.IsNull())
	{
		return false;
	}
	return true;
}

bool CMapView::LoadMap(const CString& pathName, const CString& mapFilePath,OUT CString& errorMessage)
{
	ClearAllImage();
	CString strPath = pathName;
	if(SUCCEEDED(m_ImageMap.Load(strPath.GetBuffer())))
	{
		m_nMapWidth = m_ImageMap.GetWidth();
		m_nMapHeight = m_ImageMap.GetHeight();
		LoadSceneInfo(mapFilePath);

		m_nGridPixel = m_nMapWidth / m_nMapGridX;
		m_ImageArea.Create(m_nMapGridX, m_nMapGridY, 32, CImage::createAlphaChannel);
		m_ImageBlock.Create(m_nMapGridX, m_nMapGridY, 32, CImage::createAlphaChannel);

		if(m_pFillRecord != NULL)
		{
			delete [] m_pFillRecord;
			m_pFillRecord = NULL;
		}
		m_pFillRecord = new bool[m_nMapGridX * m_nMapGridY];
		if(m_pAreaContour != NULL)
		{
			delete [] m_pAreaContour;
			m_pAreaContour = NULL;
		}
		m_pAreaContour = new bool[m_nMapGridX * m_nMapGridY];
		ReSetView();
		
		//读取障碍信息
		LoadBarrierData(mapFilePath);

		return true;
	}
	else
	{
		errorMessage = "地图加载失败! \n找不到 " + pathName;
		return false;
	}

}
bool CMapView::LoadSceneInfo(const CString& strPathName)
{
	string sSceneFileName =  CSEPathMgr::Inst().GetArtistPath()  + "/common/" + strPathName + ".amp";
	CPkgFile PkgFile;
	if( PkgFile.Open(PATH_ALIAS_SCENE_EDITOR.c_str(), sSceneFileName.c_str()) != eFE_SUCCESS )
		GenErr( "Can not open " + sSceneFileName + "!\n" );
	char* pBuf = new char[PkgFile.Size()];
	PkgFile.read( pBuf, PkgFile.Size() );
	sqr::CBufFile File( pBuf, PkgFile.Size() );
	PkgFile.Close();
	
	int Temp[256];
	void* pTemp = (void*)Temp;
	// magicid
	File.Read(pTemp, sizeof(int32));
	// version
	File.Read(pTemp, sizeof(int32));
	// desc
	File.Read(pTemp, 64);

	uint16 u16tmp;
	File.Read(&u16tmp, sizeof(uint16));
	m_nMapGridX = u16tmp * eRegionSpan;
	File.Read(&u16tmp, sizeof(uint16));
	m_nMapGridY = u16tmp * eRegionSpan;
	//后面的数据这里不关心了

	delete []pBuf;
	return true;
}


bool CMapView::LoadBarrierData(const CString& strPathName)
{
	string sRegionFile = CSEPathMgr::Inst().GetArtistPath()  + "/server/" + strPathName + "_s.arg";
	CPkgFile PkgFile;
	if( PkgFile.Open(PATH_ALIAS_SCENE_EDITOR.c_str(), sRegionFile.c_str()) != eFE_SUCCESS )
		GenErr(  "Can not open " + sRegionFile + "!\n" );
	char* pBuf = new char[PkgFile.Size()];
	PkgFile.read( pBuf, PkgFile.Size() );
	sqr::CBufFile File( pBuf, PkgFile.Size() );
	PkgFile.Close();

	//载入Region的头
	uint32 nMagicID, nVersion, nCount;
	File.read(&nMagicID, sizeof(uint32));
	File.read(&nVersion, sizeof(uint32));
	File.read(&nCount, sizeof(uint32));

	uint32 dwClient = 0;
	File.read(&dwClient, sizeof(uint32));


	int32 nOffset;
	uint32 nPos	= File.Tell();
	uint32 m_uHeightInRegion = m_nMapGridY / eRegionSpan;
	uint32 m_uWidthInRegion = m_nMapGridX / eRegionSpan;
	for( uint32 y = 0; y < m_uHeightInRegion; ++y )
	{
		for(uint32 x = 0; x < m_uWidthInRegion ; ++x )
		{		
			uint32 uIdx = y*m_uWidthInRegion + x;
			File.Seek( nPos + uIdx*sizeof( uint32 ), SEEK_SET );
			File.read( &nOffset, sizeof(uint32));
			if( nOffset >= 0 )
			{
				File.Seek( nOffset, SEEK_SET );
				LoadOneRegion( File, nCount, x, y);
			}
		}
	}
	delete[] pBuf;
	return true;
}

void CMapView::LoadOneRegion(CBufFile& File, uint32 nChunk, uint32 x, uint32 y)
{
	uint32 nPos	= File.Tell();
	// 载入每个区块
	uint32 nMagicID, nOffset, nSize;
	for(uint32 i = 0; i < nChunk; i++)
	{
		File.Seek(nPos, SEEK_SET);
		File.read(&nMagicID, sizeof(uint32));
		File.read(&nOffset, sizeof(uint32));
		File.read(&nSize, sizeof(uint32));
		nPos = File.Tell();

		File.Seek(nOffset, SEEK_SET);

		LoadOneChunk( File, x, y, nMagicID );
	}
}

const uint32	BARR = 'RRAB';
const uint32	GATR = 'RTAG';///< Block
const uint32	EBLK = 'KLBE';///< Exact Block

void CMapView::LoadOneChunk(CBufFile& File, uint32 x, uint32 y, uint32 uMagicID)
{
	switch(uMagicID)
	{
	case BARR:
	case GATR:
		uint8 nVal = 0, roadweight = 0;
		for(uint32 i = 0; i < eRegionSpan; ++i)
		{
			for(uint32 j = 0; j < eRegionSpan; ++j)
			{
				File.read( &(nVal) , sizeof(uint8) ); // 通过内存块拷贝的方式，读取障碍信息到变量(0, 1, 2, 3)
				File.read( &roadweight, sizeof(uint8));
				if ( nVal == 1)
				{
					SetBlockImageColor((x * eRegionSpan) + j,(y * eRegionSpan) + i,BLOCK_COLOR_LV1);
				}
				else if(nVal == 2)
				{
					SetBlockImageColor((x * eRegionSpan) + j,(y * eRegionSpan) + i,BLOCK_COLOR_LV2);
				}
				else if(nVal)
				{
					SetBlockImageColor((x * eRegionSpan) + j,(y * eRegionSpan) + i,BLOCK_COLOR_LV3);
				}
			}
		}
		break;
	}
}



void CMapView::ReSetView()
{
	m_fScaling =1;
	m_nCurMapWidth = m_nMapWidth;
	m_nCurMapHeight = m_nMapHeight;
	SetMapPos(0,0);
	
	SetUpdateMark(eUES_ALL);
	ReSetShowState();
	ViewPosToGridPos(m_fPosGameViewCenter, Pos(m_nViewWidth/2, m_nViewHeight/2));
}


void CMapView::ReSetShowState()
{
	for (int i = 0; i < eSS_COUNT; i++)
	{
		m_bShowStates[i] = true;
	}
	m_bShowStates[eSS_GAME_VIEW_FOLLOW] = false;
	m_bShowStates[eSS_AREA] = false;
	m_bShowStates[eSS_GRID_LINE] = false;
	m_bShowStates[eSS_IGNORE_BLOCK] = false;
	g_pMainDlg->UpdateShowBox();
}

void CMapView::RePaint()
{
	//cout << CStateMgr::Inst().GetCurState() << endl;
	++m_lFrameCount;
	if (m_iPauseTimes > 0)
	{
		--m_iPauseTimes;
		return;
	}
	if (!IsMapOk())
	{
		return;
	}
	UpdateAll();
	CDC* pDC = GetDC();
	CDC* pBufferDC = CDC::FromHandle(m_ImageBuffer.GetDC());
	//---------------------------------------------------------------
	DrawMap(pBufferDC);
	DrawBlock(pBufferDC);
	DrawGridLine(pBufferDC);

	DrawAllPath(pBufferDC);
	DrawAllDirection(pBufferDC);
	DrawTrap(pBufferDC);
	DrawObj(pBufferDC);

	DrawNpc(pBufferDC);
	DrawSelectElement(pBufferDC);
	DrawSelectRect(pBufferDC);
	DrawArea(pBufferDC);
	DrawPenRect(pBufferDC);
	DrawGameView(pBufferDC);
	DrawFocus(pBufferDC);//绘制焦点元素,放最后 使其不被其他覆盖,
	DrawPasteElement(pBufferDC);
	DrawRotateRound(pBufferDC);
	DrawMeasure(pBufferDC);

	m_ImageBuffer.Draw(pDC->m_hDC, 0,0,m_nViewWidth,m_nViewHeight);//ImageBuffer绘制到控件上
	m_ImageBuffer.ReleaseDC();
	ReleaseDC(pDC);
}

void CMapView::DrawArea(CDC* pDC)
{
	if (!IsShowSate(eSS_AREA) || m_ImageArea.IsNull())
	{
		return;
	}
	FRect fRect;
	GetGridRectOfView(fRect);
	int srcX = (int)floor(fRect.left);
	int srcY = (int)floor(m_nMapGridY - fRect.bottom);
	int srcW = (int)ceil(fRect.right - fRect.left);
	int srcH = (int)ceil(fRect.bottom - fRect.top);
	if (srcX < 0)
	{
		srcX = 0;
	}
	if (srcY < 0)
	{
		srcY = 0;
	}
	if (srcW > m_nMapGridX)
	{
		srcW = m_nMapGridX;
	}
	if (srcH > m_nMapGridY)
	{
		srcH = m_nMapGridY;
	}
	int distW = int(srcW * m_nGridPixel * m_fScaling);
	int distH = int(srcH * m_nGridPixel * m_fScaling);
	
	Pos pos;
	GridPosToViewPos(pos, FPos(float(srcX),float(m_nMapGridY - srcY)));
	m_ImageArea.AlphaBlend(pDC->m_hDC, pos.x, pos.y, distW, distH, srcX, srcY, srcW, srcH, m_AlphaValue, AC_SRC_OVER);
}

void CMapView::DrawBlock(CDC* pDC)
{
	if (!IsShowSate(eSS_BLOCK) || m_ImageBlock.IsNull() /*|| IsShowSate(eSS_AREA)*/)
	{
		return;
	}
	FRect fRect;
	GetGridRectOfView(fRect);
	int srcX = (int)floor(fRect.left);
	int srcY = (int)floor(m_nMapGridY - fRect.bottom);
	int srcW = (int)ceil(fRect.right - fRect.left);
	int srcH = (int)ceil(fRect.bottom - fRect.top);
	if (srcX < 0)
	{
		srcX = 0;
	}
	if (srcY < 0)
	{
		srcY = 0;
	}
	if (srcW > m_nMapGridX)
	{
		srcW = m_nMapGridX;
	}
	if (srcH > m_nMapGridY)
	{
		srcH = m_nMapGridY;
	}
	int distW = int(srcW * m_nGridPixel * m_fScaling);
	int distH = int(srcH * m_nGridPixel * m_fScaling);

	Pos pos;
	GridPosToViewPos(pos, FPos(float(srcX),float(m_nMapGridY - srcY)));
	m_ImageBlock.AlphaBlend(pDC->m_hDC, pos.x, pos.y, distW, distH, srcX, srcY, srcW, srcH, m_AlphaValue, AC_SRC_OVER);
}

void CMapView::DrawGridLine(CDC* pDC)
{
	static CPen pen(PS_SOLID  ,1, GRID_LINE_COLOR);

	if (!IsShowSate(eSS_GRID_LINE) || m_ImageArea.IsNull())
	{
		return;
	}
	FRect rect;
	GetGridRectOfView(rect);
	Pos pos;

	pDC->SelectObject(&pen);
	for (float x = rect.left + 1; x < rect.right; x += 1)
	{
		GridPosToViewPos(pos, FPos(float(int(x)), rect.top));
		pDC->MoveTo(pos.x, 0);
		pDC->LineTo(pos.x, m_nViewHeight);
	}
	for (float y = rect.top + 1; y < rect.bottom; y += 1)
	{
		GridPosToViewPos(pos, FPos(rect.left, float(int(y))));
		pDC->MoveTo(0, pos.y);
		pDC->LineTo(m_nViewWidth,  pos.y);
	}
}

void CMapView::DrawPenRect(CDC* pDC)
{
	static CPen pen(PS_SOLID,1, PEN_RECT_COLOR);

	if (!(IsShowSate(eSS_AREA) && (CStateMgr::IsState(eMS_DRAW_AREA) || CStateMgr::IsState(eMS_ERASE_AREA))))
	{
		return;
	}
	int size = g_pMainDlg->GetPenSize();
	FPos fPos;
	GetCurCursorGridPos(fPos);


	FRect fRect;
	fRect.left = (float)floor(fPos.x - (size - 1)/2);
	fRect.top = (float)floor(fPos.y - (size - 1)/2);
	fRect.right = fRect.left + size;
	fRect.bottom = fRect.top + size;
	Rect viewRect;
	GridRectToViewRect(viewRect, fRect);

	pDC->SelectObject(&pen);
	pDC->FrameRect(&viewRect.Correct().ToCRect(), &CBrush(SELECT_RECT_COLOR));
}

void CMapView::DrawPasteElement(CDC* pDC)
{
	if ( !CStateMgr::IsState(eMS_PASTE_ELEMENT) )
	{
		return;
	}
	FPos curCursorPos ;
	GetCurCursorGridPos(curCursorPos);
	
	CElement* pElement;
	FPos temp;
	for (int i = m_vecCopyElement.size()-1; i>=0; --i) 
	{
		pElement = m_vecCopyElement[i];
		temp = pElement->fPos;
		pElement->SetPos(curCursorPos + temp);
		pElement->Draw(pDC);
		switch(pElement->eType)
		{
		case eET_NPC:
			static_cast<PPlaceNpc>(pElement)->Arrow.Draw(pDC);
			break;
		case eET_OBJ:
			static_cast<PPlaceObj>(pElement)->Arrow.Draw(pDC);
			break;
		case eET_TRAP:
			static_cast<PPlaceTrap>(pElement)->Arrow.Draw(pDC);
		    break;
		default:
		    break;
		}

		pElement->fPos = temp;  //这里就不用了
	}
}

void CMapView::DrawRotateRound(CDC* pDC)
{
	static CPen pen(PS_SOLID,ROTATE_ROUND_SIZE, ROTATE_ROUND_COLOR);
	static CPen pen2(PS_SOLID,ROTATE_ROUND_SIZE, ROTATE_ROUND_FOCUS_COLOR);
	static CPen pen3(PS_SOLID,2, ROTATE_ROUND_CENTER_COLOR);
	static CPen pen4(PS_SOLID,2, MOVE_ROUND_FOCUS_COLOR); 
	
	if (! (CStateMgr::IsState(eMS_ROTATE_ELEMENT) || CStateMgr::IsState(eMS_ROTATE_ELEMENT_ING)) )
	{
		return;
	}
	Pos posCursor;
	Pos posCenter;
	GetCurCursorViewPos(posCursor);
	GridPosToViewPos(posCenter, m_fRectRotate.GetCenter());

	int fRadius = int (GetViewDistance((m_fRectRotate.left_top - m_fRectRotate.right_bottom).Length()) / 2) + ROTATE_ROUND_CENTER_RADIUS + 10;
	int iDis = (posCursor - posCenter).Length();

	if ( abs(iDis - fRadius) < SELECT_PRECISION + ROTATE_ROUND_SIZE || CStateMgr::IsState(eMS_ROTATE_ELEMENT_ING))
	{
		pDC->SelectObject(&pen2);
	}
	else
	{
		pDC->SelectObject(&pen);
	}
	pDC->Arc(posCenter.x - fRadius, posCenter.y - fRadius,
				   posCenter.x + fRadius, posCenter.y + fRadius, 
				   0,0,0,0);
	pDC->SelectObject(&pen3);
	pDC->Arc(posCenter.x - ROTATE_ROUND_CENTER_RADIUS, posCenter.y - ROTATE_ROUND_CENTER_RADIUS,
		posCenter.x + ROTATE_ROUND_CENTER_RADIUS, posCenter.y + ROTATE_ROUND_CENTER_RADIUS, 
		0,0,0,0);
}

void CMapView::DrawMeasure(CDC* pDC)
{
	static CPen pen(PS_SOLID,MEASURE_LINE_SIZE, MEASURE_LINE_COLOR);

	if (!(CStateMgr::IsState(eMS_MEASURE) || CStateMgr::IsFronState(eMS_MEASURE)) )
	{
		return;
	}
	pDC->SelectObject(&pen);
	int n = m_vecMeasurePos.size();
	Pos mousePos;
	GetCurCursorViewPos(mousePos);
	CString text;
	CRect rect;
	rect.left = mousePos.x + 10;
	rect.top = mousePos.y - 5;
	rect.right = rect.left + 100;
	rect.bottom = rect.top + 20;
	pDC->SetTextColor(MEASURE_TEXT_COLOR);
	pDC->SetBkColor(MEASURE_BACK_COLOR);
	if (n == 0)
	{
		text = "距离: 0.00";
		pDC->DrawText(text, &rect, DT_BOTTOM);
		return;
	}
	CPoint* pts = new CPoint[n+1];
	Pos posTemp;
	float fDis = 0;
	for (int i = 0; i< n; ++i)
	{
		if (i > 0)
		{
			fDis +=(m_vecMeasurePos[i] - m_vecMeasurePos[i-1]).Length();
		}
		GridPosToViewPos(posTemp,m_vecMeasurePos[i]);
		pts[i].x = posTemp.x;
		pts[i].y = posTemp.y;
	}
	FPos mousefPos;
	GetCurCursorGridPos(mousefPos);
	fDis += (mousefPos- m_vecMeasurePos[n-1]).Length();
	pts[n].x = mousePos.x;
	pts[n].y = mousePos.y;

	pDC->SelectObject(&pen);
	pDC->Polyline(pts, n+1);
	text.Format("距离: %0.2f", fDis);
	pDC->DrawText(text, &rect, DT_BOTTOM);
	delete [] pts;
	return;
}

void CMapView::DrawMap(CDC* pDC)
{
	pDC->FillSolidRect(0,0,m_nViewWidth, m_nViewHeight, RGB(50,255,128));
	
	int precisionOffsetX = (int)(((-m_iOffsetX / m_fScaling) - (int)(-m_iOffsetX / m_fScaling)) * m_fScaling);
	int precisionOffsetY = (int)(((-m_iOffsetY / m_fScaling) - (int)(-m_iOffsetY / m_fScaling)) * m_fScaling);
	
	m_ImageMap.Draw(pDC->m_hDC,-precisionOffsetX,-precisionOffsetY,				//放大后精度调整
		(int)(m_nViewWidth + MAX_SCALING),(int)(m_nViewHeight + MAX_SCALING),					// + MAX_SCALING 保证精度调整后无余边
		(int)(-m_iOffsetX/m_fScaling), (int)(-m_iOffsetY/m_fScaling),
		(int)((m_nViewWidth+ MAX_SCALING)/m_fScaling),(int)((m_nViewHeight + MAX_SCALING)/m_fScaling));
}

void CMapView::DrawNpc(CDC* pDC) 
{
	if (!m_bShowStates[eSS_NPC])
	{
		return;
	}
	Npc_Iter iter = m_vecCurViewNpc.begin();
	while (iter != m_vecCurViewNpc.end())
	{
		(*iter)->Draw(pDC);
		iter++;
	}
}


void CMapView::DrawAllDirection(CDC* pDC)
{
	if (!m_bShowStates[eSS_ARROW])
	{
		return;
	}
	DrawNpcDirection(pDC);
	DrawObjDirection(pDC);
	DrawTrapDirection(pDC);
	DrawPasteElementDirection(pDC);
}

void CMapView::DrawNpcDirection(CDC* pDC)
{
	if (!m_bShowStates[eSS_NPC])
	{
		return;
	}
	Npc_Iter iter = m_vecCurViewNpc.begin();
	while (iter != m_vecCurViewNpc.end())
	{
		(*iter)->Arrow.Draw(pDC);
		iter++;
	}
}

void CMapView::DrawObjDirection(CDC* pDC)
{
	if (!m_bShowStates[eSS_OBJ])
	{
		return;
	}
	Obj_Iter iter = m_vecCurViewObj.begin();
	while (iter != m_vecCurViewObj.end())
	{
		(*iter)->Arrow.Draw(pDC);
		iter++;
	}
}

void CMapView::DrawTrapDirection(CDC* pDC)
{
	if (!m_bShowStates[eSS_TRAP])
	{
		return;
	}
	Trap_Iter iter = m_vecCurViewTrap.begin();
	while (iter != m_vecCurViewTrap.end())
	{
		(*iter)->Arrow.Draw(pDC);
		iter++;
	}
}

void CMapView::DrawPasteElementDirection(CDC* pDC)
{
	
}


void CMapView::DrawAllPath(CDC* pDC)
{
	if (!m_bShowStates[eSS_NPC] || !m_bShowStates[eSS_PATH])
	{
		return;
	}
	//这里为了简单,就将所以Npc的巡逻路线都绘制了
	//因为不在视窗内的npc它的巡逻路线也可能出现在屏幕内, 即使进行巡逻点检测也会有2点不在视窗内但连线经过视窗,所以.....
	CCache::NpcBC_Iter iter = g_pCache->m_bcNpc.begin();
	while (iter !=  g_pCache->m_bcNpc.end())
	{
		DrawSingleNpcPath(pDC, *iter);
		iter++;
	}
}


void CMapView::DrawSingleNpcPath(CDC* pDC, PPlaceNpc pNpc)
{
	static CPen pen(PS_SOLID,PATH_LINE_SIZE, PATH_LINE_COLOR);
	static CPen pen2(PS_DOT ,1, PATH_LINE_COLOR);

	int n = pNpc->pathList.size();
	if (n == 0)
	{
		return;
	}
	n += 1;
	CPoint* pts = new CPoint[n+1];
	CPlaceNpc::Path_Iter iter = pNpc->pathList.begin();
	Pos posTemp;
	GridPosToViewPos(posTemp,pNpc->fPos);
	pts[n].x = pts[0].x = posTemp.x;
	pts[n].y = pts[0].y = posTemp.y;
	for (int i = 1; i < n; i++)
	{
		GridPosToViewPos(posTemp,(*iter)->fPos);
		pts[i].x = posTemp.x;
		pts[i].y = posTemp.y;
		iter++;
	}
	pDC->SelectObject(&pen);
	pDC->Polyline(pts, n);
	if (pNpc->moveType == CEnumMgr::Inst().GetMoveTypeID("循环巡逻"))
	{
		pDC->SelectObject(&pen2);
		pDC->SetBkColor(RGB(255,255,255));
		pDC->MoveTo(pts[n-1].x, pts[n-1].y);
		pDC->LineTo(pts[n].x, pts[n].y);
	}
	int id = 1;
	for (iter = pNpc->pathList.begin(); iter != pNpc->pathList.end(); iter++, id++)
	{
		(*iter)->Draw(pDC);
	}
	delete [] pts;
}


void CMapView::DrawObj(CDC* pDC) 
{
	if (!m_bShowStates[eSS_OBJ])
	{
		return;
	}
	Obj_Iter iter = m_vecCurViewObj.begin();
	while (iter != m_vecCurViewObj.end())
	{
		(*iter)->Draw(pDC);
		iter++;
	}
}

void CMapView::DrawTrap(CDC* pDC) 
{
	if (!m_bShowStates[eSS_TRAP])
	{
		return;
	}
	Trap_Iter iter = m_vecCurViewTrap.begin();
	while (iter != m_vecCurViewTrap.end())
	{
		(*iter)->Draw(pDC);
		iter++;
	}
}



void CMapView::DrawSelectRect(CDC* pDC)
{
	static CPen pen(PS_SOLID,1, DIR_ARROW_COLOR);

	if (CStateMgr::IsState(eMS_SELECT_ING))
	{
		pDC->SelectObject(&pen);
		pDC->FrameRect(&m_rectSelect.Correct().ToCRect(), &CBrush(SELECT_RECT_COLOR));
	}
}

void CMapView::DrawFocus(CDC* pDC)
{
	//这里的逻辑就是不覆盖关系 要对
	if (m_pFocusElement != NULL)//绘制焦点元素,放最后 使其不被其他覆盖,
	{
		switch(m_pFocusElement->eType)
		{
		case eET_NPC:
			{	
				if(m_bShowStates[eSS_NPC_FULL])
				{
					PPlaceNpc pNpc = static_cast<PPlaceNpc>(m_pFocusElement);
					DrawSingleNpcPath(pDC, pNpc);
					pNpc->Arrow.Draw(pDC);
				}
				m_pFocusElement->Draw(pDC);
			}
			break;
		case eET_ARROW:
			{
				PPlaceNpc pNpc = static_cast<PPlaceNpc> (static_cast<CArrow*>(m_pFocusElement)->pOwner);
				m_pFocusElement->Draw(pDC);
				pNpc->Draw(pDC);
			}
			break;
		case eET_PATH_LINE:
			{
				CPathLine* pPathLine = static_cast<CPathLine*>(m_pFocusElement);
				pPathLine->Draw(pDC);
				if (pPathLine->pFrontKeyPos == NULL)
				{
					pPathLine->pBackKeyPos->pOwner->Draw(pDC);
				}
				else
				{
					pPathLine->pFrontKeyPos->Draw(pDC);
				}
				pPathLine->pBackKeyPos->Draw(pDC);
			}
			break;
		default:
			m_pFocusElement->Draw(pDC);
			break;
		}
	}
}

void CMapView::DrawSelectElement(CDC* pDC)
{
	if (m_vecSelectElement.size() == 0)
	{
		return;
	}
	Pos posTemp;
	CElement* pElement;
	for (int i = m_vecSelectElement.size() -1; i >=0 ; i--)
	{
		pElement = m_vecSelectElement[i];
		GridPosToViewPos(posTemp, pElement->fPos);
		FillRect(pDC, posTemp, SELECT_MARK_SIZE, -1, SELECT_MARK_COLOR);
	}
}



void CMapView::DrawGameView(CDC* pDC)
{
	static CPen pen(PS_GEOMETRIC  ,GAME_VIEW_LIEN_SIZE, GAME_VIEW_COLOR);

	if (!m_bShowStates[eSS_GAME_VIEW] && !CStateMgr::IsAssistState(eAS_TAB))
	{
		return;
	}
	FPos centerPos;
	if (m_bShowStates[eSS_GAME_VIEW_FOLLOW] || CStateMgr::IsAssistState(eAS_TAB))
	{
		if(!GetCurCursorGridPos(centerPos))
		{
			centerPos  = m_fPosGameViewCenter;
		}
	}
	else
	{
		centerPos = m_fPosGameViewCenter;
	}
	FPos gameViewPos[4];//游戏窗口的四个角
	gameViewPos[0].x =  - GAME_VIEW_TOP_LENGTH / 2;
	gameViewPos[1].x = GAME_VIEW_TOP_LENGTH / 2;
	gameViewPos[0].y = gameViewPos[1].y = GAME_VIEW_TOP_DIS;
	gameViewPos[3].x =  - GAME_VIEW_BOTTOM_LENGTH / 2;
	gameViewPos[2].x = GAME_VIEW_BOTTOM_LENGTH / 2;
	gameViewPos[3].y = gameViewPos[2].y = - GAME_VIEW_BOTTOM_DIS;
	double sina = sin(GAME_VIEW_ANGLE);
	double cosa = cos(GAME_VIEW_ANGLE);
	Pos posTemp;
	CPoint pts[4];
	float tempX, tempY;
	for (int i = 0; i < 4; i++)
	{
		tempX = float(gameViewPos[i].x * cosa + gameViewPos[i].y * sina);
		tempY = float(gameViewPos[i].y * cosa - gameViewPos[i].x * sina);
		gameViewPos[i].x = tempX + centerPos.x;
		gameViewPos[i].y = tempY + centerPos.y;
		GridPosToViewPos(posTemp,gameViewPos[i]);
		pts[i].x = posTemp.x;
		pts[i].y = posTemp.y;
	}

	pDC->SelectObject(&pen);
	pDC->SelectStockObject(NULL_BRUSH);
	pDC->Polygon(pts, 4);

	m_fPosGameViewCenter = centerPos;
}


void CMapView::Init()
{
	RECT rc1, rc2;
	GetClientRect(&rc1);
	m_nViewWidth = rc1.right - rc1.left;
	m_nViewHeight = rc1.bottom - rc1.top;
	m_ImageBuffer.Create(m_nViewWidth,m_nViewHeight,32);
	GetWindowRect(&rc2);
	m_iViewOffsetX = rc2.left;
	m_iViewOffsetY = rc2.top;
}


bool CMapView::GetCurCursorGridPos(OUT FPos& gridPos) const
{
	bool isInViewMap;
	Pos viwPos;
	isInViewMap = GetCurCursorViewPos(viwPos);
	ViewPosToGridPos(gridPos, viwPos);
	if(isInViewMap)
	{
		if (gridPos.x < 0 || gridPos.x > m_nMapGridX
			||gridPos.y < 0 || gridPos.y > m_nMapGridY)
		{
			isInViewMap = false;
		}
	}
	return isInViewMap;
}

bool CMapView::IsCursorInView()
{
	CPoint pos;
	GetCursorPos(&pos);
	ScreenToClient(&pos); 
	if (pos.x < 0 || pos.x > m_nViewWidth
		|| pos.y < 0||pos.y > m_nViewHeight)
	{
		return false;
	}
	return true;
}

bool CMapView::GetCurCursorViewPos(OUT Pos& pos) const
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt); 
	pos.x = pt.x;
	pos.y = pt.y;
	if (pos.x < 0 || pos.x > m_nViewWidth
		|| pos.y < 0||pos.y > m_nViewHeight)
	{
		return false;
	}
	return true;
}
int CMapView::CurCursorX() const
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt); 
	return pt.x;
}
int CMapView::CurCursorY() const
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt); 
	return pt.y;
}

inline void CMapView::GetGridRectOfView(OUT FRect& gridFRect) const
{
	ViewRectToGridRect(gridFRect, Rect(0,0,m_nViewWidth,m_nViewHeight));
}



//基础转换方法 所有坐标转换来自他们,  
inline void CMapView::GridPosToViewPos(OUT Pos& viewPos, const FPos& gridFPos) const
{
	viewPos.x = (int)((gridFPos.x * m_nGridPixel * m_fScaling) + m_iOffsetX);
	viewPos.y =  (int)((m_nMapHeight -(gridFPos.y * m_nGridPixel)) * m_fScaling + m_iOffsetY);
}
//基础转换方法, 当地图小于视窗时候, gridFPos会出现负数现象,为保证方法的通用性,这里不处理,必要场合注意处理
inline void CMapView::ViewPosToGridPos(OUT FPos& gridFPos, const Pos& viewPos) const
{
	gridFPos.x = (viewPos.x - m_iOffsetX) / (m_nGridPixel * m_fScaling);
	gridFPos.y = (m_nMapHeight - (viewPos.y - m_iOffsetY) / m_fScaling) / m_nGridPixel;
}

inline void CMapView::GridRectToViewRect(OUT Rect& viewRect, const FRect& gridFRect) const
{
	FPos grid_left_top, grid_right_bottom;
	Pos view_left_top, view_right_bottom;//地图左上对应屏蔽左下, 地图右下对应屏蔽右上
	
	grid_left_top.x = gridFRect.left;
	grid_left_top.y = gridFRect.top;
	grid_right_bottom.x = gridFRect.right;
	grid_right_bottom.y = gridFRect.bottom;


	GridPosToViewPos(view_left_top, grid_left_top); 
	GridPosToViewPos(view_right_bottom, grid_right_bottom);
	
	viewRect.left = view_left_top.x;
	viewRect.bottom = view_left_top.y;
	viewRect.right = view_right_bottom.x;
	viewRect.top = view_right_bottom.y;
}

inline void CMapView::ViewRectToGridRect(OUT FRect& gridFRect, const Rect& viewRect) const
{
	FPos grid_left_top, grid_right_bottom;
	Pos view_left_top, view_right_bottom;//地图左上对应屏蔽左下, 地图右下对应屏蔽右上

	view_left_top.x = viewRect.left;
	view_left_top.y = viewRect.bottom;
	view_right_bottom.x = viewRect.right;
	view_right_bottom.y = viewRect.top;
	
	ViewPosToGridPos(grid_left_top, view_left_top); 
	ViewPosToGridPos(grid_right_bottom, view_right_bottom);
	gridFRect.left = grid_left_top.x;
	gridFRect.top = grid_left_top.y;
	gridFRect.right = grid_right_bottom.x;
	gridFRect.bottom = grid_right_bottom.y;
}


bool CMapView::SetScaling(float fScaling)
{
	if (!IsMapOk())
	{
		return false;
	}
	if (fScaling >= MIN_SCALING && fScaling <= MAX_SCALING)
	{
		//计算中心点 的实际位置, 用来重新设置地图位置保证观察的中心不变
		int viewCenterX =(int) (((m_nViewWidth / 2 - m_iOffsetX) / m_fScaling ) * fScaling);
		int viewCenterY = (int) (((m_nViewHeight / 2 - m_iOffsetY) / m_fScaling ) * fScaling);

		m_fScaling = fScaling;
		m_nCurMapWidth = (int)(m_fScaling * m_nMapWidth);
		m_nCurMapHeight = (int)(m_fScaling * m_nMapHeight);
		SetMapPos(m_nViewWidth / 2 - viewCenterX, m_nViewHeight / 2 - viewCenterY);
		SetUpdateMark(eUES_ALL);
		return true;
	}
	return false;
}



void CMapView::SetUpdateMark(EUpdateElementState eState)
{
	if (eUES_NONE == eState)
	{
		m_iUpdateViewElementMark = eUES_NONE;
		return;
	}
	if (eState == eUES_NPC)
	{
		SetUpdateMark(eUES_PATH_KEY_POS); //更新NPC必更新PATH_KEY_POS
	}
	m_iUpdateViewElementMark |= eState;
	
}

void CMapView::RemoveUpdateMark(EUpdateElementState eState)
{
	m_iUpdateViewElementMark &= ~eState;
}

bool CMapView::IsUpdate(EUpdateElementState eState)
{
	if (eUES_NONE == eState)
	{
		if (m_iUpdateViewElementMark == eState)
		{
			return true;
		}
		return false;
	}
	return (m_iUpdateViewElementMark & eState) == eState;
}

void CMapView::UpdateAll()
{
	if (IsUpdate(eUES_NONE))
	{
		return;
	}
	if (IsUpdate(eUES_NPC))
	{
		UpdateNpc();
	}
	if (IsUpdate(eUES_OBJ))
	{
		UpdateObj();
	}
	if (IsUpdate(eUES_TRAP))
	{
		UpdateTrap();
	}
}

void CMapView::UpdateNpc()
{
	FRect viewFRcet;
	GetGridRectOfView(viewFRcet);
	g_pCache->GetNpcFromRect(m_vecCurViewNpc, viewFRcet);
	RemoveUpdateMark(eUES_NPC);
}

void CMapView::UpdateObj()
{
	FRect viewFRcet;
	GetGridRectOfView(viewFRcet);
	g_pCache->GetObjFromRect(m_vecCurViewObj, viewFRcet);
	RemoveUpdateMark(eUES_OBJ);
}

void CMapView::UpdateTrap()
{
	FRect viewFRcet;
	GetGridRectOfView(viewFRcet);
	g_pCache->GetTrapFromRect(m_vecCurViewTrap, viewFRcet);
	RemoveUpdateMark(eUES_TRAP);
}

void CMapView::SetSelectStart()
{
	m_rectSelect.right = m_rectSelect.left = CurCursorX();
	m_rectSelect.bottom = m_rectSelect.top = CurCursorY();
	if (!CStateMgr::IsAssistState(eAS_SHIFT) && !CStateMgr::IsAssistState(eAS_CTRL))
	{
		ClearSelectElement();
	}
	else
	{
		m_vecSelectSpare.clear();
		for (int i = m_vecSelectElement.size()-1; i >=0; i--)
		{
			m_vecSelectSpare.push_back(m_vecSelectElement[i]);
		}
	}
}

void CMapView::SetSelecting()
{
	m_rectSelect.right = CurCursorX();
	m_rectSelect.bottom = CurCursorY();
	Element_Vec findVec;
	FRect fRect;
	ViewRectToGridRect(fRect, m_rectSelect);
	int findMark = GetShowElementMark();
	if (FindElement(findVec, fRect,findMark) == 0)
	{
		return;
	}
	ClearSelectElement();
	if (CStateMgr::IsAssistState(eAS_SHIFT))//加选
	{
		for (int i = m_vecSelectSpare.size()-1; i >=0; i--)
		{
			m_vecSelectElement.push_back(m_vecSelectSpare[i]);
		}
		for (int i = findVec.size()-1; i >=0; i--)
		{
			AddSelectElement(findVec[i]);
		}
	}
	else if(CStateMgr::IsAssistState(eAS_CTRL))//反选
	{
		for (int i = m_vecSelectSpare.size()-1; i >=0; i--)
		{
			m_vecSelectElement.push_back(m_vecSelectSpare[i]);
		}
		for (int i = findVec.size()-1; i >=0; i--)
		{
			if (IsSelectElement(findVec[i]))
			{
				RemoveSelectElement(findVec[i]);
			}
			else
			{
				m_vecSelectElement.push_back(findVec[i]);
			}
		}
	}
	else
	{
		for (int i = findVec.size()-1; i >=0; i--)
		{
			m_vecSelectElement.push_back(findVec[i]);
		}
	}
}

void CMapView::OnLButtonDown(UINT nFlags, CPoint point)
{
	FPos gridPos;
	ViewPosToGridPos(gridPos, Pos(point.x, point.y));
	EMainState eCurState = CStateMgr::GetCurState();

	switch(CStateMgr::GetCurState())
	{
	case eMS_PLACE_ELEMENT:
		{
			CElement* pElement = CPlaceMgr::Inst().GetPlaceElement();
			if(pElement != NULL)
			{
				PutElement(pElement, gridPos);
			}
		}
		break;
	case eMS_HAVE_FOCUS:
		if (m_pFocusElement->eType != eET_NPC && m_pFocusElement->eType != eET_OBJ 
			&& m_pFocusElement->eType != eET_TRAP && m_pFocusElement->eType != eET_PATH_KEY_POS)
		{
			break;
		}
		if (CStateMgr::IsAssistState(eAS_SHIFT))
		{
			AddSelectElement(m_pFocusElement);
		}
		else if(CStateMgr::IsAssistState(eAS_CTRL))
		{
			if (IsSelectElement(m_pFocusElement))
			{
				RemoveSelectElement(m_pFocusElement);
			}
			else
			{
				AddSelectElement(m_pFocusElement);
			}
		}
		else
		{
			if(!IsSelectElement(m_pFocusElement))
			{
				ClearSelectElement();
			}
			AddSelectElement(m_pFocusElement);
		}
		break;
	case eMS_DRAW_AREA:
		{
			m_curAreaColor = g_pMainDlg->GetCurAreaColor();
			if (m_curAreaColor != -1)
			{
				SetAreaImageColor(gridPos.x, gridPos.y, g_pMainDlg->GetPenSize() , m_curAreaColor);
			}
		}
		break;
	case eMS_ERASE_AREA:
		SetAreaImageColor(gridPos.x, gridPos.y, g_pMainDlg->GetPenSize() , -1);
		break;
	case eMS_FILL_AREA:
		{
			m_curAreaColor = g_pMainDlg->GetCurAreaColor();
			if (m_curAreaColor != -1)
			{
				FillArea(int(gridPos.x), int(gridPos.y), m_curAreaColor);
			}
		}
		break;
	case eMS_PASTE_ELEMENT:
		PutPasteElement();
		return;
	case eMS_ROTATE_ELEMENT:
		if (IsCursorOnRotateRound())
		{
			BeginRotateElement();
		}
		return;
	case eMS_MEASURE:
		AddMeasurePos(gridPos);
		break;
	default:
	    break;
	}
	
	CStateMgr::Inst().DoEvent(eEVENT_L_DOWN_IN_VIEW);
	if ( CStateMgr::IsState(eMS_SELECT_BEGIN))
	{
		SetSelectStart();
	}

	CStatic::OnLButtonDown(nFlags, point);
}

void CMapView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CStateMgr::Inst().DoEvent(eEVENT_L_UP_IN_VIEW);

	CStatic::OnLButtonUp(nFlags, point);
}

void CMapView::EditElement(CElement* pElement)
{
	CPoint cursorPos;
	GetCursorPos(&cursorPos);
	cursorPos.x += 10;
	switch(pElement->eType)
	{
	case eET_NPC:
		{
			CNpcProPertiesDlg dlg(static_cast<PPlaceNpc>(pElement), cursorPos);
			g_pMapView->PauseRender();
			if( dlg.DoModal() == IDOK)
			{
				SetUpdateMark(eUES_NPC);
			}
		}
		break;
	case eET_OBJ:
		{
			PPlaceObj pObj= static_cast<PPlaceObj>(pElement);
			CObjTrapPropertiesDlg dlg("OBJ",pObj->name, pObj->fPos, cursorPos);
			g_pMapView->PauseRender();
			dlg.DoModal();
		}
		break;
	case eET_TRAP:
		{
			PPlaceTrap pTrap= static_cast<PPlaceTrap>(pElement);
			CObjTrapPropertiesDlg dlg("TRAP",pTrap->name, pTrap->fPos, cursorPos);
			g_pMapView->PauseRender();
			dlg.DoModal();
		}
		break;
	case eET_PATH_KEY_POS:
		{

		}
		break;
	default:
		break;
	}
}

void CMapView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (m_pFocusElement != NULL)
	{
		ClearSelectElement();
		EditElement(m_pFocusElement);
	}
	else
	{
		CStateMgr::Inst().DoEvent(eEVENT_R_DOWN_IN_VIEW);
	}
	
	
	CStatic::OnRButtonDown(nFlags, point);
}

void CMapView::OnMButtonDown(UINT nFlags, CPoint point)
{
	FPos gridPos;
	ViewPosToGridPos(gridPos, Pos(point.x, point.y));
	if (IsShowSate(eSS_AREA))
	{
		int color = GetAreaColor(int(gridPos.x), int(gridPos.y));
		CString name  =  g_pCache->GetAreaName(color);
		g_pMainDlg->SelectArea(name);
	}
	CStatic::OnMButtonDown(nFlags, point);
}


void CMapView::OnRButtonUp(UINT nFlags, CPoint point)
{
	CStateMgr::Inst().DoEvent(eEVENT_R_UP_IN_VIEW);

	CStatic::OnRButtonUp(nFlags, point);
}

inline float CMapView::GetGridDistance(float viewDis) const
{
	return (viewDis/ m_fScaling / (float)m_nGridPixel);
}

inline float CMapView::GetViewDistance(float gridDis) const
{
	return (gridDis * m_nGridPixel * m_fScaling);
}

int CMapView::GetShowElementMark(int rangeMark) const
{
	int findMark = 0;
	if (m_bShowStates[eSS_NPC] && (rangeMark & eET_NPC) != 0)
	{
		findMark |= eET_NPC;
		if (m_bShowStates[eSS_PATH] && (rangeMark & eET_PATH_KEY_POS) != 0)
		{
			findMark |= eET_PATH_KEY_POS;
		}
	}
	if (m_bShowStates[eSS_OBJ]&& (rangeMark & eET_OBJ) != 0)
	{
		findMark |= eET_OBJ;
	}
	if (m_bShowStates[eSS_TRAP]&& (rangeMark & eET_TRAP) != 0)
	{
		findMark |= eET_TRAP;
	}
	return findMark;
}

CElement* CMapView::FindNearestElement(const FPos& fPos, float fRange)
{
	fRange = GetGridDistance(fRange); //换成格子大小
	Element_Vec vec;
	FRect fRect(fPos.x - fRange, fPos.y - fRange, fPos.x + fRange, fPos.y + fRange);
	int findMark = GetShowElementMark();
	if (FindElement(vec, fRect,findMark) == 0)
	{
		return NULL;
	}
	if (vec.size() == 1)
	{
		return vec[0];
	}
	float fRSquare;
	float fMinRSquare;
	CElement* pNearestElement = vec[0];
	float cx, cy;
	cx = vec[0]->fPos.x - fPos.x;
	cy = vec[0]->fPos.y - fPos.y;
	fMinRSquare = cx * cx + cy * cy;
	for (UINT i = 1; i < vec.size(); i++)
	{
		cx = vec[i]->fPos.x - fPos.x;
		cy = vec[i]->fPos.y - fPos.y;
		fRSquare = cx * cx + cy * cy;
		if (fMinRSquare > fRSquare)
		{
			fMinRSquare = fRSquare;
			pNearestElement = vec[i];
		}
	}
	return pNearestElement;
}

int CMapView::FindElement(OUT Element_Vec& vec, const FRect& fRect, int mark)const
{
	//用于 Element_Vec& 在模板中类型不能转换,  只好暂时用这个较笨的方法, 有时间在好好想一下怎么类型转换
	if((mark & eET_NPC) == eET_NPC)
	{
		vector<PPlaceNpc> vecNpc;
		g_pCache->GetNpcFromRect(vecNpc, fRect);
		for (UINT i = 0; i < vecNpc.size(); i++)
		{
			vec.push_back(static_cast<CElement*>(vecNpc[i]));
		}
	}
	if((mark & eET_OBJ) == eET_OBJ)
	{
		vector<PPlaceObj> vecObj;
		g_pCache->GetObjFromRect(vecObj, fRect);
		for (UINT i = 0; i < vecObj.size(); i++)
		{
			vec.push_back(static_cast<CElement*>(vecObj[i]));
		}
	}
	if((mark & eET_TRAP) == eET_TRAP)
	{
		vector<PPlaceTrap> vecTrap;
		g_pCache->GetTrapFromRect(vecTrap, fRect);
		for (UINT i = 0; i < vecTrap.size(); i++)
		{
			vec.push_back(static_cast<CElement*>(vecTrap[i]));
		}
	}
	if((mark & eET_PATH_KEY_POS) == eET_PATH_KEY_POS)
	{
		vector<PPathKeyPos> vecKeyPos;
		g_pCache->GetPathKeyPosFromRect(vecKeyPos, fRect);
		for (UINT i = 0; i < vecKeyPos.size(); i++)
		{
			vec.push_back(static_cast<CElement*>(vecKeyPos[i]));
		}
	}
	return vec.size();
}

CArrow* CMapView::FindArrow(const FPos& fPos) const
{
	if(!m_bShowStates[eSS_ARROW] || g_eModeType == eMT_PATH_EDIT)
	{
		return NULL;
	}
	float range = GetGridDistance(DIR_ARROW_SIZE);
	FRect fRect(fPos.x - range, fPos.y - range, 
				fPos.x + range, fPos.y + range);
	Element_Vec vecElement;
	int mark = GetShowElementMark(eET_NPC|eET_OBJ|eET_TRAP);
	if (FindElement(vecElement, fRect, mark) == 0)
	{
		return NULL;
	}
	CArrow* pArrow = NULL;
	CArrow* pArrowTemp;
	Pos  arrowHeadView;
	FPos arrowHeadGrid;
	float fRange = GetGridDistance(SELECT_PRECISION);
	float disNearest = fRange;
	float disTemp;
	for (UINT i = 0; i < vecElement.size(); i++)
	{
		pArrowTemp = NULL;
		switch(vecElement[i]->eType)
		{
		case eET_NPC:
			pArrowTemp = &(static_cast<PPlaceNpc>(vecElement[i])->Arrow);
			break;
		case eET_OBJ:
			pArrowTemp = &(static_cast<PPlaceObj>(vecElement[i])->Arrow);
			break;
		case eET_TRAP:
			pArrowTemp = &(static_cast<PPlaceTrap>(vecElement[i])->Arrow);
		    break;
		}
		GridPosToViewPos(arrowHeadView, vecElement[i]->fPos);
		arrowHeadView += pArrowTemp->arrowHeadPos;
		ViewPosToGridPos(arrowHeadGrid, arrowHeadView);
		if(IsPosNearLine(disTemp,vecElement[i]->fPos,arrowHeadGrid,fPos,fRange))
		{
			if (disNearest > disTemp)
			{
				disNearest = disTemp;
				pArrow = pArrowTemp;
			}
		}
	}
	return pArrow;
}

CPathLine* CMapView::FindPathLine(const FPos& fPos) const
{
	if(!m_bShowStates[eSS_NPC]||!m_bShowStates[eSS_PATH])
	{
		return NULL;
	}
	CCache::NpcBC_Iter iter = g_pCache->m_bcNpc.begin();
	PPlaceNpc pNpc;
	float range = GetGridDistance(SELECT_PRECISION);
	float disNearest = range;
	float disTemp;
	m_PathLine.pFrontKeyPos = NULL;
	m_PathLine.pBackKeyPos = NULL;
	FPos pos1, pos2;
	CPathLine limeTemp;
	
	while (iter !=  g_pCache->m_bcNpc.end())
	{
		pNpc  =  *iter;
		if (pNpc->pathList.size() != 0)
		{
			
			limeTemp.pFrontKeyPos = NULL;
			limeTemp.pBackKeyPos = NULL;
			CPlaceNpc::Path_Iter pathIter = pNpc->pathList.begin();
			pos1 = pNpc->fPos;
			PPathKeyPos preKeyPos = NULL;
			while (pathIter != pNpc->pathList.end())
			{
				pos2 = (*pathIter)->fPos;
				if(IsPosNearLine(disTemp, pos1, pos2, fPos, range))
				{
					if (disNearest >= disTemp)
					{
						disNearest = disTemp;
						limeTemp.pFrontKeyPos = preKeyPos;
						limeTemp.pBackKeyPos = *pathIter;
					}
				}
				preKeyPos = *pathIter;
				++pathIter;
				pos1 = pos2;

			}
			if(limeTemp.pBackKeyPos != NULL)
			{
				m_PathLine.pFrontKeyPos = limeTemp.pFrontKeyPos;
				m_PathLine.pBackKeyPos  = limeTemp.pBackKeyPos;
			}
		}
		++iter;
	}
	if (m_PathLine.pBackKeyPos != NULL)
	{
		return &m_PathLine;
	}
	else
	{
		return NULL;
	}
}


void CMapView::UpdateFocus()
{
	FPos pos;
	GetCurCursorGridPos(pos);
	CElement* pElement;
	pElement = FindNearestElement(pos, SELECT_PRECISION);

	if (m_pFocusElement != NULL)
	{
		m_pFocusElement->RemoveMark(eSM_FOCUS);
	}
	if (pElement == NULL)
	{
		pElement = FindArrow(pos);
	}
	if(pElement == NULL)
	{
		pElement = FindPathLine(pos);
	}
	SetMyFocus(pElement);
}

void CMapView::SetMyFocus(CElement* pElement)
{
	if (m_pFocusElement != NULL)
	{
		m_pFocusElement->RemoveMark(eSM_FOCUS);
	}
	if (pElement != NULL)
	{
		pElement->SetMark(eSM_FOCUS);
		CStateMgr::Inst().DoEvent(eEVENT_GET_FOCUS);
	}
	else
	{
		CStateMgr::Inst().DoEvent(eEVENT_LOST_FOCUS);
	}
	m_pFocusElement = pElement;
}

bool CMapView::IsSelectElement(CElement* pElement)
{
	for (int i = m_vecSelectElement.size()-1; i >=0; i--)
	{
		if(m_vecSelectElement[i] == pElement)
		{
			return true;
		}
	}
	return false;
}

bool CMapView::AddSelectElement(CElement* pElement)
{
	if (IsSelectElement(pElement))
	{
		return false;
	}
	m_vecSelectElement.push_back(pElement);
	return true;
}

void CMapView::SelectColony(int iColonyId)
{
	ClearSelectElement();
	g_pCache->GetColonyMember(m_vecSelectElement, iColonyId);
}

int  CMapView::RemoveSelectElement(int eType)
{
	int count = 0;
	Element_Iter iter = m_vecSelectElement.begin();
	while (iter != m_vecSelectElement.end())
	{
		CElement* pElement = *iter;
		if ((pElement->eType & eType) != 0)
		{
			iter = m_vecSelectElement.erase(iter);
			 ++ count;
		}
		else
		{
			++iter;
		}
	}
	m_vecSelectSpare.clear();
	return count;
}

bool CMapView::RemoveSelectElement(CElement* pElement)
{
	Element_Iter iter = m_vecSelectElement.begin();
	while (iter != m_vecSelectElement.end())
	{
		if (*iter == pElement)
		{
			break;
		}
		 ++ iter;
	}
	if (iter != m_vecSelectElement.end())
	{
		m_vecSelectElement.erase(iter);
		return true;
	}
	else
	{
		return false;
	}
}

void CMapView::ClearSelectElement()
{
	m_vecSelectElement.clear();
}

bool CMapView::GetVecElementRect(OUT FRect& fRect, const Element_Vec& srcVec, int eType)
{
	float fMinX = (float)m_nMapGridX, fMinY = (float)m_nMapGridY, fMaxX = 0, fMaxY = 0;
	const CElement* pElement;
	bool isHave = false;
	for (int i = srcVec.size()-1; i >=0; i--)
	{
		pElement = srcVec[i];
		if ((pElement->eType & eType) != 0)
		{
			if (pElement->fPos.x < fMinX)
				fMinX = pElement->fPos.x;
			if (pElement->fPos.y < fMinY)
				fMinY = pElement->fPos.y;
			if (pElement->fPos.x > fMaxX)
				fMaxX = pElement->fPos.x;
			if (pElement->fPos.y > fMaxY)
				fMaxY = pElement->fPos.y;
			isHave = true;
		}
	}
	if (isHave)
	{
		fRect.minX = fMinX;
		fRect.minY = fMinY;
		fRect.maxX = fMaxX;
		fRect.maxY = fMaxY;
		return true;
	}
	return false;
}

bool CMapView::GetVecElementCenter(OUT FPos& fPosCenter, const Element_Vec& srcVec, int eType)
{
	FRect fRect;
	if (GetVecElementRect(fRect, srcVec, eType))
	{
		fRect.GetCenter(fPosCenter);
		return true;
	}
	return false;
}

void CMapView::ClearCopy()
{
	for (int i = m_vecCopyElement.size()-1; i>=0; --i) 
	{
		delete m_vecCopyElement[i];
	}
	m_vecCopyElement.clear();
}

void CMapView::ClearRotateBasic()
{
	for (int i = m_vecRotateBasic.size()-1; i>=0; --i) 
	{
		delete m_vecRotateBasic[i];
	}
	m_vecRotateBasic.clear();
}
void CMapView::CopeyVecElement(OUT Element_Vec& destVec, const Element_Vec& srcVec, int type)
{
	CElement* pElement;
	for (uint i = 0; i < srcVec.size(); ++i) 
	{
		pElement = srcVec[i];
		if ((type & pElement->eType) == 0)
		{
			continue;
		}
		switch(pElement->eType)
		{
		case eET_NPC:
			{
				PPlaceNpc pNpc = new PlaceNpc();
				CPlaceNpc::CopyeNpc(pNpc, static_cast<PPlaceNpc>(pElement));
				destVec.push_back(static_cast<CElement*>(pNpc));
			}
			break;
		case eET_OBJ:
			{
				PPlaceObj pObj = new PlaceObj();
				pObj->name = pElement->name;
				pObj->fPos = pElement->fPos;
				pObj->direction = pElement->direction;
				destVec.push_back(static_cast<CElement*>(pObj));
			}
			break;
		case eET_TRAP:
			{
				PPlaceTrap pTrap = new PlaceTrap();
				pTrap->name = pElement->name;
				pTrap->fPos = pElement->fPos;
				pTrap->direction = pElement->direction;
				destVec.push_back(static_cast<CElement*>(pTrap));
			}
			break;
		case eET_PATH_KEY_POS:
			{
				PPathKeyPos pPathKeyPos = new PathKeyPos();
				pPathKeyPos->fPos = pElement->fPos;
				destVec.push_back(static_cast<CElement*>(pPathKeyPos));
			}
			break;
		default:
			continue;
		}
	}
}
bool CMapView::CheckUserCopy(Element_Vec& vecSrc, OUT CString& msg)
{
	int count = vecSrc.size();
	CElement* pElement;
	PPlaceNpc pNpc;
	int colonyId = 0;
	for (int i= 0; i < count; ++i)
	{
		pElement = vecSrc[i];
		if (pElement->eType == eET_NPC)
		{
			pNpc = static_cast<PPlaceNpc>(pElement);
			if (pNpc->colonyId > 0)
			{
				colonyId = pNpc->colonyId;
				break;
			}
		}
	}
	if (colonyId > 0)
	{
		for (int i= 0; i < count; ++i)
		{
			pElement = vecSrc[i];
			if (pElement->eType == eET_NPC)
			{
				pNpc = static_cast<PPlaceNpc>(pElement);
				if (pNpc->colonyId != colonyId)
				{
					msg = "不能同时复制不同的群组";
					return false;
				}
			}
			else
			{
				msg = "群组成员不能和其他的对象一起复制";
				return false;
			}
		}
	}
	return true;
}

bool CMapView::UserCopyElement(Element_Vec* pVecSrc /* = NULL*/)
{
	ClearCopy();
	FPos fPosCenter;
	if (pVecSrc == NULL)
	{
		pVecSrc = &m_vecSelectElement;
	}
	CString msg;
	if(!CheckUserCopy(*pVecSrc, msg))
	{
		MessageBox(msg);
		CStateMgr::Inst().DoEvent(eEVENT_CLEAR_AS);
		return false;
	}


	CopeyVecElement(m_vecCopyElement,*pVecSrc, eET_NPC|eET_OBJ|eET_TRAP);
	if (!GetVecElementCenter(fPosCenter, m_vecCopyElement, eET_ALL))
	{
		return false;
	}
	int bossIndex = 0;
	for (int i = m_vecCopyElement.size()-1; i>=0; --i) 
	{
		m_vecCopyElement[i]->fPos -= fPosCenter;
		if (CPlaceNpc::IsBoos(m_vecCopyElement[i]))
		{
			bossIndex = i;
		}
	}
	//将虚拟npc放到第一个,在粘贴的时候才能通过检测
	CElement* pTemp;
	if (bossIndex != 0)
	{
		pTemp = m_vecCopyElement[0];
		m_vecCopyElement[0] = m_vecCopyElement[bossIndex];
		m_vecCopyElement[bossIndex] = pTemp;
	}

	CStateMgr::Inst().DoEvent(eEVENT_CLEAR_COPY);
	return true;
}

void CMapView::CutElement()
{
	if (!UserCopyElement())
	{
		return;
	}
	
	DeleteAllSelectElement();
}

void CMapView::PasteElement()
{
	if (!IsMapOk() || m_vecCopyElement.size() == 0 || !IsCursorInView())
	{
		return;
	}
	CString msg;
	if (!CheckUserCopy(m_vecCopyElement, msg))
	{
		ClearCopy();
		MessageBox("无效的复制源.");
		CStateMgr::Inst().DoEvent(eEVENT_CLEAR_AS);
		return;
	}
	//如果是群组的话,全部都是同一群组成员
	bool isHaveBoss = false;
	int colonyId = 0;
	uint count = m_vecCopyElement.size();
	for (uint i = 0; i < count; ++i)
	{
		if (m_vecCopyElement[i]->eType == eET_NPC)
		{
			if (CPlaceNpc::IsBoos(m_vecCopyElement[i]))
			{
				if (i != 0)
				{

				}
				isHaveBoss = true;
				break;
			}
			colonyId = static_cast<PPlaceNpc>(m_vecCopyElement[i])->colonyId;
		}
		else
		{
			break;
		}
	}

	//如果有虚拟npc 则为新的群组
	if(isHaveBoss)
	{
		colonyId = g_pCache->GetNextColonyId();
		for (uint i = 0; i < count; ++i)
		{
			static_cast<PPlaceNpc>(m_vecCopyElement[i])->colonyId = colonyId;
		}
	}

	CStateMgr::Inst().DoEvent(eEVENT_PASTE_ELEMENT);
}

CElement* CMapView::PutElement(CElement* pElement, const FPos& gridPos)
{
	if (IsBlock(int(gridPos.x), int(gridPos.y)))
	{
		MessageBox("这里是障碍区, 不能摆放 东西");
		return NULL;
	}
	if (!CheckPos(gridPos, eET_ALL,pElement))
	{
		MessageBox("有两个对象靠的太近了");
		return NULL;
	}
	CElement* result = NULL;
	switch(pElement->eType)
	{
	case eET_NPC:
		return PutNpc(static_cast<PPlaceNpc>(pElement), gridPos);
	case eET_OBJ:
		return PutObj(static_cast<PPlaceObj>(pElement), gridPos);
	case eET_TRAP:
	    return PutTrap(static_cast<PPlaceTrap>(pElement), gridPos);
	default:
	    break;
	}
	return result;
}

CElement* CMapView::PutNpc(PPlaceNpc pNpc, const FPos& gridPos)
{
	CString msg;
	if (!PutNpcCheck(pNpc, gridPos, &msg))
	{
		MessageBox(msg);
		return NULL;
	}
	SetUpdateMark(eUES_NPC);
	return g_pCache->AddNpc(pNpc, gridPos.x, gridPos.y);
}

CElement* CMapView::PutObj(PPlaceObj pObj, const FPos& gridPos)
{
	CString msg;
	if (!PutObjCheck(pObj, gridPos, &msg))
	{
		MessageBox(msg);
		return NULL;
	}
	SetUpdateMark(eUES_OBJ);
	return g_pCache->AddObj(pObj, gridPos.x, gridPos.y);
	
}

CElement* CMapView::PutTrap(PPlaceTrap pTrap, const FPos& gridPos)
{
	CString msg;
	if (!PutTrapCheck(pTrap, gridPos, &msg))
	{
		MessageBox(msg);
		return NULL;
	}
	SetUpdateMark(eUES_TRAP);
	return g_pCache->AddTrap(pTrap, gridPos.x, gridPos.y);
}

bool CMapView::PutNpcCheck(PPlaceNpc pNpc, const FPos& gridPos, OUT CString* pMsg /*=NULL*/)
{
	if(g_pOriginalData->IsSingleNpc(pNpc->name))
	{	
		if (g_eModeType == eMT_DYNAMIC)
		{
			if (pMsg != NULL)
			{
				*pMsg = "动态文件中不能摆放 唯一 对象";
			}
			return false;
		}
		if (g_pCache->FindNpc(pNpc->name) != NULL)
		{
			if (pMsg != NULL)
			{
				*pMsg = pNpc->name + " 为唯一NPC 不能重复摆放";
			}
			return false;
		}
	}
	if (g_eModeType == eMT_PATH_EDIT && g_pCache->FindNpc(pNpc->name))
	{
		return false;
	}
	if(CPlaceNpc::IsServant(pNpc))
	{
		PPlaceNpc pBoss = g_pCache->FindGroupBoss(pNpc->colonyId);
		if ((pBoss->fPos - gridPos).Length() > COLONY_AREA )
		{
			if (pMsg != NULL)
			{
				*pMsg = "粘贴的群组成员离虚拟NPC过远";
			}
			return false;
		}
	}

	return true;
}

bool CMapView::PutObjCheck(PPlaceObj pObj, const FPos& gridPos, OUT CString* pMsg /*=NULL*/)
{
	if(g_pOriginalData->IsSingleObj(pObj->name))
	{

		if (g_eModeType == eMT_DYNAMIC)
		{
			if (pMsg != NULL)
			{
				*pMsg = "动态文件中不能摆放 唯一 对象";
			}
			return false;
		}
		if (g_pCache->FindObj(pObj->name) != NULL)
		{
			if (pMsg != NULL)
			{
				*pMsg = pObj->name + " 为唯一 OBJ 不能重复摆放";
			}
			return false;
		}
	}
	return true;
}

bool CMapView::PutTrapCheck(PPlaceTrap pTrap, const FPos& gridPos, OUT CString* pMsg /*=NULL*/)
{
	if(g_pOriginalData->IsSingleTrap(pTrap->name))
	{
		if (g_eModeType == eMT_DYNAMIC)
		{
			if (pMsg != NULL)
			{
				*pMsg = "动态文件中不能摆放 唯一 对象";
			}
			return false;
		}
		if (g_pCache->FindTrap(pTrap->name) != NULL)
		{
			if (pMsg != NULL)
			{
				*pMsg = pTrap->name + " 为唯一 TRAP 不能重复摆放";
			}
			return false;
		}
	}
	return true;
}

void CMapView::PutPasteElement()
{
	ClearSelectElement();
	Element_Vec vecPutElement; //记录已经摆放的Element 如果遇到摆放失败的,将删除这些 Element
	bool isPutSucceed = true;
	FPos gridPos;
	CElement* pElement;
	FPos curCursorPos ;
	GetCurCursorGridPos(curCursorPos);
	for (uint i = 0; i < m_vecCopyElement.size(); ++i) 
	{
		gridPos = curCursorPos + m_vecCopyElement[i]->fPos;
		pElement = PutElement(m_vecCopyElement[i], gridPos);
		if( pElement == NULL)
		{
			isPutSucceed = false;
			break;
		}
		vecPutElement.push_back(pElement);
	}
	if (!isPutSucceed)
	{
		for (int i = vecPutElement.size()-1; i>=0; --i)  //倒着删除
		{
			DeleteElement(vecPutElement[i]);
		}
		return;
	}
	else
	{
		for (int i = vecPutElement.size()-1; i>=0; --i) 
		{
			AddSelectElement(vecPutElement[i]);
		}
	}
	CStateMgr::Inst().DoEvent(eEVENT_PUT_PASTE_ELEMENT);
}

bool CMapView::IsPosNearLine(OUT float& dis ,const FPos& fPosLineStart, const FPos& fPosLineEnd, const FPos& fPosTest, float fRange) const
{
	Vector2 vecA = fPosLineEnd - fPosLineStart;

	Vector2 vecB = fPosTest - fPosLineStart;
	dis = abs(vecA^vecB) / vecA.Length();
	if (dis > fRange)
	{
		return false;
	}
	float rate =  ((vecA * vecB)/(vecA * vecA));//垂足位置 相对 vecA 的比率
	if (rate >= 0 && rate <= 1)
	{
		return true;
	}
	return false;

}

bool CMapView::CheckPos(const FPos& fPos, int mark, CElement* pExclude /* = NULL */)
{
	if (IsShowSate(eSS_IGNORE_BLOCK))
	{
		return true;
	}
	Element_Vec vec;
	FRect fRect(fPos.x - MIN_DIS, fPos.y -MIN_DIS,fPos.x + MIN_DIS, fPos.y + MIN_DIS);

	FindElement(vec, fRect, mark);
	if (vec.size() > 1)
	{
		return false;//目标点有人占了
	}
	else if (vec.size() == 1 && vec[0] != pExclude)
	{
		return false;	//目标点有人占了
	}
	return true;
}



void CMapView::MoveAllSelectElement(const FPos& offset)
{
	FPos fPos;
	for (uint i = 0; i < m_vecSelectElement.size(); i++)
	{
		fPos = m_vecSelectElement[i]->fPos + offset;
		if (!CheckPos(fPos, eET_ALL, m_vecSelectElement[i]) || IsBlock(int(fPos.x), int(fPos.y)))
		{
			continue;
		}
		MoveElement(m_vecSelectElement[i], fPos);
	}
}

void CMapView::MoveAllSelectElementToGridMiddle()
{
	FPos fPos;
	for (uint i = 0; i < m_vecSelectElement.size(); i++)
	{
		fPos = m_vecSelectElement[i]->fPos;
		fPos.x = float ((int)(fPos.x)) + 0.5f ;
		fPos.y = float ((int)(fPos.y)) + 0.5f ;
		if (!CheckPos(fPos, eET_ALL, m_vecSelectElement[i]) || IsBlock(int(fPos.x), int(fPos.y)))
		{
			continue;
		}
		MoveElement(m_vecSelectElement[i], fPos);
	}
}

void CMapView::MoveElement(CElement* pElement, const FPos& fPos)
{
	switch(pElement->eType)
	{
	case eET_NPC:
		g_pCache->MoveNpc(static_cast<PPlaceNpc>(pElement),fPos);
		break;
	case eET_OBJ:
		g_pCache->MoveObj(static_cast<PPlaceObj>(pElement),fPos);
		break;
	case eET_TRAP:
		g_pCache->MoveTrap(static_cast<PPlaceTrap>(pElement),fPos);
		break;
	case eET_PATH_KEY_POS:
		g_pCache->MovePathKeyPos(static_cast<PPathKeyPos>(pElement),fPos);
		break;
	}
}


void CMapView::OnMoveElement()
{
	if (m_pFocusElement == NULL)
	{
		return;
	}

	EElementType type = m_pFocusElement->eType;
	FPos fPos;
	GetCurCursorGridPos(fPos);
	if (type == eET_NPC || type == eET_OBJ || type == eET_TRAP || type == eET_PATH_KEY_POS) //不重叠判断
	{
		if (!CheckPos(fPos, eET_ALL, m_pFocusElement) || IsBlock(int(fPos.x), int(fPos.y)))
		{
			return;
		}
	}

	if (m_vecSelectElement.size() > 1 && !CStateMgr::IsAssistState(eAS_SHIFT))
	{

		MoveAllSelectElement(fPos - m_pFocusElement->fPos);
		return;
	}


	switch(type)
	{
	case eET_NPC:
		if(CStateMgr::IsAssistState(eAS_SHIFT)&&CStateMgr::IsState(eMS_MOVE_ELEMENT)) //拖出路径点
		{
			PPlaceNpc pNpc = static_cast<PPlaceNpc>(m_pFocusElement);
			if (!CheckInsertPathKeyPos(pNpc, fPos))
			{
				return;
			}
			CPlaceNpc::Path_Iter iter = pNpc->pathList.begin();
			PPathKeyPos pKeyPos;
			if (iter ==  pNpc->pathList.end())
			{
				pKeyPos = pNpc->InsetPathKeyPos(NULL,fPos);
			}
			else
			{
				pKeyPos = pNpc->InsetPathKeyPos(*iter,fPos);
			}
			if(pKeyPos != NULL)
			{
				SetMyFocus(pKeyPos);
				CStateMgr::Inst().DoEvent(eEVENT_NEW_PATH_KEY_POS);
			}
		}
		else
		{
				MoveElement(m_pFocusElement, fPos);
		}
		break;
	case eET_OBJ:
		MoveElement(m_pFocusElement, fPos);
		break;
	case eET_TRAP:
		MoveElement(m_pFocusElement, fPos);
	    break;
	case eET_PATH_KEY_POS:
		if(CStateMgr::IsAssistState(eAS_SHIFT) && CStateMgr::IsState(eMS_MOVE_ELEMENT)) //拖出路径点
		{
			PPathKeyPos pKeyPos = static_cast<PPathKeyPos>(m_pFocusElement);
			if (!CheckInsertPathKeyPos(pKeyPos, fPos))
			{
				return;
			}
			PPathKeyPos pResultKeyPos = pKeyPos->pOwner->InsetPathKeyPos( pKeyPos->pOwner->FindNextKeyPos(pKeyPos),fPos);
			if(pResultKeyPos != NULL)
			{
				SetMyFocus(pResultKeyPos);
				CStateMgr::Inst().DoEvent(eEVENT_NEW_PATH_KEY_POS);
			}
		}
		else
		{
			MoveElement(m_pFocusElement, fPos);
		}
	    break;
	case eET_ARROW:
		{
			CArrow* pArrow = static_cast<CArrow*>(m_pFocusElement);
			int dir = GetGameDir(fPos - pArrow->pOwner->fPos);
			pArrow->SetDir(dir);
		}	
		break;
	case eET_PATH_LINE:
		if(CStateMgr::IsAssistState(eAS_SHIFT))
		{
			CPathLine* pPathLine = static_cast<CPathLine*>(m_pFocusElement);
			if (!CheckInsertPathKeyPos(pPathLine, fPos))
			{
				return;
			}
			PPathKeyPos pKeyPos= g_pCache->InsetPathKeyPos(pPathLine, fPos);
			if(pKeyPos != NULL)
			{
				SetMyFocus(pKeyPos);
				CStateMgr::Inst().DoEvent(eEVENT_NEW_PATH_KEY_POS);
			}
		}
		break;
	default:
	    break;
	}
}

void CMapView::RotateElement()
{
	if (CStateMgr::IsState(eMS_ROTATE_ELEMENT))
	{
		CStateMgr::Inst().DoEvent(eEVENT_ROTATE_ELEMENT_END);
		return;
	}

	if (! (CStateMgr::IsState(eMS_NORMAL) && GetVecElementRect(m_fRectRotate, m_vecSelectElement, eET_ALL)) )
	{	
		return;
	}
	MoveMapToCenter(m_fRectRotate.GetCenter());

	CStateMgr::Inst().DoEvent(eEVENT_ROTATE_ELEMENT);
}

bool CMapView::IsCursorOnRotateRound()
{
	Pos posCursor;
	Pos posCenter;
	GetCurCursorViewPos(posCursor);
	GridPosToViewPos(posCenter, m_fRectRotate.GetCenter());
	int fRadius = int (GetViewDistance((m_fRectRotate.left_top - m_fRectRotate.right_bottom).Length()) / 2) + ROTATE_ROUND_CENTER_RADIUS + 10;
	int iDis = (posCursor - posCenter).Length();
	return abs(iDis - fRadius) < (SELECT_PRECISION + ROTATE_ROUND_SIZE);
}

void CMapView::BeginRotateElement()
{
	ClearRotateBasic();
	CopeyVecElement(m_vecRotateBasic, m_vecSelectElement, eET_ALL);
	GetCurCursorGridPos(m_fPosRotateStart);
	CStateMgr::Inst().DoEvent(eEVENT_ROTATE_ELEMENT_BEGIN);
}

void CMapView::UpdateRotateElement()
{
	Ast(m_vecRotateBasic.size() == m_vecSelectElement.size());
	FPos curPos;
	FPos centerPos;
	m_fRectRotate.GetCenter(centerPos);
	GetCurCursorGridPos(curPos);
	Vector2 vStart = m_fPosRotateStart - centerPos;
	Vector2 vEnd = curPos - centerPos;
	double radian = acos( (vStart*vEnd)/(vStart.Length()*vEnd.Length()) );
	bool isClockwise = (vStart^vEnd) < 0;
	int gameDir = isClockwise ? (int)(radian / DIR_TO_RADIAN_RATE): -(int)(radian / DIR_TO_RADIAN_RATE);

	CElement* pBasicElement;
	CElement* pSelectElement;
	Vector2 vRadius;
	for (int i = m_vecRotateBasic.size()-1; i>=0; i--) 
	{
		pBasicElement = m_vecRotateBasic[i];
		pSelectElement = m_vecSelectElement[i];
		vRadius = pBasicElement->fPos - centerPos;
		
		pSelectElement->SetDir(pBasicElement->direction + gameDir);
		MoveElement(pSelectElement,centerPos + vRadius.Rotate(radian, isClockwise));
	}
}

void CMapView::DeleteElement(CElement* pElement)
{

	switch(pElement->eType)
	{
	case eET_PATH_KEY_POS:
		{
			PPathKeyPos pKeyPos = static_cast<PPathKeyPos>(pElement);
			pKeyPos->pOwner->DeletePathKeyPos(pKeyPos);
		}
		SetUpdateMark(eUES_PATH_KEY_POS);
		break;
	case eET_NPC:
		g_pCache->DeleteNpc(static_cast<PPlaceNpc>(pElement));
		SetUpdateMark(eUES_NPC);
		break;
	case eET_OBJ:
		g_pCache->DeleteObj(static_cast<PPlaceObj>(pElement));
		SetUpdateMark(eUES_OBJ);
		break;
	case eET_TRAP:
		g_pCache->DeleteTrap(static_cast<PPlaceTrap>(pElement));
		SetUpdateMark(eUES_TRAP);
		break;
	default:
	    return;
	}
}

bool CMapView::DeleteElemetVec(Element_Vec& vecElement)
{
	Element_Vec vecPathKeyPos; //优先删除
	Element_Vec	vecOther;
	Element_Vec vecBoss; //最后删除

	int count = vecElement.size();
	//筛选删除分类,
	CElement* pElement;
	PPlaceNpc pNpc;
	bool isAgree =false;
	for (int i = 0; i < count; ++i)
	{
		pElement = vecElement[i];
		if (pElement->eType == eET_NPC)
		{
			pNpc = static_cast<PPlaceNpc>(pElement);
			if (pNpc->colonyType == CEnumMgr::Inst().GetColonyTypeID("BOSS"))
			{
				if (!isAgree)
				{
					if(IDYES != MessageBox("要删除的对象中有虚拟NPC,\n确定要删除整个群组吗?", "提示", MB_YESNO))
					{
						return false;
					}
				}
				vecBoss.push_back(pElement);
			}
			else
			{
				vecOther.push_back(pElement);
			}
		}
		else if (pElement->eType == eET_PATH_KEY_POS)
		{
			vecPathKeyPos.push_back(pElement);
		}
		else
		{
			vecOther.push_back(pElement);
		}
	}
	SetMyFocus(NULL);
	
	for (int i = vecPathKeyPos.size()-1; i >= 0; --i)
	{
		DeleteElement(vecPathKeyPos[i]);
	}

	for (int i = vecOther.size()-1; i >= 0; --i)
	{
		DeleteElement(vecOther[i]);
	}
	
	Element_Vec vecColonyMember;
	for (int i = vecBoss.size()-1; i >= 0; --i)
	{
		pNpc = static_cast<PPlaceNpc>(vecBoss[i]);
		vecColonyMember.clear();
		g_pCache->GetColonyMember(vecColonyMember, pNpc->colonyId);
		for (int j = vecColonyMember.size()-1; j >= 0; --j)
		{
			DeleteElement(vecColonyMember[j]);
		}
	}
	vecElement.clear();
	return true;
}

bool CMapView::DeleteAllSelectElement()
{
	SetMyFocus(NULL);
	return DeleteElemetVec(m_vecSelectElement);
}

void CMapView::SetShowState(EShowState eShowState, bool isShow)
{
	m_bShowStates[eShowState] = isShow;
	if(!isShow)//隐藏某些显示时候,从 选择vec中删除此类对象
	{
		switch(eShowState)
		{
		case eSS_NPC:
			RemoveSelectElement(eET_NPC);
			RemoveSelectElement(eET_PATH_KEY_POS);
			break;
		case eSS_OBJ:
			RemoveSelectElement(eET_OBJ);
			break;
		case eSS_TRAP:
			RemoveSelectElement(eET_TRAP);
		    break;
		case eSS_PATH:
			RemoveSelectElement(eET_PATH_KEY_POS);
		    break;
		default:
		    break;
		}
	}
	if (eShowState == eSS_AREA)
	{
		if (isShow)
		{
			CStateMgr::Inst().DoEvent(eEVENT_BACK_NORMAL);
			CStateMgr::Inst().DoEvent(eEVENT_BEGIN_EDIT_AREA);
		}
		else
		{
			CStateMgr::Inst().DoEvent(eEVENT_BEGIN_EDIT_AREA);
		}
	}
}

void CMapView::SetAreaImageAlpha(int alpha)
{
	if (alpha < 0)
	{
		alpha = 0;
	}
	else if(alpha > 255)
	{
		alpha = 255;
	}
	m_AlphaValue = alpha;
}


void CMapView::SetAreaImageColor(float centerX, float centerY,  int size,int color)
{
	centerY = m_nMapGridY - centerY;
	int leftTop_X = (int)(centerX - (size - 1)/2);
	int leftTop_Y = (int)(centerY - (size - 1)/2);
	int xStart, yStart, xEnd, yEnd;

	xStart = leftTop_X < 0 ? 0 : leftTop_X;
	yStart = leftTop_Y < 0 ? 0 : leftTop_Y;
	xEnd = leftTop_X + size > m_nMapGridX ? m_nMapGridX-1: leftTop_X + size;
	yEnd = leftTop_Y+ size > m_nMapGridY ? m_nMapGridY-1: leftTop_Y + size;
	int x,y;
	byte* pColor = (byte*)m_ImageArea.GetBits();
	int pitch  = m_ImageArea.GetPitch();

	byte* pTemp;
	if (color != -1) //画
	{
		if(g_eModeType != eMT_AREA_FB)
		{
			for(y = yStart; y < yEnd; y++)
			{
				for(x = xStart; x < xEnd; x++)
				{
					pTemp = pColor + pitch * y + x*4;
					if (!IsOverArea && *(pTemp +3) == 0xff)
					{
						continue;
					}
					*pTemp = GetBValue(color);
					*(pTemp +1) = GetGValue(color);
					*(pTemp +2) = GetRValue(color);
					*(pTemp +3) = 0xff;
					//OnAddArea(x,y);
				}
			}
		}
		else
		{
			AddBarrierObj(xStart, yStart);
			for (int i = 0; i < 8; i += 2)
			{
				if (IsBarrierObj(xStart + OFFSET_8DIR[i][0],m_nMapGridY - 1 - yStart - OFFSET_8DIR[i][1]) ||
					IsBlock(xStart + OFFSET_8DIR[i][0],m_nMapGridY - 1 - yStart - OFFSET_8DIR[i][1]))
				{
					int offset1 = i < 7 ? i + 1 : i - 7;
					int offset2 = i > 1 ? i - 1 : i + 7;
					int x1 = xStart + OFFSET_8DIR[offset1][0];
					int y1 = yStart + OFFSET_8DIR[offset1][1];
					int x2 = xStart + OFFSET_8DIR[offset2][0];
					int y2 = yStart + OFFSET_8DIR[offset2][1];
					if (!IsBarrierObj(x1,m_nMapGridY - 1 - y1)
						&&!IsBarrierObj(x2,m_nMapGridY - 1 - y2)
						&&!IsBlock(x1, m_nMapGridY - 1 -y1)
						&&!IsBlock(x2, m_nMapGridY - 1 -y2))
					{
						AddBarrierObj(x1,y1);
					}
				}
			}
		}
	}
	else//取消
	{
		for(y = yStart; y < yEnd; y++)
		{
			for(x = xStart; x < xEnd; x++)
			{
				pTemp = pColor + pitch * y + x*4;
				*pTemp = 0;
				*(pTemp +1) = 0;
				*(pTemp +2) = 0;
				*(pTemp +3) = 0;
				//OnRemoveArea(x, y);
			}
		}
	}
}

void CMapView::AddBarrierObj(const Pos& gridPos)
{
	int x,y;
	x = gridPos.x;
	y = m_nMapGridY - gridPos.y - 1;
	AddBarrierObj(x, y);
}

void CMapView::AddBarrierObj(int x, int y)
{
	if (x < 0 || x >= m_nMapGridX || y < 0 || y >= m_nMapGridY || IsBlock(x, m_nMapGridY - y - 1))
	{
		return;
	}
	byte* pColor = (byte*)m_ImageArea.GetBits();
	int pitch  = m_ImageArea.GetPitch();
	byte* pTemp;
	pTemp = pColor + pitch * y + x*4;
	*pTemp = GetBValue(BARRIER_OBJ_IMAGE_COLOR);
	*(pTemp +1) = GetGValue(BARRIER_OBJ_IMAGE_COLOR);
	*(pTemp +2) = GetRValue(BARRIER_OBJ_IMAGE_COLOR);
	*(pTemp +3) = 0xff;
}

bool CMapView::IsBarrierObj(int gridX, int gridY)
{
	int x,y;
	x = gridX;
	y = m_nMapGridY - gridY - 1;
	if (x < 0 || x >= m_nMapGridX || y < 0 || y >= m_nMapGridY)
	{
		return true;
	}
	byte* pColor = (byte*)m_ImageArea.GetBits();
	int pitch  = m_ImageArea.GetPitch();
	byte* pTemp;
	pTemp = pColor + pitch * y + x*4;

	if(*(pTemp +3) == 0xff)
	{
		return true;
	}
	return false;
}



inline bool CMapView::IsPosDraw(int x, int y)
{
	byte* pColor = (byte*)m_ImageArea.GetBits();
	int pitch  = m_ImageArea.GetPitch();
	if (x < 0 || x >= m_nMapGridX || y < 0 || y >= m_nMapGridY)
	{
		return true;
	}
	return *(pColor + pitch * y + x*4 + 3) == 0xff;
}

void CMapView::EmptyFillRecord()
{
	if(m_pFillRecord != NULL)
	{
		int x, y;
		for(x = 0; x < m_nMapGridX; x++)
		{
			for(y = 0; y < m_nMapGridY; y++)
			{
				m_pFillRecord[y * m_nMapGridX + x] = false;
			}
		}
	}
}

void CMapView::FillArea(int gridX, int gridY, int color)
{
	int x,y;
	x = gridX;
	y = m_nMapGridY - gridY - 1;
	std::vector<Pos> vStep;
	Pos startPos(x, y);
	if(IsPosDraw(startPos.x, startPos.y))
	{
		return;
	}
	EmptyFillRecord();
	vStep.push_back(startPos);
	m_pFillRecord[startPos.y * m_nMapGridX + startPos.x] = true;
	Pos posTemp;
	int tempX,tempY;
	while(!vStep.empty())
	{
		posTemp = vStep.back();
		vStep.pop_back();
		for (int i = 0; i < 4; ++i)
		{
			tempX = posTemp.x + OFFSET_4DIR[i][0];
			tempY = posTemp.y + OFFSET_4DIR[i][1];
			if (!IsPosDraw(tempX, tempY)
				&& !m_pFillRecord[m_nMapGridX * tempY + tempX])
			{
				vStep.push_back(Pos(tempX,tempY));
				m_pFillRecord[m_nMapGridX * tempY + tempX] = true;
			}
		}
	}
	
	int c, r;
	byte* pColor = (byte*)m_ImageArea.GetBits();
	int pitch  = m_ImageArea.GetPitch();
	byte* pTemp;
	for(c = 0; c < m_nMapGridX; c++)
	{
		for(r = 0; r < m_nMapGridY; r++)
		{
			if(m_pFillRecord[r * m_nMapGridX + c])
			{
				pTemp = pColor + pitch * r + c*4;
				*pTemp = GetBValue(color);
				*(pTemp +1) = GetGValue(color);
				*(pTemp +2) = GetRValue(color);
				*(pTemp +3) = 0xff;
				//OnAddArea(c, r);
			}
		}
	}
}

void CMapView::SetContourColor(int gridX, int gridY, int color)
{
	int x,y;
	x = gridX;
	y = m_nMapGridY - gridY - 1;
	if (x < 0 || x >= m_nMapGridX || y < 0 || y >= m_nMapGridY)
	{
		return;
	}
	byte* pColor = (byte*)m_ImageArea.GetBits();
	int pitch  = m_ImageArea.GetPitch();
	byte* pTemp;
	pTemp = pColor + pitch * y + x*4;
	if (color == -1)//无
	{
		*pTemp = 0;
		*(pTemp +1) =0;
		*(pTemp +2) = 0;
		*(pTemp +3) = 0;
	}
	else
	{
		*pTemp = GetBValue(color);
		*(pTemp +1) = GetGValue(color);
		*(pTemp +2) = GetRValue(color);
		*(pTemp +3) = 0xff;
	}
	m_pAreaContour[y*m_nMapGridX + x] = true;
}

void CMapView::DrawAreaLine(int gridX, int gridY1, int gridY2, int color)
{
	if (color == -1)//无
	{
		return;
	}
	int x,y1,y2;
	x = gridX;
	y2 = m_nMapGridY - gridY1 - 1;
	y1 = m_nMapGridY - gridY2 - 1;

	byte* pColor = (byte*)m_ImageArea.GetBits();
	int pitch  = m_ImageArea.GetPitch();
	byte* pTemp;
	for (int i = y1; i <= y2; i ++)
	{
		pTemp = pColor + pitch * i + x*4;
		*pTemp = GetBValue(color);
		*(pTemp +1) = GetGValue(color);
		*(pTemp +2) = GetRValue(color);
		*(pTemp +3) = 0xff;
	}
}

void CMapView::SetBlockImageColor(int gridX, int gridY, int color)
{
	int x,y;
	x = gridX;
	y = m_nMapGridY - gridY - 1;
	if (x < 0 || x >= m_nMapGridX || y < 0 || y >= m_nMapGridY)
	{
		return;
	}
	byte* pColor = (byte*)m_ImageBlock.GetBits();
	int pitch  = m_ImageBlock.GetPitch();
	byte* pTemp;
	pTemp = pColor + pitch * y + x*4;
	*pTemp = GetBValue(color);
	*(pTemp +1) = GetGValue(color);
	*(pTemp +2) = GetRValue(color);
	*(pTemp +3) = 0xff;
}

bool CMapView::IsBlock(int gridX, int gridY)
{
	if(g_pMainDlg->IsLeapCheck || IsShowSate(eSS_IGNORE_BLOCK))
	{
		return false;
	}
	int x,y;
	x = gridX;
	y = m_nMapGridY - gridY - 1;
	if (x < 0 || x >= m_nMapGridX || y < 0 || y >= m_nMapGridY)
	{
		return true;
	}
	byte* pColor = (byte*)m_ImageBlock.GetBits();
	int pitch  = m_ImageBlock.GetPitch();
	byte* pTemp;
	pTemp = pColor + pitch * y + x*4;

	if(* pTemp == GetBValue(BLOCK_COLOR_LV3) 
		&& *(pTemp +1) == GetGValue(BLOCK_COLOR_LV3) 
		&& *(pTemp +2) == GetRValue(BLOCK_COLOR_LV3)
		&& *(pTemp +3) != 0)
	{
		return true;
	}
	if(* pTemp == GetBValue(BLOCK_COLOR_LV2) 
		&& *(pTemp +1) == GetGValue(BLOCK_COLOR_LV2) 
		&& *(pTemp +2) == GetRValue(BLOCK_COLOR_LV2)
		&& *(pTemp +3) != 0)
	{
		return true;
	}
	return false;
}

bool CMapView::IsBlock(const FPos& gridFPos)
{
	return IsBlock(int (gridFPos.x), int (gridFPos.y));
}

void CMapView::ReplaceColor(int oldColor, int newColor)
{
	byte* pColor = (byte*)m_ImageArea.GetBits();
	int pitch  = m_ImageArea.GetPitch();
	byte* pTemp;
	int ByteWidth = m_nMapGridX * 4;
	int x, y;
	for(y = 0; y < m_nMapGridY; y++)
	{
		for(x = 0; x < ByteWidth; x+=4)
		{
			pTemp = pColor + pitch * y + x;
			if(*(pTemp +3) != 0xff)//未填的颜色
			{
				continue;
			}
			if (*pTemp == GetBValue(oldColor) 
				&& *(pTemp +1) == GetGValue(oldColor)
				&& *(pTemp +2) == GetRValue(oldColor))
			{
				*pTemp = GetBValue(newColor);
				*(pTemp +1) = GetGValue(newColor);
				*(pTemp +2) = GetRValue(newColor);
			}
		}
	}
}

void CMapView::DelColor(int color)
{
	byte* pColor = (byte*)m_ImageArea.GetBits();
	int pitch  = m_ImageArea.GetPitch();
	byte* pTemp;
	int ByteWidth = m_nMapGridX * 4;
	int x, y;
	for(y = 0; y < m_nMapGridY; y++)
	{
		for(x = 0; x < ByteWidth; x+=4)
		{
			pTemp = pColor + pitch * y + x;
			if(*(pTemp +3) != 0xff)//未填的颜色
			{
				continue;
			}
			if (*pTemp == GetBValue(color) 
				&& *(pTemp +1) == GetGValue(color)
				&& *(pTemp +2) == GetRValue(color))
			{
				*pTemp = 0;
				*(pTemp +1) = 0;
				*(pTemp +2) = 0;
				*(pTemp +3) = 0;
			}
		}
	}
}

void CMapView::ClearArea()
{
	if (m_ImageArea.IsNull() || m_pAreaContour == NULL)
	{
		cout << "CMapView::ClearArea() 失败 " << endl;
		return;
	}
	int x,y;
	byte* pColor = (byte*)m_ImageArea.GetBits();
	int pitch  = m_ImageArea.GetPitch();
	for(y = 0; y < m_nMapGridY; y++)
	{
		for(x = 0; x < m_nMapGridX; x++)
		{
			*((int*)(pColor + pitch * y) + x) = 0;
		}
	}
	ClearContour();
}

int CMapView::GetAreaColor(int gridX, int gridY)
{
	int x,y;
	x = gridX;
	y = m_nMapGridY - gridY - 1;
	if (x < 0 || x >= m_nMapGridX || y < 0 || y >= m_nMapGridY)
	{
		return -1;
	}
	byte* pColor = (byte*)m_ImageArea.GetBits();
	int pitch  = m_ImageArea.GetPitch();
	byte* pTemp;
	pTemp = pColor + pitch * y + x*4;
	if (*(pTemp +3) != 0xff)
	{
		return -1;
	}
	else
	{
		return RGB(*(pTemp +2),*(pTemp +1),*pTemp);
	}
}

//八方向
bool CMapView::IsInsidePos(int x, int y)
{
	int* pColor = (int*)m_ImageArea.GetBits();
	int pitch  = m_ImageArea.GetPitch()/sizeof(int);
	int color = *(pColor + pitch * y + x);
	if ((x == 0 || x == m_nMapGridX -1 || y == 0 || y == m_nMapGridY -1) && color != 0 )
	{
		return false;
	}
	int tx, ty;
	for (int i = 0; i< 8; ++i)
	{
		tx = x + OFFSET_8DIR[i][0];
		ty = y + OFFSET_8DIR[i][1];
		if (tx < 0 || tx >= m_nMapGridX || ty < 0 || ty >= m_nMapGridY)
		{
			continue;
		}
		if ( *(pColor + pitch * ty + tx) != color)
		{
			return false;
		}
	}
	return true;
}

void CMapView::ComputeContour()
{
	if (m_ImageArea.IsNull() || m_pAreaContour == NULL)
	{
		return;
	}

	memset(m_pAreaContour, true, sizeof(bool) * m_nMapGridX * m_nMapGridY);
	int x, y;
	for(y = 0; y < m_nMapGridY; y++)
	{
		for(x = 0; x < m_nMapGridX; x++)
		{
			if (IsInsidePos(x, y))
			{
				m_pAreaContour[y * m_nMapGridX + x] = false;
			}
		}
	}
	
	//byte* pColor = (byte*)m_ImageArea.GetBits();
	//int pitch  = m_ImageArea.GetPitch();
	//for(y = 0; y < m_nMapGridY; y++)
	//{
	//	for(x = 0; x < m_nMapGridX; x++)
	//	{
	//		if (!m_pAreaContour[y * m_nMapGridX + x])
	//		{
	//			*((int*)(pColor + pitch * y) + x) = 0;
	//		}
	//	}
	//}
}



void CMapView::FillContour()
{
	if (m_ImageArea.IsNull() || m_pAreaContour == NULL)
	{
		return;
	}
	byte* pColor = (byte*)m_ImageArea.GetBits();
	int pitch  = m_ImageArea.GetPitch();
	int x, y;
	int curColor;
	for(y = 0; y < m_nMapGridY; y++)
	{
		curColor = 0;//每行的第一个先假设为无
		for(x = 0; x < m_nMapGridX; x++)
		{
			if (!m_pAreaContour[y * m_nMapGridX + x])
			{
				*((int*)(pColor + pitch * y) + x) = curColor;
			}
			else
			{
				curColor = *((int*)(pColor + pitch * y) + x);
			}
		}
	}
}

bool CMapView::IsContourPos(int gridX, int gridY)
{
	int x,y;
	x = gridX;
	y = m_nMapGridY - gridY - 1;
	return m_pAreaContour[y*m_nMapGridX + x];
}


bool CMapView::IsAreaEdge(int ix, int iy)
{
	int x, y;
	for (int i= 0; i < 8; ++i)
	{
		x = ix + OFFSET_8DIR[i][0];
		y = iy + OFFSET_8DIR[i][1];
		if (x < 0 || x >= m_nMapGridX || y < 0 || y >= m_nMapGridY)
		{
			continue;
		}
		if (IsPosDraw(x, y))
		{
			return true;
		}
	}
	return false;
}
void CMapView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CStatic::OnLButtonDblClk(nFlags, point);
}

void CMapView::PauseRender()
{
	m_iPauseTimes += PAUSE_TIMES;
}

void CMapView::Measure()
{
	if (CStateMgr::IsState(eMS_MEASURE))
	{
		CStateMgr::Inst().DoEvent(eEVENT_MEASURE_END);
		return;
	}

	if ( CStateMgr::IsState(eMS_NORMAL) )
	{
		ClearMeasurePos();
		CStateMgr::Inst().DoEvent(eEVENT_MEASURE_BEGIN);
	}
}

void CMapView::AddMeasurePos(const FPos& fPos)
{
	m_vecMeasurePos.push_back(fPos);
}

void CMapView::ClearMeasurePos()
{
	m_vecMeasurePos.clear();
}

bool CMapView::CheckInsertPathKeyPos(CPathLine* pPathLine, const FPos& fPos)
{
	if (pPathLine->pBackKeyPos == NULL)
	{
		return false;
	}
	if (pPathLine->pFrontKeyPos)
	{
		if (!Line(pPathLine->pFrontKeyPos->fPos, fPos))
		{
			return false;
		}
	}
	else if (!Line(pPathLine->pBackKeyPos->pOwner->fPos, fPos))
	{
		return false;
	}
	if (!Line(pPathLine->pBackKeyPos->fPos, fPos) )
	{
		return false;
	}
	return true;
}

bool CMapView::CheckInsertPathKeyPos(PPlaceNpc pNpc, const FPos& fPos)
{
	if (!Line(pNpc->fPos, fPos))
	{
		return false;
	}
	CPlaceNpc::Path_Iter iter = pNpc->pathList.begin();
	if (iter !=  pNpc->pathList.end() && !Line((*iter)->fPos, fPos))
	{
		return false;
	}
	return true;
}

bool CMapView::CheckInsertPathKeyPos(PPathKeyPos pPathKeyPos, const FPos& fPos)
{
	PPathKeyPos pPrePos = NULL;
	PPlaceNpc pNpc = pPathKeyPos ->pOwner;
	CPlaceNpc::Path_Iter iter = pNpc->pathList.begin();
	while (iter != pNpc->pathList.end())
	{
		if (*iter == pPathKeyPos)
		{
			break;
		}
		else
		{
			pPrePos = *iter;
		}
		++iter;
	}
	if (pPrePos == NULL)
	{
		if ( !Line(pNpc->fPos, fPos))
		{
			return false;
		}
	}
	else if (!Line(pPrePos->fPos, fPos) )
	{
		return false;
	}
	if (!Line(pPathKeyPos->fPos, fPos) )
	{
		return false;
	}
	return true;
}

bool CMapView::CheckMovePathKeyPos(PPathKeyPos pPathKeyPos, const FPos& fPos)
{
	PPathKeyPos pPrePos = NULL;
	PPlaceNpc pNpc = pPathKeyPos->pOwner;
	CPlaceNpc::Path_Iter iter = pNpc->pathList.begin();
	while (iter != pNpc->pathList.end())
	{
		if (*iter == pPathKeyPos)
		{
			++iter;
			break;
		}
		else
		{
			pPrePos = *iter;
		}
		++iter;
	}
	if (pPrePos == NULL)
	{
		if (!Line(pNpc->fPos, fPos))
		{
			return false;
		}
	}
	else if (!Line(pPrePos->fPos, fPos) )
	{
		return false;
	}
	if (iter != pNpc->pathList.end() && !Line((*iter)->fPos, fPos) )
	{
		return false;
	}
	return true;
}

bool CMapView::CheckMoveNpc(PPlaceNpc pNpc, const FPos& fPos)
{
	CPlaceNpc::Path_Iter iter = pNpc->pathList.begin();
	if (iter != pNpc->pathList.end())
	{
		if (!Line((*iter)->fPos, fPos))
		{
			return false;
		}
	}
	return true;
}

bool CMapView::Line(const FPos& srcPos, const FPos& destPos)
{
	struct DrawLine:public ILineAction 
	{
		DrawLine(CMapView* pMapView):m_pMapView(pMapView){}
		CMapView* m_pMapView;
		bool Do( int32 x, int32 y )
		{
			return !m_pMapView->IsBlock( x >> GRID_PRECISION, y >> GRID_PRECISION );
		}
	};
	DrawLine drawLine(this);
	return _Line(int(srcPos.x * (1 << GRID_PRECISION)),int(srcPos.y * (1 << GRID_PRECISION)), int(destPos.x * (1 << GRID_PRECISION)),int(destPos.y * (1 << GRID_PRECISION)), drawLine);
}
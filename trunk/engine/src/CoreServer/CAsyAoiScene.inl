#include "CAsyAoiScene.h" 
#include "CAsyAoiGrid.h" 
#include "CRectangle.h" 

template<typename FunType, FunType Func> 
void CAsyAoiScene::QueryObject(void *pArg,const CIRect& Rect)const
{
	int32 nXOfs = Rect.right>Rect.left ? 1 : -1;
	int32 nYOfs = Rect.bottom>Rect.top ? 1 : -1;

	//int32 nRealWidth = m_uViewAoiRadius*2 + m_uGridWidth;
	int32 nRealWidth = m_uRealWidth;

	CAsyAoiGrid* pLineBeginGrid		=	GetGrid( Rect.left , Rect.top );
	CAsyAoiGrid* pEndLineBeginGrid	=	pLineBeginGrid + ( Rect.bottom - Rect.top ) * nRealWidth;

	if ( nYOfs < 0 )
		nRealWidth = - nRealWidth;

	for( ; pLineBeginGrid != pEndLineBeginGrid; pLineBeginGrid += nRealWidth )
	{
		CAsyAoiGrid* pGrid		=	pLineBeginGrid;
		CAsyAoiGrid* pEndGrid	=	pGrid + ( Rect.right - Rect.left );

		list<CAsyAoiViewObj*>::iterator it, end;

		for( ; pGrid!=pEndGrid; pGrid+=nXOfs )
		{
			it = pGrid->m_lstObject.begin();
			end = pGrid->m_lstObject.end();
			while (it != end)
			{
				Func(pArg, (*it));
				it++;
			}
	
		}
	}
}

template <typename FuncType, FuncType Func>
void CAsyAoiScene::QueryObject(void *pArg,const CPos& GridPos,float fGridRadius)const
{
	CIRect Rect;

	uint32 uRadius=uint32( ceil( fGridRadius ) );

	Rect.left	=	GridPos.x	-	uRadius;
	Rect.right	=	GridPos.x	+	uRadius	+	1;
	Rect.top	=	GridPos.y	-	uRadius;
	Rect.bottom	=	GridPos.y	+	uRadius	-	1;

	CPos OtGridPos;

	int32 nRealWidth = m_uViewAoiRadius*2 + m_uGridWidth;

	CAsyAoiGrid* pLineBeginGrid		=	GetGrid( Rect.left , Rect.top );

	for( int32 nY = Rect.top ; nY < Rect.bottom; nY++,pLineBeginGrid += nRealWidth )
	{
		CAsyAoiGrid* pGrid		=	pLineBeginGrid;

		for( int32 nX = Rect.left; nX < Rect.right ; nX++,pGrid++ )
		{
			list<CAsyAoiViewObj*>& lstObject=pGrid->m_lstObject;

			OtGridPos.x = nX;
			OtGridPos.y = nY;

			list<CAsyAoiViewObj*>::iterator it, end;
			it = lstObject.begin();
			end = lstObject.end();
			while (it != end)
			{
				if( fGridRadius< GridPos.Dist(OtGridPos) )
					continue;
				Func(pArg, (*it));
				it++;
			}
		}
	}
}

template <typename FuncType, FuncType Func>
void CAsyAoiScene::QueryObject(void *pEnterArg, void *pLeaveArg,
							const CPos& OldGridPos,
							const CPos& NewGridPos)const
{
	int32 nXOfs	= abs( NewGridPos.x - OldGridPos.x );
	int32 nYOfs	= abs( NewGridPos.y - OldGridPos.y );

	//vector<CCoreGridServer*> vecEnterGrid;
	//vector<CCoreGridServer*> vecLeaveGrid;

	int32 nDoubleAoiRadius = m_uViewAoiRadius<<1;

	if( ( nXOfs > nDoubleAoiRadius ) || ( nYOfs > nDoubleAoiRadius ) )
	{
		//两个矩形没有交集

		CIRect EnterRect;
		CIRect LeaveRect;

		EnterRect.left		=	NewGridPos.x	-	m_uViewAoiRadius;
		EnterRect.right		=	NewGridPos.x	+	m_uViewAoiRadius;
		EnterRect.top		=	NewGridPos.y	+	m_uViewAoiRadius;
		EnterRect.bottom	=	NewGridPos.y	-	m_uViewAoiRadius;

		LeaveRect.left		=	OldGridPos.x	-	m_uViewAoiRadius;
		LeaveRect.right		=	OldGridPos.x	+	m_uViewAoiRadius;
		LeaveRect.top		=	OldGridPos.y	+	m_uViewAoiRadius;
		LeaveRect.bottom	=	OldGridPos.y	-	m_uViewAoiRadius;

		EnterRect.right++;
		EnterRect.bottom--;
		LeaveRect.right++;
		LeaveRect.bottom--;

		QueryObject<FuncType, Func>(pEnterArg,EnterRect);
		QueryObject<FuncType, Func>(pLeaveArg,LeaveRect);

		//QueryGrid(vecEnterGrid,EnterRect);
		//QueryGrid(vecLeaveGrid,LeaveRect);
	}
	else
	{
		//两个矩形有交集

		CIRect	EnterHoRect,EnterVeRect;

		EnterHoRect.left	=	NewGridPos.x	-	m_uViewAoiRadius;
		EnterHoRect.right	=	NewGridPos.x	+	m_uViewAoiRadius;

		if( NewGridPos.x > OldGridPos.x )
		{
			EnterVeRect.left	=	OldGridPos.x	+	m_uViewAoiRadius	+	1;
			EnterVeRect.right	=	NewGridPos.x	+	m_uViewAoiRadius;
		}
		else
		{
			EnterVeRect.left	=	NewGridPos.x	-	m_uViewAoiRadius;
			EnterVeRect.right	=	OldGridPos.x	-	m_uViewAoiRadius	-	1;
		}

		if( NewGridPos.y > OldGridPos.y )
		{
			EnterHoRect.top		=	NewGridPos.y	+	m_uViewAoiRadius;
			EnterHoRect.bottom	=	OldGridPos.y	+	m_uViewAoiRadius	+	1;

			EnterVeRect.top		=	EnterHoRect.bottom						-	1;
			EnterVeRect.bottom	=	NewGridPos.y	-	m_uViewAoiRadius;
		}
		else
		{
			EnterHoRect.top		=	OldGridPos.y	-	m_uViewAoiRadius	-	1;
			EnterHoRect.bottom	=	NewGridPos.y	-	m_uViewAoiRadius;

			EnterVeRect.top		=	NewGridPos.y	+	m_uViewAoiRadius;
			EnterVeRect.bottom	=	EnterHoRect.top							+	1;
		}

		//将矩形的右边界和下边界加1，才能传给QueryObject
		EnterHoRect.right++;
		EnterHoRect.bottom--;
		EnterVeRect.right++;
		EnterVeRect.bottom--;

		QueryObject<FuncType, Func>(pEnterArg,EnterHoRect);
		QueryObject<FuncType, Func>(pEnterArg,EnterVeRect);

		//QueryGrid(vecEnterGrid,EnterHoRect);
		//QueryGrid(vecEnterGrid,EnterVeRect);


		CIRect	LeaveHoRect,LeaveVeRect;

		LeaveHoRect.left	=	OldGridPos.x	-	m_uViewAoiRadius;
		LeaveHoRect.right	=	OldGridPos.x	+	m_uViewAoiRadius;

		if( OldGridPos.x > NewGridPos.x )
		{
			LeaveVeRect.left	=	NewGridPos.x	+	m_uViewAoiRadius	+	1;
			LeaveVeRect.right	=	OldGridPos.x	+	m_uViewAoiRadius;
		}
		else
		{
			LeaveVeRect.left	=	OldGridPos.x	-	m_uViewAoiRadius;
			LeaveVeRect.right	=	NewGridPos.x	-	m_uViewAoiRadius	-	1;
		}

		if( OldGridPos.y > NewGridPos.y )
		{
			LeaveHoRect.top		=	OldGridPos.y	+	m_uViewAoiRadius;
			LeaveHoRect.bottom	=	NewGridPos.y	+	m_uViewAoiRadius	+	1;

			LeaveVeRect.top		=	LeaveHoRect.bottom						-	1;
			LeaveVeRect.bottom	=	OldGridPos.y	-	m_uViewAoiRadius;
		}
		else
		{
			LeaveHoRect.top		=	NewGridPos.y	-	m_uViewAoiRadius	-	1;
			LeaveHoRect.bottom	=	OldGridPos.y	-	m_uViewAoiRadius;

			LeaveVeRect.top		=	OldGridPos.y	+	m_uViewAoiRadius;
			LeaveVeRect.bottom	=	LeaveHoRect.top							+	1;
		}

		//将矩形的右边界和下边界加1，才能传给QueryObject
		LeaveHoRect.right++;
		LeaveHoRect.bottom--;
		LeaveVeRect.right++;
		LeaveVeRect.bottom--;

		QueryObject<FuncType, Func>(pLeaveArg,LeaveHoRect);
		QueryObject<FuncType, Func>(pLeaveArg,LeaveVeRect);

		//QueryGrid(vecLeaveGrid,LeaveHoRect);
		//QueryGrid(vecLeaveGrid,LeaveVeRect);
	}
}
